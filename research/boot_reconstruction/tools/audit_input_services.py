#!/usr/bin/env python3
"""Original ARM checks for newly used time/delay and direct file service ABI.
RAM-only mocked OS/filesystem backends; no SD or device I/O.
"""
import argparse,json,hashlib,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H,Image

def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();im=Image(blob);rows=[]
 def vm():return ArmVM(blob,cpu_model=UC_CPU_ARM_926)
 for low,high,base in [(123,0,100),(0x20,1,0xfffffff0),(5,17,9),(0xffffffff,0,0)]:
  v=vm();v.put32(0x556150,base)
  def clock(v,args):v.u.reg_write(v.regs[1],high);return low
  v.stub(0x339d18,'clock',clock)
  result=v.run(0x2d3254);expected=((low-base)*10)&0xffffffff
  assert result==expected;rows.append({'group':'time','tick_low':low,'tick_high':high,'base_low':base,'units':result})
 for lo,hi in [(2,0),(25,0),(0xffffffff,1)]:
  v=vm();v.put32(0x63e0f0,0x1003000);v.stub(0x33ad5c,'counter_delay',0,3)
  v.run(0x339be8,[lo,hi]);assert v.calls==[['counter_delay',[0x1003000,lo,hi]]]
  rows.append({'group':'delay','low':lo,'high':hi,'forwarded':v.calls[0][1]})
 # eCos open: fd0 is valid; flags are forwarded to the FS's open entry as r3.
 for flags in [1,0xa]:
  v=vm();pathaddr=0x1001000;obj=0x1002000;mount=0x1003000;fstab=0x1004000
  v.u.mem_write(pathaddr,b'/fat20a1/hda1/SPMPKEYS.CSV\0');v.put32(mount+20,fstab);v.put32(fstab+8,0);v.put32(fstab+20,0x1201000)
  def resolve(v,args):v.put32(args[0],0);v.put32(args[1],pathaddr);v.put32(args[2],mount);return 0
  def backend(v,args):assert args==[mount,0,pathaddr,flags];return 0
  for addr,name,result,n in [(0x2a5bf0,'fd_alloc',0,1),(0x2a5aa0,'file_alloc',obj,0),
    (0x281f48,'resolve',resolve,3),(0x28228c,'fs_lock',0,2),(0x282378,'fs_unlock',0,2),
    (0x2a5c6c,'assign_fd',0,2),(0x1201000,'fs_open',backend,4)]:v.stub(addr,name,result,n)
  assert v.run(0x2a5ed4,[pathaddr,flags,0o600])==0
  rows.append({'group':'absolute_open','fd':0,'flags':flags,'filesystem_mode_forwarded':flags})
 # FAT open explicitly ORs create+write for O_CREAT; no O_TRUNC bit is added.
 v=vm();v.u.mem_write(0x1001000,b'B:/SPMPKEYS.CSV\0')
 def path_convert(v,args):v.put32(args[0],0x1001000);return 0
 v.stub(0x2834dc,'path_convert',path_convert,2);v.stub(0x2a8264,'fat_backend_open',3,2)
 assert v.run(0x283788,[0x1003000,0,0x1001000,0xa],stack=[0x1002000])==0
 assert ['fat_backend_open',[0x1001000,0xa]] in v.calls
 rows.append({'group':'fat_create','flags_to_backend':10,'truncate_bit_set':False})
 for returned,error in [(64,0),(7,0),(0,0),(7,11),(0,5)]:
  v=vm();obj=0x1002000;ops=0x1003000;v.put32(obj,2);v.put32(obj+12,ops);v.put32(ops+4,0x1201000)
  def backend(v,args):
   assert args[0]==obj;uio=args[1];assert v.get32(uio+12)==64
   iov=v.get32(uio);assert [v.get32(iov),v.get32(iov+4)]==[0x1001000,64]
   v.put32(uio+12,64-returned);return error
  for addr,name,result,n in [(0x2a5d20,'lookup',obj,1),(0x2a5dac,'lock',0,2),(0x2a5e08,'unlock',0,2),
    (0x2a5d7c,'release',0,1),(0x299a14,'errno',0x1004000,0),(0x1201000,'write_backend',backend,2)]:v.stub(addr,name,result,n)
  result=v.run(0x2eb610,[0,0x1001000,64]);expected=returned if error==0 or error==11 and returned else 0xffffffff
  assert result==expected;rows.append({'group':'write','fd':0,'backend_bytes':returned,'error':error,'returned':result})
 for position,error in [(0,0),(12345,0),(0,5)]:
  v=vm();obj=0x1002000;ops=0x1003000;v.put32(obj+12,ops);v.put32(ops+8,0x1201000)
  def backend(v,args):assert args[0]==obj and args[2]==2 and v.get32(args[1])==0;v.put32(args[1],position);return error
  for addr,name,result,n in [(0x2a5d20,'lookup',obj,1),(0x2a5dac,'lock',0,2),(0x2a5e08,'unlock',0,2),
    (0x2a5d7c,'release',0,1),(0x299a14,'errno',0x1004000,0),(0x1201000,'seek_backend',backend,3)]:v.stub(addr,name,result,n)
  result=v.run(0x2eb69c,[0,0,2]);assert result==(position if not error else 0xffffffff)
  rows.append({'group':'seek_end','position':position,'backend_error':error,'returned':result})
 for backend in [0,5]:
  v=vm();v.stub(0x2a5d20,'lookup',0x1002000,1);v.stub(0x2a5d7c,'release',0,1)
  v.stub(0x2a5cc8,'close_backend',backend,1);v.stub(0x299a14,'errno',0x1004000)
  result=v.run(0x2eb640,[0]);assert result==(0 if not backend else 0xffffffff)
  rows.append({'group':'close','fd':0,'backend_error':backend,'returned':result})
 assert im.string(0x4fe924)=='/fat20a1' and im.string(0x4fac08)=='/hda1'
 assert im.literal(0x432008)==0x4fe924 and im.literal(0x432018)==0x4fac08
 assert im.word(0x2837ac)==0xe3140008 and im.word(0x2837b0)==0x1384400a
 if hashlib.sha256(path.read_bytes()).hexdigest()!=H['8000_MMI.RAP']:raise AssertionError('Original changed')
 return {'source_sha256':H['8000_MMI.RAP'],'cpu':'ARM926','case_count':len(rows),'cases':rows,
  'storage_path_evidence':{'stock_storage_selection':'0x3e2f08 reads byte 0x66cd4c',
   'game_base_branch':'0x432000 constructs /fat20a1/hda1/GAME/',
   'fat_fstab':'0x56a9dc open ->0x283788, O_CREAT propagation at 0x2837ac/7b0'},
  'limits':['No physical SD test; mount path follows this image stock browser branch.',
   'Native time returns low tick delta *10; physical tick period not measured.',
   'All underlying scheduler/FS/device backends are mocked RAM fixtures.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',required=True,type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
 r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original ARM input service cases')
