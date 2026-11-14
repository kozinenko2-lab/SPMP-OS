#!/usr/bin/env python3
"""Execute selected original FAT mutation wrappers in RAM, with fake disk calls.
No real filesystem mutation or MMIO. Error numbers are observations, not names.
"""
import argparse,json
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H

def audit(path):
 from unicorn.arm_const import UC_CPU_ARM_926
 blob=path.read_bytes();rows=[]
 def vm():return ArmVM(blob,cpu_model=UC_CPU_ARM_926)
 groups=[('fatfs_unlink',0x283858,0x2a99d0,[(0,0),(-8,21),(-17,200),(-123,5)]),('fatfs_mkdir',0x2838e8,0x2a7ea0,[(0,0),(-7,20),(-20,302),(-123,5)]),('fatfs_rmdir',0x2839a4,0x2a7c28,[(0,0),(-7,20),(-1609,22),(-123,5)]),('fatfs_rename',0x283a34,0x2ab6e8,[(0,0),(-17,200),(-20,302),(-16,0xfffffff0),(-123,0xffffff85)])]
 for name,entry,backend,cases in groups:
  for error,expected in cases:
   v=vm();old,new=0x1001000,0x1002000
   v.u.mem_write(old,b'/fat20a0/hda0/SPMP-OS/User/a.tmp\0');v.u.mem_write(new,b'/fat20a0/hda0/SPMP-OS/User/a.a26\0')
   translated=[]
   def translate(v,args):translated.append(v.string(args[1]));v.put32(args[0],args[1]);return 0
   def disk(v,args):
    assert args[0]==old
    if name=='fatfs_rename':assert args[1]==new
    if name=='fatfs_mkdir':assert args[1]==0
    return 0 if not error else 0xffffffff
   v.stub(0x2834dc,'path_translate',translate,2);v.stub(backend,'disk_operation',disk,2 if name in ('fatfs_rename','fatfs_mkdir') else 1)
   v.stub(0x2ab170,'error_get',error,0);v.stub(0x298b64,'diag',0,4)
   result=v.run(entry,[0,0,old,0],stack=[new] if name=='fatfs_rename' else [])
   assert result==expected,(name,error,result,expected)
   assert len(translated)==(2 if name=='fatfs_rename' else 1)
   rows.append({'case':name,'entry':hex(entry),'disk_error_get':error,'wrapper_return_u32':result,'translated_paths':len(translated)})
 for args in ([0],[0x1001000]):
  v=vm();assert v.im.word(0x284100)==0xe3a00000 and v.im.word(0x284104)==0xe12fff1e
  assert v.run(0x284100,args)==0 and not v.calls
  rows.append({'case':'fatfs_fsync_noop','entry':'0x284100','return':0,'backend_calls':0})
 # Low-level rename routes to a volume driver's +0x6c method. The wrapper
 # itself does not supply or check a NOREPLACE flag; driver semantics unresolved.
 for same_volume,result in [(True,0),(True,-1),(False,0)]:
  v=vm();old,new,volume,driver,other=0x1001000,0x1002000,0x1003000,0x1004000,0x1005000
  v.u.mem_write(old,b'a.tmp\0');v.u.mem_write(new,b'a.a26\0');v.put32(volume+0x30,0x1234);v.put32(volume+0x34,driver);v.put32(driver+0x6c,0x1201000)
  resolutions=[]
  def resolve(v,args):
   path=v.get32(args[0]);resolutions.append(path);v.put32(args[1],volume if same_volume or len(resolutions)==1 else other);return 0
  def strcpy(v,args):v.u.mem_write(args[0],v.string(args[1]).encode()+b'\0');return args[0]
  def rename(v,args):assert args[0]==0x1234 and v.string(args[1])=='a.tmp' and v.string(args[2])=='a.a26';return result
  for a,n,r,c in [(0x33a048,'lock',0,1),(0x33a05c,'unlock',0,1),(0x2abefc,'resolve_volume',resolve,2),(0x346058,'strcpy',strcpy,2),(0x1201000,'driver_rename',rename,3)]:v.stub(a,n,r,c)
  value=v.run(0x2ab6e8,[old,new]);assert value==(0 if same_volume and result==0 else 0xffffffff)
  assert sum(n=='driver_rename' for n,args in v.calls)==int(same_volume)
  assert v.calls[0][0]=='lock' and v.calls[-1][0]=='unlock'
  rows.append({'case':'rename_driver_dispatch','same_volume':same_volume,'driver_return':result,'return_u32':value,'driver_called':same_volume})
 return {'source_sha256':H['8000_MMI.RAP'],'case_count':len(rows),'cases':rows,'limits':['Original ARM wrapper/coordinator instructions; path translation, locks and disk/volume-driver calls are simulated.','No actual file, SD, NAND, Flash, GPIO or MMIO access.','OEM errors are not assumed to be Linux errno. No tested atomic NOREPLACE/durability binding yet.','The driver behind rename +0x6c still needs separate recovery; coordinator lock alone does not prove no-overwrite semantics.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=audit(a.rap);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'original ARM FAT wrapper/coordinator cases')
