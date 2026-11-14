#!/usr/bin/env python3
"""Compiled interactive input/journal ARM926 checks, virtual SD only.
No real filesystem card/console access; optional exact RAP loading is isolated.
"""
import argparse,json,struct,hashlib
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
from emulate_native_package import ProbeObserver,PROFILE,GLOBALS,DEFAULT_SP,private_rap
from make_binding_profile import synthetic_prefix
from make_input_profile import synthetic_input_prefix,produce
BASE=Path(__file__).resolve().parents[1]
INPUT=json.loads((BASE/'evidence/input_profile.json').read_text())

class InputObserver(ProbeObserver):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs)
  self.targets.update({p['address']:p['name'] for p in INPUT['prefixes']})
  self.counts={};self.units=self.scenario.get('clock_start',0);self.key_index=0
  self.file=bytearray(b'PREEXISTING\n');self.position=0;self.closed=0;self.opened=0;self.rows=[]
  self.pixels=bytearray(320*240*3);self.preview=None;self.decoded_at_entry=False
 def mutate(self):
  super().mutate()
  if 'input_prefix' in self.scenario:
   address=INPUT['prefixes'][self.scenario['input_prefix']]['address'];b=bytes(self.u.mem_read(address,1));self.u.mem_write(address,bytes([b[0]^1]))
 def code(self,u,address,size,user):
  if address==ENTRY:
   _,plain,_=pack(self.image.data,'SPMP INPUT')
   if bytes(u.mem_read(ENTRY,len(plain)))!=plain:raise AssertionError('RAP/host payload mismatch before entry')
   self.decoded_at_entry=True
   if hasattr(self,'firmware_vm'):
    for p in INPUT['prefixes']:self.firmware_vm.stubs.pop(p['address'],None)
  super().code(u,address,size,user)
 def callback(self,name):
  args=[self.u.reg_read(r) for r in self.regs[:4]];self.counts[name]=self.counts.get(name,0)+1;result=0
  if self.u.reg_read(self.spreg)%8:raise AssertionError('API call alignment')
  fault=self.scenario.get('fault','')
  if name=='time':result=self.units&0xffffffff
  elif name=='delay':
   if args[1] or args[0] not in (2,25):raise AssertionError('Delay arguments')
   if fault!='stalled_clock':self.units=(self.units+self.scenario.get('time_step',4000))&0xffffffff
  elif name=='open':
   path=bytes(self.u.mem_read(args[0],64)).split(b'\0')[0]
   if path!=b'/fat20a1/hda1/SPMPKEYS.CSV' or args[1:3]!=[10,0o600]:raise AssertionError('Unapproved file path/flags')
   if fault=='no_sd':result=0xffffffff
   else:self.opened+=1;result=0
  elif name=='seek':
   if args[:3]!=[0,0,2]:raise AssertionError('Seek must append at EOF')
   self.position=len(self.file);result=self.position
   if fault=='seek':result=0xffffffff
   if fault=='file_limit':result=1048576
  elif name=='write':
   if args[0]!=0 or args[2]>256:raise AssertionError('Write count/fd')
   if fault=='write':result=0xffffffff
   elif fault=='zero_write':result=0
   else:
    n=min(args[2],32) if fault=='short_write' else args[2]
    data=bytes(self.u.mem_read(args[1],n));self.file[self.position:self.position+n]=data;self.position+=n;result=n
  elif name=='close':self.closed+=1;result=0xffffffff if fault=='close' else 0
  elif name=='origin':
   if list(struct.unpack('<hh',self.u.mem_read(args[0],4)))!=[0,0]:raise AssertionError('Origin')
  elif name=='double_buffer':
   if args[0]!=1:raise AssertionError('Double buffer')
  elif name=='dimensions':self.output(args[0],struct.pack('<H',320));self.output(args[1],struct.pack('<H',240))
  elif name=='get_color':self.output(args[0],bytes(self.color))
  elif name=='set_color':self.color=list(self.u.mem_read(args[0],3))
  elif name=='fill':
   x,y,w,h=struct.unpack('<hhHH',self.u.mem_read(args[0],8))
   if min(x,y)<0 or x+w>320 or y+h>240:raise AssertionError('Rectangle bounds')
   if self.scenario.get('render'):
    row=bytes(self.color)*w
    for py in range(y,y+h):self.pixels[(py*320+x)*3:(py*320+x+w)*3]=row
  elif name=='paint':
   if self.scenario.get('render') and self.counts[name]==5:self.preview=bytes(self.pixels)
  elif name=='keys':
   sequence=self.scenario.get('keys',[0,1,0,2,0,0x80000,0,8,0])
   if fault=='never_release':key=1
   elif fault=='stalled_clock':key=self.key_index&1
   else:key=sequence[self.key_index%len(sequence)]
   self.key_index+=1;self.output(args[0]+4,struct.pack('<I',key));result=1
  for r in self.regs[1:4]+[self.regs[12]]:self.u.reg_write(r,0xa5a5a5a5)
  self.u.reg_write(self.regs[0],result);self.u.reg_write(self.pc,self.u.reg_read(self.lr))
 def result(self,value):
  failure=self.scenario.get('status',0)
  if value!=failure:raise AssertionError('Unexpected input result '+repr((value,failure)))
  if self.get32(self.image.bss_end)!=0xdecafbad:raise AssertionError('BSS guard')
  if value>=0x900:
   if self.main_seen or self.counts or bytes(self.u.mem_read(self.image.bss_start,self.image.bss_end-self.image.bss_start))!=b'\xa5'*(self.image.bss_end-self.image.bss_start):raise AssertionError('Preflight changed BSS/called services')
   return {'status':value,'bss_untouched':True,'api_calls':0}
  state=self.image.address('spmp_input_state');words=struct.unpack('<29I',self.u.mem_read(state,116))
  polls,events,mask,pressed,released,seen,last,elapsed,start=words[:9];log_status,log_bytes,records,prompt=words[9:13]
  expected={'no_sd':1,'seek':2,'write':3,'zero_write':3,'close':4,'file_limit':5,'stalled_clock':5}.get(self.scenario.get('fault',''),0)
  if log_status!=expected:raise AssertionError('Unexpected journal status '+repr((log_status,expected)))
  if self.opened!=self.closed:raise AssertionError('File handle left open')
  if not self.file.startswith(b'PREEXISTING\n'):raise AssertionError('Prior content overwritten')
  if self.color!=[8,128,248]:raise AssertionError('FG not restored')
  if log_status==0:
   decoded=self.file.decode('ascii')
   if value==0 and ('KEY,' not in decoded or 'SAMPLE,' not in decoded or 'END,' not in decoded):raise AssertionError('Missing input journal records')
   if value==2 and 'END_ERROR,' not in decoded:raise AssertionError('Missing input error record')
   if log_bytes!=len(self.file)-12:raise AssertionError('Journal bytes mismatch')
  if self.scenario.get('name')=='learned_map' and self.get32(state+156)!=1:raise AssertionError('Learned MAP menu not selected')
  if self.scenario.get('name')=='learned_exit' and (elapsed>=60000 or self.get32(state+152)!=2):raise AssertionError('Learned EXIT did not return early')
  if self.scenario.get('fault')=='stalled_clock' and polls!=3000:raise AssertionError('Poll limit not enforced')
  return {'status':value,'polls':polls,'events':events,'seen_mask':hex(seen),'log_status':log_status,
   'logged_bytes':log_bytes,'log_records':records,'file_sha256':hashlib.sha256(self.file).hexdigest(),
   'close_attempted_for_every_open':True,'previous_content_preserved':True,'fg_restored':True,'api_counts':self.counts}

