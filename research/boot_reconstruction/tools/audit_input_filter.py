#!/usr/bin/env python3
"""Exact-RAP ARM audit of all 32 precollected single-bit key masks, kind6.
No physical GPIO/key identities inferred; all system/volume/pause effects mocked.
"""
import argparse,hashlib,json
from pathlib import Path
from audit_boot import H
from arm_abi_vm import ArmVM

def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();rows=[]
 mapping={0:1,1:2,2:4,3:8,4:0x10000,5:0x20000,11:0x2000,19:0x80000}
 for bit in range(32):
  raw=1<<bit;v=ArmVM(blob,cpu_model=UC_CPU_ARM_926)
  v.stub(0x2d2e6c,'collector',raw);v.stub(0x45ffbc,'native_kind',6)
  v.put32(0x55bd04,0);v.put32(0x693c58,15);v.u.mem_write(0x55bd50,b'\0')
  for address,name,nargs in [(0x298b64,'diag',0),(0x2d6d60,'command',3),(0x3c4c5c,'gui_action',0),
   (0x2d8b88,'message',3),(0x46190c,'pause',1),(0x339be8,'delay',2),
   (0x3e30cc,'save_volume',1),(0x2c4de8,'set_volume',1)]:v.stub(address,name,0,nargs)
  out=0x1001000;v.put32(out,0xabcdef01);v.put32(out+4,0xdeadbeef)
  assert v.run(0x2d2f7c,[out])==1 and v.get32(out)==0xabcdef01
  cooked=v.get32(out+4);assert cooked==mapping.get(bit,0)
  volume=v.get32(0x693c58);assert volume==15+(-1 if bit==8 else 1 if bit==9 else 0)
  effects=[{'name':n,'args':a} for n,a in v.calls if n not in ('collector','native_kind','diag')]
  if bit==12:assert [e['name'] for e in effects]==['command','gui_action','message','pause','delay']
  elif bit not in (8,9):assert not effects
  rows.append({'collector_mask':f'0x{raw:08X}','cooked_mask':f'0x{cooked:08X}',
   'classification':'game_mask' if cooked else 'volume' if bit in (8,9) else 'system_pause' if bit==12 else 'no_game_output',
   'volume_before':15,'volume_after':volume,'mocked_effects':effects})
 assert hashlib.sha256(path.read_bytes()).hexdigest()==H['8000_MMI.RAP']
 return {'source_sha256':H['8000_MMI.RAP'],'execution':'Original ARM926 native_get_key_input and cooked core; injected collector mask/native kind6',
  'case_count':len(rows),'cases':rows,'limits':['Collector and OS/GUI/volume/pause backends are intercepted; no physical key/GPIO access.',
   'Collector masks are not device scan codes and cannot be assigned physical labels from this audit.',
   'Input globals 0x556144/0x556148 are upstream RAM state/event words; they were not captured in the user V1 log.',
   'Embedded image SHA is validated offline; no runtime board memory trace.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original ARM kind6 input-filter cases')
