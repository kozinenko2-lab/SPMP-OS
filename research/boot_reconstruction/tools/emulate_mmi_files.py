#!/usr/bin/env python3
"""Compiled two-panel FM and real MMI loader; virtual read-only SD fixtures only."""
import argparse,json,struct,urllib.parse
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
import emulate_mmi_shell as shell
from emulate_native_package import PROFILE,GLOBALS,DEFAULT_SP,private_rap
from emulate_input import INPUT
from make_binding_profile import synthetic_prefix as base_prefix
from make_input_profile import synthetic_input_prefix
from make_raw_profile import synthetic_prefix as raw_prefix,LITERALS
from make_shell_profile import synthetic_prefix as shell_prefix
from make_files_profile import synthetic_prefix as files_prefix,produce
BASE=Path(__file__).resolve().parents[1];FILES=json.loads((BASE/'evidence/files_profile.json').read_text())
TITLE='SPMP OS FILES';shell.TITLE=TITLE
ROOT='/fat20a0/hda0';USER=ROOT+'/SPMP-OS/User'
FONT_PATH=ROOT+'/SPMP-OS/System/Fonts/Roboto-Regular.ttf'
FONT=(BASE.parents[1]/'project/spmp-os/assets/fonts/Roboto-Regular.ttf').read_bytes()
TREE={ROOT:['SPMP-OS'],ROOT+'/SPMP-OS':['System','User'],ROOT+'/SPMP-OS/System':['Cores','Fonts','Lib'],ROOT+'/SPMP-OS/System/Cores':['Atari2600'],ROOT+'/SPMP-OS/System/Lib':[],ROOT+'/SPMP-OS/System/Fonts':['Roboto-Regular.ttf'],USER:['ROMs','Logs'],USER+'/Logs':[],USER+'/ROMs':['Atari2600'],USER+'/ROMs/Atari2600':['Пример renamed.a26']}
def steps(masks):return [{}]+[v for mask in masks for v in ({'state':mask},{})]
NAV=steps([4,0x20,0x20,0x20,0x80,0x100,0x400,0x2000,0x2000,0x1000,0x80])
class FilesObserver(shell.ShellObserver):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs);self.targets.update({p['address']:p['name'] for p in FILES['prefixes']})
  self.dir_handle=False;self.dir_path='';self.dir_index=0;self.dir_opens=0;self.dir_closes=0;self.paths=set()
  self.font_open=False;self.font_opens=0;self.font_closes=0;self.font_position=0;self.font_reads=0
  self.preview_cyrillic=False
 def cstring(self,address):return bytes(self.u.mem_read(address,512)).split(b'\0')[0].decode('utf-8')
 def code(self,u,address,size,user):
  if address==ENTRY and hasattr(self,'firmware_vm'):
   for p in FILES['prefixes']:self.firmware_vm.stubs.pop(p['address'],None)
  super().code(u,address,size,user)
 def mutate(self):
  super().mutate()
  if 'files_prefix' in self.scenario:
   a=FILES['prefixes'][self.scenario['files_prefix']]['address'];self.u.mem_write(a,bytes([self.u.mem_read(a,1)[0]^1]))
 def callback(self,name):
  args=[self.u.reg_read(r) for r in self.regs[:4]];fault=self.scenario.get('fault','')
  if name in ('opendir','readdir_r','closedir','stat','read'):
   self.counts[name]=self.counts.get(name,0)+1;value=0
   if name=='opendir':
    path=self.cstring(args[0])
    if not (path==ROOT or path.startswith(ROOT+'/')):raise AssertionError('Non-SD directory request')
    self.paths.add(path)
    if path not in TREE or fault=='no_sd':value=0
    else:
     if self.dir_handle:raise AssertionError('Directory handle leaked/reused')
     self.dir_handle=True;self.dir_opens+=1;self.dir_path=path;self.dir_index=0;value=1
   elif name=='readdir_r':
    if args[0]!=1 or not self.dir_handle:raise AssertionError('Directory fd')
    self.output(args[2],struct.pack('<I',0));names=TREE[self.dir_path]
    if fault=='directory_read':value=5
    elif self.dir_index<len(names):
     name='../escape' if fault=='bad_name' else names[self.dir_index];self.dir_index+=1
     data=name.encode()+b'\0';self.output(args[1],data+b'\0'*(260-len(data)));self.output(args[2],struct.pack('<I',args[1]))
   elif name=='stat':
    path=self.cstring(args[0]);parent,_,name=path.rpartition('/')
    if parent not in TREE or name not in TREE[parent] or fault in ('directory_stat','no_sd') or path==FONT_PATH and fault=='font_missing':value=0xffffffff
    else:self.output(args[1],struct.pack('<8I',1 if path in TREE else 8,0,0,0,0 if path in TREE else len(FONT) if path==FONT_PATH else 4096,0,0,0))
   elif name=='closedir':
    if args[0]!=1 or not self.dir_handle:raise AssertionError('Closing bad directory')
    self.dir_handle=False;self.dir_closes+=1
    if fault=='directory_close':value=0xffffffff
   else:
    if args[0]!=2 or not self.font_open:raise AssertionError('Browser attempted non-font file payload read')
    count=min(args[2],len(FONT)-self.font_position);data=FONT[self.font_position:self.font_position+count]
    if fault=='font_bad' and self.font_position==0:data=b'BAD!'+data[4:]
    self.output(args[1],data);self.font_position+=count;self.font_reads+=1;value=count
   self.finish(value);return
  if name=='open':
   self.counts[name]=self.counts.get(name,0)+1
   if self.cstring(args[0])==FONT_PATH:
    if args[1:3]!=[1,0] or self.font_open:raise AssertionError('Font open flags/handle')
    self.font_open=True;self.font_opens+=1;self.font_position=0;self.finish(2);return
   if self.cstring(args[0])!=USER+'/Logs/SPMPFM.CSV' or args[1:3]!=[10,0o600]:raise AssertionError('FM journal path/flags')
   if fault=='no_sd':value=0xffffffff
   else:self.opened+=1;value=0
   self.finish(value);return
  if name=='seek' and args[0]==2:
   if not self.font_open or args[2]!=0 or args[1]>len(FONT):raise AssertionError('Font seek')
   self.font_position=args[1];self.finish(self.font_position);return
  if name=='close' and args[0]==2:
   if not self.font_open:raise AssertionError('Font close twice')
   self.font_open=False;self.font_closes+=1;self.finish(0);return
  super().callback(name)
  if name=='paint' and self.scenario.get('render') and self.dir_path==USER+'/ROMs/Atari2600' and not self.preview_cyrillic:
   self.preview=bytes(self.pixels);self.preview_cyrillic=True
 def result(self,value):
  r=super().result(value)
  if value>=0x900:return r
  if self.dir_handle or self.dir_opens!=self.dir_closes:raise AssertionError('FM directory leak')
  if self.font_open or self.font_opens!=self.font_closes:raise AssertionError('Font handle leak')
  mode=self.get32(self.image.address('spmp_files_font_mode'))
  expected_font=0 if self.scenario.get('fault') in ('no_sd','directory_stat','font_missing','font_bad') else 1
  if mode!=expected_font:raise AssertionError('Direct TTF/fallback mode '+repr((mode,expected_font)))
  comments=[line for line in self.file.decode().splitlines() if line.startswith('#PATH,')]
  selected=urllib.parse.unquote(''.join(line.split(',',2)[2] for line in comments))
  if self.scenario.get('selected') and selected!='SPMP-OS/User/ROMs/Atari2600/Пример renamed.a26':raise AssertionError('Selected external/UTF8 path: '+selected)
  if self.scenario.get('selected') and USER+'/ROMs/Atari2600' not in self.paths:raise AssertionError('Nested ROM directory not visited')
  return {**r,'directory_opens':self.dir_opens,'directory_closes':self.dir_closes,'visited_directories':sorted(self.paths),'selected_path':selected,'user_file_payload_reads':0,'font_reads':self.font_reads,'font_mode':mode,'font_opens':self.font_opens,'font_closes':self.font_closes,'filesystem_mode':'directory/stat and system TTF reads; journal appends separately'}
