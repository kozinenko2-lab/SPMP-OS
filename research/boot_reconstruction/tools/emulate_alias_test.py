#!/usr/bin/env python3
"""Execute the compiled read-only RAM-alias diagnostic on an ARM926 VM."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
from emulate_native_package import ProbeObserver,PROFILE,GLOBALS,DEFAULT_SP
from make_binding_profile import synthetic_prefix
from make_input_profile import synthetic_input_prefix

BASE=Path(__file__).resolve().parents[1]
INPUT=json.loads((BASE/'evidence/input_sd0_profile.json').read_text())


class AliasObserver(ProbeObserver):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs)
  self.targets.update({p['address']:p['name'] for p in INPUT['prefixes']})
  self.counts={};self.file=bytearray(b'PREVIOUS\n');self.position=0
  self.opened=0;self.closed=0;self.pixels=bytearray(320*240*3);self.preview=None
 def callback(self,name):
  args=[self.u.reg_read(r) for r in self.regs[:4]];self.counts[name]=self.counts.get(name,0)+1;result=0
  if self.u.reg_read(self.spreg)%8:raise AssertionError('API call stack alignment')
  fault=self.scenario.get('fault','')
  if name=='errno':
   self.u.mem_write(0x1006000,struct.pack('<I',2 if fault=='no_sd' else 0));result=0x1006000
  elif name=='delay':
   if args[:2]!=[50,0]:raise AssertionError('Unexpected delay')
  elif name=='open':
   path=bytes(self.u.mem_read(args[0],64)).split(b'\0')[0]
   if path!=b'/fat20a0/hda0/SPMPALI.CSV' or args[1:3]!=[10,0o600]:raise AssertionError('Path/flags')
   if fault=='no_sd':result=0xffffffff
   else:self.opened+=1;result=0
  elif name=='seek':
   if args[:3]!=[0,0,2]:raise AssertionError('Append seek')
   self.position=len(self.file);result=self.position
  elif name=='write':
   if args[0]!=0 or args[2]>160:raise AssertionError('Write bounds')
   data=bytes(self.u.mem_read(args[1],args[2]));self.file[self.position:self.position+len(data)]=data
   self.position+=len(data);result=len(data)
  elif name=='close':self.closed+=1
  elif name=='origin':
   if struct.unpack('<hh',self.u.mem_read(args[0],4))!=(0,0):raise AssertionError('Origin')
  elif name=='double_buffer':
   if args[0]!=1:raise AssertionError('Double buffer')
  elif name=='dimensions':self.output(args[0],struct.pack('<H',320));self.output(args[1],struct.pack('<H',240))
  elif name=='get_color':self.output(args[0],bytes(self.color))
  elif name=='set_color':self.color=list(self.u.mem_read(args[0],3))
  elif name=='fill':
   x,y,w,h=struct.unpack('<hhHH',self.u.mem_read(args[0],8))
   if min(x,y)<0 or x+w>320 or y+h>240:raise AssertionError('Rectangle bounds')
   row=bytes(self.color)*w
   for py in range(y,y+h):self.pixels[(py*320+x)*3:(py*320+x+w)*3]=row
  elif name=='paint':self.preview=bytes(self.pixels)
  elif name in ('init_graph','time','keys'):pass
  else:raise AssertionError('Unexpected callback '+name)
  for r in self.regs[1:4]+[self.regs[12]]:self.u.reg_write(r,0xa5a5a5a5)
  self.u.reg_write(self.regs[0],result);self.u.reg_write(self.pc,self.u.reg_read(self.lr))
 def result(self,value):
  if value!=0:raise AssertionError('Application return '+hex(value))
  address=self.image.address('spmp_alias_record')
  words=struct.unpack('<14I',self.u.mem_read(address,56))
  source,alias,result,mask=words[:4];observed=list(words[4:12]);log_status,log_errno=words[12:]
  expected=2 if self.scenario.get('corrupt_alias') else 0
  if result!=expected or bool(mask)!=bool(expected):raise AssertionError('Alias result/mask')
  if alias!=source|0x10000000:raise AssertionError('Alias address')
  expected_log=1 if self.scenario.get('fault')=='no_sd' else 0
  if log_status!=expected_log or log_errno!=(2 if expected_log else 0):raise AssertionError('Log result')
  if self.opened!=self.closed:raise AssertionError('File handle leak')
  if expected_log==0:
   if not self.file.startswith(b'PREVIOUS\nALIAS_V1,'):raise AssertionError('Log was not appended')
   if len(self.file.splitlines()[-1].split(b','))!=13:raise AssertionError('CSV columns')
  if self.color!=[8,128,248]:raise AssertionError('Foreground not restored')
  if self.get32(self.image.bss_end)!=0xdecafbad:raise AssertionError('BSS guard')
  return {'status':value,'alias_result':result,'mismatch_mask':hex(mask),'source':hex(source),'alias':hex(alias),
          'observed':[hex(v) for v in observed],'log_status':log_status,'log_errno':log_errno,
          'previous_log_preserved':expected_log!=0 or self.file.startswith(b'PREVIOUS\n'),'api_counts':self.counts}


def run(path,preview_path=None,log_path=None):
 from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
 from unicorn import arm_const as ar
 image=StartupELF(path.read_bytes());container,plain,manifest=pack(image.data,'SPMP ALIAS TEST');rows=[]
 cases=[{'name':'alias_pass'},{'name':'alias_mismatch','corrupt_alias':True},{'name':'sd_open_error','fault':'no_sd'}]
 for case in cases:
  u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
  u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000)
  u.mem_write(ENTRY,container);u.mem_write(ENTRY,plain)
  u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
  for p in PROFILE['prefixes']:u.mem_write(p['target'],synthetic_prefix(p['slot']))
  for p in INPUT['prefixes']:u.mem_write(p['address'],synthetic_input_prefix(p['address']))
  for address,value in GLOBALS.items():u.mem_write(address,struct.pack('<I',value))
  source=image.address('spmp_alias_seed');alias=source|0x10000000;page=alias&~0xfff
  u.mem_map(page,0x1000);seed=bytearray(u.mem_read(source,32))
  if case.get('corrupt_alias'):seed[7]^=1
  u.mem_write(alias,bytes(seed))
  observer=AliasObserver(u,image,DEFAULT_SP,case);saved=[0x45670000+4*i for i in range(8)]
  for r,value in zip(observer.regs[4:12],saved):u.reg_write(r,value)
  u.reg_write(ar.UC_ARM_REG_SP,DEFAULT_SP);u.reg_write(ar.UC_ARM_REG_LR,0x1200000);u.reg_write(observer.regs[0],PROFILE['api_base'])
  u.emu_start(ENTRY,0x1200000,count=3000000,timeout=10000000)
  if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=DEFAULT_SP or [u.reg_read(r) for r in observer.regs[4:12]]!=saved:
   raise AssertionError('Return ABI')
  rows.append({'case':case['name'],**observer.result(u.reg_read(observer.regs[0]))})
  if case['name']=='alias_pass':
   if preview_path:preview_path.write_bytes(b'P6\n320 240\n255\n'+(observer.preview or bytes(observer.pixels)))
   if log_path:log_path.write_bytes(bytes(observer.file))
 return {'execution':'ARM926 compiled NGame with synthetic OEM services and read-only mirrored alias page',
         'case_count':len(rows),'package':manifest,'cases':rows,
         'sha256':hashlib.sha256(container).hexdigest(),
         'limits':['The VM mirrors only the application seed page; it does not emulate RedBoot MMU/cache hardware.',
                   'No console, physical SD, MMIO, CEVA or Flash was accessed.']}


if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',required=True,type=Path)
 p.add_argument('--output',required=True,type=Path);p.add_argument('--preview',type=Path);p.add_argument('--log',type=Path);a=p.parse_args()
 result=run(a.elf,a.preview,a.log);a.output.write_text(json.dumps(result,indent=2)+'\n')
 print('PASS:',result['case_count'],'compiled alias-test scenarios')
