#!/usr/bin/env python3
"""Original ARM RAM input pipeline: getters, event setter, collector and TTL.
Scheduler/device/system effects mocked; no GPIO or console access.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from audit_boot import H
from arm_abi_vm import ArmVM
from native_abi_models import cooked_keys

def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();rows=[]
 def vm():
  v=ArmVM(blob,cpu_model=UC_CPU_ARM_926);v.stub(0x45ffbc,'native_kind',6);v.stub(0x2d8b78,'menu_active',0)
  v.put32(0x55bd04,0);v.u.mem_write(0x55bd50,b'\1');v.put32(0x693c58,15)
  for a,n,k in [(0x298b64,'diag',0),(0x2d6d60,'command',3),(0x3c4c5c,'gui',0),(0x2d8b88,'message',3),(0x46190c,'pause',1),(0x339be8,'delay',2)]:v.stub(a,n,0,k)
  return v
 for value in [0,0x40,0x340,0xffffffff]:
  v=vm();v.put32(0x556144,value);v.put32(0x556148,0x400);v.put32(0x55614c,2)
  assert v.run(0x2d2e40)==value and v.get32(0x556148)==0x400 and v.get32(0x55614c)==2
  rows.append({'case':'state_get','value':hex(value),'latch_and_ttl_unchanged':True})
  v=vm();v.put32(0x556144,0x40)
  assert v.run(0x2d2e50,[value])==0 and v.get32(0x556148)==value and v.get32(0x556144)==0x40
  rows.append({'case':'event_set_vm_only','value':hex(value),'state_unchanged':True})
 for bit in range(16):
  v=vm();state=1<<bit;v.put32(0x556144,state);v.put32(0x556148,0)
  expected=v.im.word(0x55bdac+4+bit*4)&~0x800
  raw=v.run(0x2d2e6c);assert raw==expected
  out=0x1001000;v.put32(out,0xabcdef01)
  assert v.run(0x2d2f7c,[out])==1
  cooked=v.get32(out+4);assert cooked==cooked_keys(6,raw,15,1)['keys'] and v.get32(out)==0xabcdef01
  rows.append({'case':'state_to_collector_to_cooked','state_bit':bit,'state':hex(state),'collector':hex(raw),'cooked':hex(cooked)})
 for value in [0x40,0x400,0x800,0x1000]:
  v=vm();v.put32(0x556144,0);v.put32(0x556148,value);v.put32(0x55614c,0)
  # Passive reads preserve the event. Collector calls consume its TTL.
  for _ in range(4):assert v.run(0x2d2e40)==0 and v.get32(0x556148)==value
  actual=[v.run(0x2d2e6c) for _ in range(5)]
  assert actual==[value]*4+[0] and v.get32(0x556148)==0
  rows.append({'case':'latch_ttl','event':hex(value),'collector_returns':[hex(x) for x in actual]})
 v=vm();v.stub(0x2d8b78,'menu_active',1);v.put32(0x556144,0x40);v.put32(0x556148,0x400)
 assert v.run(0x2d2e6c)==0 and v.get32(0x556144)==0 and v.get32(0x556148)==0
 rows.append({'case':'menu_clear','before_state':'0x40','before_event':'0x400','after_state':0,'after_event':0})
 assert hashlib.sha256(path.read_bytes()).hexdigest()==H['8000_MMI.RAP']
 return {'source_sha256':H['8000_MMI.RAP'],'cpu':'ARM926','case_count':len(rows),'cases':rows,
  'limits':['Original RAM state/event pipeline only. No physical key labels inferred from table indices.',
   'Event setter executes only in this RAM VM; device app snapshot only reads these globals.',
   'Menu flag/native kind/OS/system actions mocked; no GPIO, firmware writes or console execution.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original raw RAM input cases')
