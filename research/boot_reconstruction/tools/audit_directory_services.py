#!/usr/bin/env python3
"""Original eCos directory/stat ARM execution; RAM fixtures, no physical SD."""
import argparse,json,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from make_files_profile import produce
def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();profile=produce(blob);rows=[]
 def vm():return ArmVM(blob,cpu_model=UC_CPU_ARM_926)
 for fd,error in [(1,0),(7,0),(-1,24),(1,5)]:
  v=vm();pa,obj,mount,fs,err=0x1001000,0x1002000,0x1003000,0x1004000,0x1005000
  v.u.mem_write(pa,b'/fat20a0/hda0/SPMP-OS/User\0');v.put32(mount+20,fs);v.put32(fs+44,0x1201000)
  def alloc(v,args):assert args==[1];return fd
  def resolve(v,args):v.put32(args[0],0);v.put32(args[1],pa);v.put32(args[2],mount);return 0
  def backend(v,args):assert args==[mount,0,pa,obj];return error
  def assign(v,args):assert args==[fd,obj];return 0
  for a,n,r,c in [(0x2a5bf0,'alloc_fd',alloc,1),(0x2a5aa0,'alloc_file',obj,0),(0x281f48,'resolve',resolve,3),(0x28228c,'lock',0,2),(0x282378,'unlock',0,2),(0x1201000,'opendir_backend',backend,4),(0x2a5c6c,'assign',assign,2),(0x2a5cc8,'free_fd',0,1),(0x2a5b0c,'free_file',0,1),(0x299a14,'errno',err,0)]:v.stub(a,n,r,c)
  result=v.run(0x2a69bc,[pa]);assert result==(fd if fd>0 and not error else 0)
  if result:assert v.get32(obj)&0x20001==0x20001
  else:assert v.get32(err)==(24 if fd<0 else error)
  rows.append({'case':'opendir','allocated_fd':fd,'backend_error':error,'result':result})
 for count,error,fd in [(260,0,1),(0,0,1),(-1,5,1),(260,0,0)]:
  v=vm();entry,result,err=0x1001000,0x1002000,0x1003000;v.put32(result,0xaabbccdd)
  def read(v,args):
   assert args==[fd,entry,260]
   if count>0:v.u.mem_write(entry,b'Adventure.a26\0')
   return count
  v.put32(err,error);v.stub(0x2eb5e0,'read',read,3);v.stub(0x299a14,'errno',err)
  value=v.run(0x2a6b8c,[fd,entry,result]);assert value==(9 if fd==0 else error)
  assert v.get32(result)==(entry if fd and count>0 else 0)
  rows.append({'case':'readdir_r','fd':fd,'backend_count':count,'backend_error':error,'status':value,'entry_returned':bool(v.get32(result))})
 for fd in [1,7]:
  v=vm();v.stub(0x2eb640,'close',0,1);assert v.run(0x2a6be0,[fd])==0 and v.calls==[['close',[fd]]]
  rows.append({'case':'closedir','fd':fd,'forwarded_to_close':True})
 for mode,size,error in [(1,0,0),(8,4096,0),(8,123456,0),(0,0,2)]:
  v=vm();pa,st,mount,fs,err=0x1001000,0x1002000,0x1003000,0x1004000,0x1005000
  v.u.mem_write(pa,b'/fat20a0/hda0/SPMP-OS/User/test.a26\0');v.u.mem_write(st,b'\xa5'*36);v.put32(mount+20,fs);v.put32(fs+52,0x1201000)
  def resolve(v,args):v.put32(args[0],0);v.put32(args[1],pa);v.put32(args[2],mount);return 0
  def backend(v,args):
   assert args==[mount,0,pa,st]
   if not error:v.u.mem_write(st,struct.pack('<8I',mode,0,0,0,size,0,0,0))
   return error
  for a,n,r,c in [(0x281f48,'resolve',resolve,3),(0x28228c,'lock',0,2),(0x282378,'unlock',0,2),(0x1201000,'stat_backend',backend,4),(0x299a14,'errno',err,0)]:v.stub(a,n,r,c)
  result=v.run(0x2a6504,[pa,st]);assert result==(0 if not error else 0xffffffff) and v.get32(st+32)==0xa5a5a5a5
  if not error:assert v.get32(st)==mode and v.get32(st+16)==size
  rows.append({'case':'stat','mode':mode,'size':size,'backend_error':error,'result':result,'guard_preserved':True})
 return {'source_sha256':profile['source_sha256'],'case_count':len(rows),'cases':rows,'limits':['Original wrappers execute; OS locks, mount resolution and FAT backend are simulated.','No console/SD/NAND or GPIO accessed. FAT mode/size population also checked against selected disassembly.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original directory/stat cases')
