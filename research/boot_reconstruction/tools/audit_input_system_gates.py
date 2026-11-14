#!/usr/bin/env python3
"""Exact-RAP HOLD gate / POWER suppression / GUI lock audit.
All OS effects intercepted; any MMIO region here is RAM-only, not a device.
"""
import argparse,hashlib,json
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H

def audit(path):
 from unicorn import UC_HOOK_MEM_WRITE
 from unicorn import arm_const as ar
 blob=path.read_bytes();rows=[]
 def vm():return ArmVM(blob,cpu_model=ar.UC_CPU_ARM_926)
 # Power reaches the system pause path only with the collector's gate clear.
 for gate in [0,1]:
  v=vm();v.stub(0x45ffbc,'native_kind',6);v.put32(0x55bd04,0)
  v.put32(0x556aa0,gate);v.put32(0x556144,0);v.put32(0x556148,0x1000);v.put32(0x55614c,0)
  for a,n,k in [(0x298b64,'diag',0),(0x2d6d60,'command',3),(0x3c4c5c,'gui_action',0),
   (0x2d8b88,'message',3),(0x46190c,'pause',1),(0x339be8,'delay',2)]:v.stub(a,n,0,k)
  out=0x1001000;assert v.run(0x2d2f7c,[out])==1 and v.get32(out+4)==0
  effects=[{'name':n,'args':a} for n,a in v.calls if n not in ('native_kind','diag')]
  if gate:assert not effects and v.get32(0x556148)==0
  else:assert [e['name'] for e in effects]==['command','gui_action','message','pause','delay'] and effects[-1]['args']==[30,0]
  rows.append({'case':'power_gate','gate':gate,'event_before':'0x00001000','event_after':hex(v.get32(0x556148)),'cooked':0,'effects_mocked':effects})
 # A normal held matrix key is also cleared by the gate, independently of GUI lock.
 for gate in [0,1]:
  v=vm();v.stub(0x45ffbc,'native_kind',6);v.put32(0x55bd04,0);v.u.mem_write(0x55bd50,b'\1')
  v.put32(0x556aa0,gate);v.put32(0x556144,0x20);v.put32(0x556148,0);v.stub(0x298b64,'diag',0)
  out=0x1001000;assert v.run(0x2d2f7c,[out])==1 and v.get32(out+4)==(0 if gate else 0x10000)
  rows.append({'case':'matrix_gate','gate':gate,'state_before':'0x20','state_after':hex(v.get32(0x556144)),'cooked':hex(v.get32(out+4))})
 # HOLD messages change the collector gate immediately; forwarding depends on game mode.
 for active in [0,1]:
  for message,expected in [(0x573,1),(0x574,0)]:
   v=vm();v.stub(0x298b64,'diag',0);v.stub(0x2d3d28,'native_active',active)
   v.stub(0x2d89e0,'queue_message',0,1);v.stub(0x339f3c,'queue_flag',0,2)
   v.put32(0x556aa0,1-expected);v.run(0x2d8b88,[message,0,0])
   queued=any(n=='queue_message' for n,a in v.calls)
   assert v.get32(0x556aa0)==expected and queued==(active==0)
   rows.append({'case':'hold_message','native_active':active,'message':hex(message),'gate_after':expected,'queued_to_gui':queued})
 for value in [0,1,2]:
  v=vm();v.put32(0x556aa0,7);v.run(0x3c3254,[value]);assert v.run(0x3c3264)==value and v.get32(0x556aa0)==7
  rows.append({'case':'gui_lock_word','value':value,'address':'0x561078','collector_gate_unchanged':True})
 # Isolate an original worker block with the parent function's proven register context.
 for before,after,msg in [(0,1,0x573),(1,0,0x574)]:
  v=vm();v.u.mem_write(0x556869,bytes([before]));v.stub(0x298b64,'diag',0);v.stub(0x2d8b88,'hold_message',0,3)
  assert v.im.word(0x2d5160)==0xe3a0a001 and v.im.word(0x2d5164)==0xe1a07005
  v.u.reg_write(ar.UC_ARM_REG_R7,0);v.u.reg_write(ar.UC_ARM_REG_R10,1)
  v.u.reg_write(ar.UC_ARM_REG_SP,0x1110000)
  v.u.emu_start(0x2d524c,0x2d52a8,count=1000,timeout=1000000)
  assert v.u.reg_read(ar.UC_ARM_REG_PC)==0x2d52a8 and bytes(v.u.mem_read(0x556869,1))==bytes([after])
  assert ['hold_message',[msg,0,0]] in v.calls
  rows.append({'case':'hold_worker_toggle_block','initial_byte':before,'final_byte':after,'byte_address':'0x556869','message':hex(msg),'context':'Parent r7=0/r10=1, execute original 0x2d524c..0x2d52a8; not full worker or gesture timing'})
 # The stock HOLD reader configures registers as well as reading GPIO input.
 # Demonstrate this in an explicitly mapped RAM fixture; never call it on the board.
 for value in [0,0x4000]:
  v=vm();v.u.mem_map(0x90000000,0x10000);v.put32(0x9000a064,value);writes=[]
  def write(u,access,address,size,n,user):
   if 0x90000000<=address<0x90010000:writes.append(hex(address))
  v.u.hook_add(UC_HOOK_MEM_WRITE,write);v.stub(0x32f034,'pin_config_mock',0,4)
  result=v.run(0x34f6dc);assert result==bool(value)
  assert writes==['0x9000a034','0x9000a004','0x9000a014','0x9000a03c']
  rows.append({'case':'stock_hold_reader_RAM_MMIO_fixture','input_address':'0x9000a064','input_bit':'0x4000','fixture_value':hex(value),'returned':result,'register_stores_observed':writes,'not_read_only':True})
 assert hashlib.sha256(path.read_bytes()).hexdigest()==H['8000_MMI.RAP']
 return {'source_sha256':H['8000_MMI.RAP'],'case_count':len(rows),'cases':rows,
  'limits':['All test execution in ARM926 RAM VM; OS/GUI/pause/pin config backends intercepted.',
   'MMIO addresses mapped to synthetic RAM only. No console GPIO reads or writes.',
   'The hardware V2 log lacks gate/GUI-lock/HOLD software byte, so actual flag values during its runs are unknown.',
   'Gate suppression is a source-proven explanation compatible with the report, not a measured root cause.',
   'Worker block has explicit initial register context; full timing/board polarity not established.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original system input gate cases')