def synthetic(path,preview=None,log_path=None):
 from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
 from unicorn import arm_const as ar
 image=StartupELF(path.read_bytes());container,plain,manifest=pack(image.data,'SPMP INPUT');rows=[]
 learned=[0]+[v for mask in [1,2,4,8,0x10000,0x20000,0x40,0x80,0x100,0x200,0x400,0x800,0x80000] for v in (mask,0)]
 # A held DOWN must produce one menu step, followed by learned A selecting MAP.
 learned_slow=[0]+[v for mask in [1,2,4,8,0x10000,0x20000,0x40,0x80,0x100,0x200,0x400,0x800,0x80000] for v in (mask,0,0,0)]
 cases=[{'name':'guided','render':True},{'name':'learned_map','keys':learned_slow+[2,2,2,0,0x10000,0],'time_step':1000},
  {'name':'learned_exit','keys':learned+[2,0,2,0x10000],'time_step':2000},{'name':'short_write','fault':'short_write'},
  {'name':'clock_wrap','clock_start':0xfffffff0},{'name':'held_launcher','fault':'never_release','status':2},*({'name':f,'fault':f} for f in ['no_sd','seek','write','zero_write','close','file_limit','stalled_clock']),
  *({'name':'extra_prefix_'+str(i),'input_prefix':i,'status':0x907} for i in range(6))]
 for case in cases:
  u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
  u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000);u.mem_write(ENTRY,container);u.mem_write(ENTRY,plain)
  u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
  for p in PROFILE['prefixes']:u.mem_write(p['target'],synthetic_prefix(p['slot']))
  for p in INPUT['prefixes']:u.mem_write(p['address'],synthetic_input_prefix(p['address']))
  for address,value in GLOBALS.items():u.mem_write(address,struct.pack('<I',value))
  observer=InputObserver(u,image,DEFAULT_SP,case);saved=[0x45670000+4*i for i in range(8)]
  for r,value in zip(observer.regs[4:12],saved):u.reg_write(r,value)
  u.reg_write(ar.UC_ARM_REG_SP,DEFAULT_SP);u.reg_write(ar.UC_ARM_REG_LR,0x1200000);u.reg_write(observer.regs[0],PROFILE['api_base'])
  u.emu_start(ENTRY,0x1200000,count=50000000,timeout=30000000)
  if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=DEFAULT_SP or [u.reg_read(r) for r in observer.regs[4:12]]!=saved:raise AssertionError('Return/ABI/VM bound')
  rows.append({'case':case['name'],**observer.result(u.reg_read(observer.regs[0]))})
  if case['name']=='guided':
   if preview:preview.write_bytes(b'P6\n320 240\n255\n'+(observer.preview or bytes(observer.pixels)))
   if log_path:log_path.write_bytes(bytes(observer.file))
 return {'execution':'ARM926 compiled input application, synthetic profile/clock/SD/graphics',
  'case_count':len(rows),'package':manifest,'cases':rows,'limits':['No console/SD accessed. Clock accelerated by 4000 units per delay call.',
  'Prompt names are requested labels, not automatic physical-button identification.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',required=True,type=Path);p.add_argument('--rap',type=Path)
 p.add_argument('--output',required=True,type=Path);p.add_argument('--preview',type=Path);p.add_argument('--log',type=Path);a=p.parse_args()
 if a.rap:
  if produce(a.rap.read_bytes())!=INPUT:raise ValueError('Input profile differs from original RAP')
  r=private_rap(a.elf,a.rap,observer_class=InputObserver,title='SPMP INPUT',cases=[{'name':'guided'},{'name':'no_sd','fault':'no_sd'},{'name':'short_write','fault':'short_write'}])
  r['execution']='Actual RAP ARM926 BIN loader through source-built interactive input app and normal return'
  r['mocked'].append('all six input app services: time/delay/absolute open/write/seek/close')
  r['limits'].append('App SD and clock are synthetic; no physical card is accessed.')
 else:r=synthetic(a.elf,a.preview,a.log)
 a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'compiled input scenarios')