def cases():
 short=steps([0x1000,0x80])
 return [{'name':'navigate_select','script':NAV,'views':[0,4,5],'selected':True,'render':True},
  {'name':'power_continue_exit','script':shell.POWER,'views':[3,4,5],'power_rows':2},
  {'name':'refresh_panels','script':steps([0x10,0x100,0x400,0x2000,0x1000,0x80]),'views':[0,4,5]},
  {'name':'clock_wrap','clock_start':0xfffffff0,'script':short},
  {'name':'font_missing','fault':'font_missing','script':short},{'name':'font_bad','fault':'font_bad','script':short},
  *({'name':f,'fault':f,'script':short} for f in ['no_sd','short_write','write','close','directory_read','directory_stat','directory_close','bad_name']),
  {'name':'time_cap','script':[],'reason':2},
  *({'name':'files_prefix_'+str(i),'files_prefix':i,'status':0x90a} for i in range(5))]
def synthetic(path,preview=None,log=None):
 from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
 from unicorn import arm_const as ar
 image=StartupELF(path.read_bytes());_,plain,manifest=pack(image.data,TITLE);rows=[]
 for case in cases():
  u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
  u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000);u.mem_write(ENTRY,plain)
  u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
  for p in PROFILE['prefixes']:u.mem_write(p['target'],base_prefix(p['slot']))
  for p in INPUT['prefixes']:u.mem_write(p['address'],synthetic_input_prefix(p['address']))
  for p in shell.RAW['prefixes']:u.mem_write(p['address'],raw_prefix(p['address'],p['bytes']))
  for a,v in LITERALS.items():u.mem_write(a,struct.pack('<I',v))
  for p in shell.SHELL['prefixes']:u.mem_write(p['address'],shell_prefix(p['address']))
  for p in FILES['prefixes']:u.mem_write(p['address'],files_prefix(p['address'],p['bytes']))
  for a,v in GLOBALS.items():u.mem_write(a,struct.pack('<I',v))
  observer=FilesObserver(u,image,DEFAULT_SP,case);saved=[0x45670000+4*i for i in range(8)]
  for reg,v in zip(observer.regs[4:12],saved):u.reg_write(reg,v)
  u.reg_write(ar.UC_ARM_REG_SP,DEFAULT_SP);u.reg_write(ar.UC_ARM_REG_LR,0x1200000);u.reg_write(observer.regs[0],PROFILE['api_base'])
  u.emu_start(ENTRY,0x1200000,count=100000000,timeout=60000000)
  if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=DEFAULT_SP or [u.reg_read(reg) for reg in observer.regs[4:12]]!=saved:raise AssertionError('FM ABI/return/VM limit: '+case['name'])
  rows.append({'case':case['name'],**observer.result(u.reg_read(observer.regs[0]))})
  if case['name']=='navigate_select':
   if preview:preview.write_bytes(b'P6\n320 240\n255\n'+(observer.preview or bytes(observer.pixels)))
   if log:log.write_bytes(bytes(observer.file))
 return {'execution':'Compiled ARM926 FM01 integration, synthetic MMI services/read-only SD/graphics','case_count':len(rows),'package':manifest,'cases':rows,'limits':['No physical console/card accessed. Directory, stat, clock and graphics backends simulated.','No write/move/delete application operation; only diagnostic journal appends.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for key in ['elf','output']:p.add_argument('--'+key,type=Path,required=True)
 for key in ['rap','preview','log']:p.add_argument('--'+key,type=Path)
 a=p.parse_args()
 if a.rap:
  if produce(a.rap.read_bytes())!=FILES:raise ValueError('Directory profile mismatch')
  r=private_rap(a.elf,a.rap,observer_class=FilesObserver,title=TITLE,cases=[cases()[0],cases()[1],{'name':'no_sd','fault':'no_sd','script':steps([0x1000,0x80])}],max_instructions=200000000,timeout_us=120000000)
  r['execution']='Original RAP loader/collector -> compiled file manager -> normal return'
  r['mocked'].extend(['directory/stat/read-only SD fixtures','upstream input state/event/gates','time and journal services'])
 else:r=synthetic(a.elf,a.preview,a.log)
 a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'compiled file manager scenarios')
