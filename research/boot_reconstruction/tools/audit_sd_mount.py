#!/usr/bin/env python3
"""Trace exact RAP SD identity and real mount resolver/open/path translation in RAM.
No device mounting occurs; OS/physical FAT backend boundaries are intercepted.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H

def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();rows=[]
 def vm():return ArmVM(blob,cpu_model=UC_CPU_ARM_926)
 # Follow driver registration rather than ambiguous system-init diagnostics.
 v=vm();v.put32(0x513ee4,0)
 v.stub(0x298b64,'diag',0,1);v.stub(0x2b84b0,'register_read',0,2)
 v.stub(0x2b84e8,'register_write',0,2);v.stub(0x2bab1c,'sd_controller_init',0,0)
 assert v.run(0x288348)==1
 assert v.get32(0x513cac)==0x2884c0 and v.get32(0x513cbc)==0x28850c
 assert v.im.string(0x4d13a0).startswith('fat_sd_init()')
 assert [args[0] for name,args in v.calls if name in ('register_read','register_write')]==[0,0]
 rows.append({'case':'sd_driver_registration','storage_id':0,'mount_callback':'0x2884c0','unmount_callback':'0x28850c'})
 assert v.im.string(0x4d1518).startswith('fat_nf_init()')
 assert v.im.word(0x28873c)==0xe3a00002 and v.im.literal(0x288740)==0x288ac4
 assert v.im.string(0x4d3a48)=='fat_msdc_init' and v.im.word(0x288c78)==0xe3a00001
 rows.append({'case':'nand_registration_static','storage_id':2,'instruction':'0x28873c mov r0,2','mount_callback':'0x288ac4','executed':False})
 # Actual storage builder reads the image's device table and selects fat20a0 for SD.
 for device,mount in [(0,'/fat20a0'),(2,'/fat20a2')]:
  v=vm();observed={}
  def sprintf(v,args):
   dest,fmt,source,part=args
   assert v.string(fmt)=='%s:%d';text=v.string(source)+':'+str(part)
   v.u.mem_write(dest,text.encode()+b'\0');return len(text)
  def command(v,args):
   assert args[0]==0x601 and args[2]==12
   pointers=[v.get32(args[1]+4*i) for i in range(3)]
   observed.update(device=v.string(pointers[0]),mount=v.string(pointers[1]),fs=v.string(pointers[2]))
   assert observed['mount']==mount and observed['fs']=='fat2_0';return 0
  v.stub(0x33e2e0,'sprintf',sprintf,4);v.stub(0x2d6d60,'mount_command',command,3)
  assert v.run(0x4382b4,[device,0,0,0])==0
  rows.append({'case':'storage_mount_builder','storage_id':device,**observed})
 # Run real eCos open, real resolver/prefix matching, real FAT open/path conversion.
 # Only one active mount is installed: the SD mount proven above (index 0). The old path
 # must fail resolution BEFORE touching the FAT backend; the corrected path opens.
 for root,ok in [('/fat20a1/hda1',False),('/fat20a0/hda0',True)]:
  v=vm();mount=0x56ab04;obj=0x1002000;pathaddr=0x1001000;tcb=0x1005000;observed={}
  v.u.mem_write(0x56ab04,b'\0'*(0x56ac00-0x56ab04))
  v.put32(mount,0x4cfab8);v.put32(mount+16,1);v.put32(mount+20,0x56a9dc)
  v.put32(0x63e0f0,tcb);v.put32(0x55b734,0)
  v.u.mem_write(pathaddr,(root+'/SPMPKEYS.CSV').encode()+b'\0')
  def backend(v,args):
   observed.update(path=v.string(args[0]),flags=args[1])
   assert observed=={'path':'/hda0/SPMPKEYS.CSV','flags':10};return 3
  for address,name,result,nargs in [(0x2a5bf0,'fd_alloc',0,1),(0x2a5aa0,'file_alloc',obj,0),
   (0x28228c,'lock',0,2),(0x282378,'unlock',0,2),(0x2a5c6c,'assign_fd',0,2),
   (0x2a5cc8,'fd_release',0,1),(0x2a5b0c,'file_release',0,1),(0x2a8264,'fat_backend_open',backend,2)]:
   v.stub(address,name,result,nargs)
  result=v.run(0x2a5ed4,[pathaddr,10,0o600])
  assert result==(0 if ok else 0xffffffff)
  assert bool(observed)==ok
  location=v.run(0x299a14);assert location==tcb+0x88
  assert v.get32(location)==(0 if ok else 2)
  rows.append({'case':'real_resolver_open','request_root':root,'opened':ok,'errno':v.get32(location),'fat_backend':observed})
 for error in [2,5,9,28]:
  v=vm();v.put32(0x63e0f0,0x1005000);v.put32(0x55b734,0);v.put32(0x1005088,error)
  location=v.run(0x299a14);assert location==0x1005088 and v.get32(location)==error
  rows.append({'case':'thread_errno_location','offset':0x88,'value':error})
 assert hashlib.sha256(path.read_bytes()).hexdigest()==H['8000_MMI.RAP']
 return {'source_sha256':H['8000_MMI.RAP'],'cpu':'ARM926','case_count':len(rows),'cases':rows,
  'conclusion':'SD driver registers identity 0: /fat20a0/hda0. Identity 1 is MSDC; identity 2 is NAND.',
  'limits':['Mount state is a RAM fixture derived from original code, not a live console capture.',
   'Stock mount command, SD controller init, driver read/write registration, OS locking/FD resources and physical FAT backend are intercepted.',
   'Real resolver/open/FAT path translation and errno arithmetic execute original ARM instructions.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original SD mount/resolver/errno scenarios')
