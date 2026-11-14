#!/usr/bin/env python3
"""Execute the compiled MMI-hosted shell; inputs/SD/display are VM fixtures only."""
import argparse,csv,hashlib,json,struct
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
from emulate_native_package import ProbeObserver,PROFILE,GLOBALS,DEFAULT_SP,private_rap
from emulate_input import InputObserver,INPUT
from make_binding_profile import synthetic_prefix as base_prefix
from make_input_profile import synthetic_input_prefix
from make_raw_profile import synthetic_prefix as raw_prefix,LITERALS,produce as raw_profile
from make_shell_profile import synthetic_prefix as shell_prefix,produce as shell_profile
BASE=Path(__file__).resolve().parents[1]
RAW=json.loads((BASE/'evidence/raw_input_profile.json').read_text())
SHELL=json.loads((BASE/'evidence/mmi_shell_profile.json').read_text())
TITLE='SPMP OS MINI'
STATE,EVENT,TTL,GATE,GUI=0x556144,0x556148,0x55614c,0x556aa0,0x561078
NAV=[{}, {'state':0x20},{},{'state':0x80},{},{'state':4},{},{'state':0x20},{},{'state':0x80},{},{'state':4},{},{'state':0x20}]
POWER=[{}, {'event':0x1000}, {},{},{},{},{'state':0x20},{},{'event':0x1000},{},{},{},{},{'state':4},{},{'state':0x20}]

class ShellObserver(InputObserver):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs)
  from unicorn import UC_HOOK_MEM_READ
  self.reads={STATE:0,EVENT:0,GATE:0,GUI:0};self.collects=0;self.step=0;self.views=set();self.power_rows=0
  if not self.original:self.targets[0x2d2e6c]='collect'
  def read_hook(u,access,address,size,value,user):
   if address in self.reads and ENTRY<=u.reg_read(self.pc)<ENTRY+128*1024:self.reads[address]+=1
  self.u.hook_add(UC_HOOK_MEM_READ,read_hook)
 def fixture(self,item):
  for name,address in [('state',STATE),('gate',GATE),('gui',GUI)]:self.put32(address,item.get(name,0))
  if 'event' in item:self.put32(EVENT,item['event']);self.put32(TTL,0)
 def mutate(self):
  super().mutate()
  for address in (STATE,EVENT,TTL,GATE,GUI):self.put32(address,0)
  self.fixture(self.scenario.get('initial',{}))
  if 'raw_prefix' in self.scenario:
   a=RAW['prefixes'][self.scenario['raw_prefix']]['address'];self.u.mem_write(a,bytes([self.u.mem_read(a,1)[0]^1]))
  if 'raw_literal' in self.scenario:self.put32(self.scenario['raw_literal'],0x556140)
  if 'shell_prefix' in self.scenario:
   a=SHELL['prefixes'][self.scenario['shell_prefix']]['address'];self.u.mem_write(a,bytes([self.u.mem_read(a,1)[0]^1]))
  if 'shell_literal' in self.scenario:self.put32(self.scenario['shell_literal'],0x556140)
 def code(self,u,address,size,user):
  if address==ENTRY:
   _,plain,_=pack(self.image.data,TITLE)
   if bytes(u.mem_read(ENTRY,len(plain)))!=plain:raise AssertionError('Shell decoded payload mismatch')
   if hasattr(self,'firmware_vm'):
    for p in INPUT['prefixes']:self.firmware_vm.stubs.pop(p['address'],None)
  if address==0x2d2e6c and self.entered:self.collects+=1
  ProbeObserver.code(self,u,address,size,user)
 def finish(self,value):
  if self.u.reg_read(self.spreg)%8:raise AssertionError('Unaligned shell callback')
  for r in self.regs[1:4]+[self.regs[12]]:self.u.reg_write(r,0xa5a5a5a5)
  self.u.reg_write(self.regs[0],value);self.u.reg_write(self.pc,self.u.reg_read(self.lr))
 def callback(self,name):
  args=[self.u.reg_read(r) for r in self.regs[:4]]
  if name=='keys':raise AssertionError('Shell must not call cooked GetKeys')
  if name=='collect':
   if self.get32(GATE):self.put32(STATE,0);self.put32(EVENT,0);value=0
   elif self.get32(EVENT):
    value=self.get32(EVENT);n=self.get32(TTL)+1
    self.put32(TTL,n if n<4 else 0)
    if n>=4:self.put32(EVENT,0)
   else:
    table=[1,8,2,4,0x40,0x10,0x80,0x20,0x200,0x100,0x100,0x200,0x800,0x400,0,1]
    value=0
    for i,v in enumerate(table):
     if self.get32(STATE)&(1<<i):value|=v
    value&=~0x800
   self.finish(value);return
  if name=='open':
   self.counts[name]=self.counts.get(name,0)+1
   if bytes(self.u.mem_read(args[0],64)).split(b'\0')[0]!=b'/fat20a0/hda0/SPMPOS.CSV' or args[1:3]!=[10,0o600]:raise AssertionError('Shell SD path/flags')
   if self.scenario.get('fault')=='no_sd':value=0xffffffff
   else:self.opened+=1;value=0
   self.finish(value);return
  super().callback(name)
  if name=='delay':
   self.views.add(self.get32(self.image.address('spmp_shell_model')))
   if args[0]==2:
    script=self.scenario.get('script',NAV)
    self.fixture(script[self.step] if self.step<len(script) else {})
    self.step+=1
 def result(self,value):
  expected=self.scenario.get('status',0)
  if value!=expected:raise AssertionError('Shell result '+repr((value,expected,self.scenario['name'])))
  if self.get32(self.image.bss_end)!=0xdecafbad:raise AssertionError('Shell BSS guard')
  if value>=0x900:
   if self.main_seen or self.counts or self.collects or any(self.reads.values()) or bytes(self.u.mem_read(self.image.bss_start,self.image.bss_end-self.image.bss_start))!=b'\xa5'*(self.image.bss_end-self.image.bss_start):raise AssertionError('Rejected shell touched BSS/API/input')
   return {'status':value,'bss_untouched':True,'api_calls':0,'input_ram_reads':0}
  at=self.image.address('spmp_shell_journal');polls=self.get32(at);logstatus=self.get32(at+36);reason=self.get32(self.image.address('spmp_shell_exit_reason'))
  fault=self.scenario.get('fault','')
  wanted={'no_sd':1,'seek':2,'write':3,'zero_write':3,'close':4,'file_limit':5}.get(fault,0)
  if logstatus!=wanted:raise AssertionError('Shell log status '+repr((fault,logstatus,wanted)))
  if self.get32(at+168)!={'no_sd':2,'seek':5,'write':28,'close':9}.get(fault,0):raise AssertionError('Shell errno')
  if self.opened!=self.closed or not self.file.startswith(b'PREEXISTING\n') or self.color!=[8,128,248]:raise AssertionError('Shell append/close/FG')
  if self.collects!=polls+(reason==1) or not all(n>=self.collects*2 for n in self.reads.values()):raise AssertionError('Snapshots/collector count '+repr((polls,self.collects,self.reads,reason)))
  self.views.add(self.get32(self.image.address('spmp_shell_model')))
  if not set(self.scenario.get('views',[]))<=self.views:raise AssertionError('Missing shell views '+repr(self.views))
  if reason!=self.scenario.get('reason',1):raise AssertionError('Shell exit reason '+repr((reason,self.scenario)))
  if fault=='stalled_clock' and polls!=7500:raise AssertionError('Shell fallback poll cap')
  if logstatus==0:
   lines=[x for x in self.file.decode('ascii').splitlines() if x and not x.startswith('#') and not x.startswith(('PREEXISTING','event,'))]
   records=list(csv.reader(lines))
   if any(len(row)!=10 for row in records) or records[0][0]!='START' or records[-1][0]!='END':raise AssertionError('Shell CSV schema/end')
   for row in records:
    for cell in row[1:]:int(cell,16)
   self.power_rows=sum(row[0]=='POWER' for row in records)
   if self.power_rows!=self.scenario.get('power_rows',0):raise AssertionError('POWER pulses '+repr(self.power_rows))
   if self.get32(at+40)!=len(self.file)-12:raise AssertionError('Shell logged bytes')
  return {'status':value,'polls':polls,'collector_calls':self.collects,'cooked_getkeys_calls':0,'exit_reason':reason,'views':sorted(self.views),'power_rows':self.power_rows,
   'log_status':logstatus,'log_errno':self.get32(at+168),'logged_bytes':self.get32(at+40),'input_ram_reads':{hex(k):v for k,v in self.reads.items()},
   'fg_restored':True,'close_attempted_for_every_open':True,'previous_content_preserved':True,'file_sha256':hashlib.sha256(self.file).hexdigest()}

def cases():
 return [
  {'name':'navigate','render':True,'views':[0,1,2,4]},
  {'name':'power_continue_exit','script':POWER,'power_rows':2,'views':[0,3,4]},
  {'name':'held_launcher','initial':{'state':0x20},'script':[{'state':0x20}]*3+[{}, {'state':0x80}],'views':[0,4]},
  *({'name':key+'_release_before_action','initial':{key:1,'state':0x20},'script':[{key:1,'state':0x20},{'state':0x20},{},{'state':0x80}],'views':[0,4]} for key in ['gate','gui']),
  {'name':'shoulders_volume_distinct','script':[{},*({'state':v} for v in [0x400,0,0x100,0,0x800,0,0x200,0,0x2800,0,0x2200,0,0x4000,0,0x80])],'views':[0,4]},
  {'name':'short_write','fault':'short_write'}, {'name':'clock_wrap','clock_start':0xfffffff0},
  *({'name':f,'fault':f} for f in ['no_sd','seek','write','zero_write','close','file_limit']),
  {'name':'time_cap','script':[],'reason':2}, {'name':'stalled_clock','fault':'stalled_clock','script':[],'reason':3},
  *({'name':'shell_prefix_'+str(i),'shell_prefix':i,'status':0x909} for i in range(2)),
  *({'name':'shell_literal_'+hex(p['literal']),'shell_literal':p['literal'],'status':0x909} for p in SHELL['prefixes']),
  {'name':'raw_prefix','raw_prefix':0,'status':0x908},{'name':'raw_literal','raw_literal':0x2d2e4c,'status':0x908},
  {'name':'input_prefix','input_prefix':0,'status':0x907}]

def synthetic(path,preview=None,log=None):
 from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
 from unicorn import arm_const as ar
 image=StartupELF(path.read_bytes());container,plain,manifest=pack(image.data,TITLE);rows=[]
 for case in cases():
  u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
  u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000);u.mem_write(ENTRY,plain)
  u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
  for p in PROFILE['prefixes']:u.mem_write(p['target'],base_prefix(p['slot']))
  for p in INPUT['prefixes']:u.mem_write(p['address'],synthetic_input_prefix(p['address']))
  for p in RAW['prefixes']:u.mem_write(p['address'],raw_prefix(p['address'],p['bytes']))
  for a,v in LITERALS.items():u.mem_write(a,struct.pack('<I',v))
  for p in SHELL['prefixes']:u.mem_write(p['address'],shell_prefix(p['address']))
  for a,v in GLOBALS.items():u.mem_write(a,struct.pack('<I',v))
  observer=ShellObserver(u,image,DEFAULT_SP,case);saved=[0x45670000+4*i for i in range(8)]
  for reg,v in zip(observer.regs[4:12],saved):u.reg_write(reg,v)
  u.reg_write(ar.UC_ARM_REG_SP,DEFAULT_SP);u.reg_write(ar.UC_ARM_REG_LR,0x1200000);u.reg_write(observer.regs[0],PROFILE['api_base'])
  u.emu_start(ENTRY,0x1200000,count=100000000,timeout=60000000)
  if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=DEFAULT_SP or [u.reg_read(reg) for reg in observer.regs[4:12]]!=saved:raise AssertionError('Shell ABI/return/VM bound: '+case['name'])
  rows.append({'case':case['name'],**observer.result(u.reg_read(observer.regs[0]))})
  if case['name']=='navigate':
   if preview:preview.write_bytes(b'P6\n320 240\n255\n'+(observer.preview or bytes(observer.pixels)))
   if log:log.write_bytes(bytes(observer.file))
 return {'execution':'Compiled ARM926 MMI shell, synthetic profile/input/SD/graphics','case_count':len(rows),'package':manifest,'cases':rows,
  'limits':['No console/card/GPIO accessed. Collector is simulated in public tests.','Own stores limited to BSS/inherited stack; input snapshots are reads only.']}

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--elf',type=Path,required=True);p.add_argument('--rap',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--preview',type=Path);p.add_argument('--log',type=Path);a=p.parse_args()
 if a.rap:
  blob=a.rap.read_bytes()
  if shell_profile(blob)!=SHELL or raw_profile(blob)!=RAW:raise ValueError('Exact-image profile mismatch')
  r=private_rap(a.elf,a.rap,observer_class=ShellObserver,title=TITLE,cases=[cases()[0],cases()[1],{'name':'no_sd','fault':'no_sd'}],max_instructions=100000000,timeout_us=60000000)
  r['execution']='Original RAP loader -> own shell with original collector -> normal return'
  r['original_code_executed'].extend(['kind6 input collector 0x2d2e6c','input gate getter 0x2d8b78'])
  r['mocked'].extend(['time/SD/errno services','upstream input state/event/gate/GUI RAM fixtures'])
  r['limits'].append('No physical POWER/menu/lock timing tested; own menu replaces cooked GetKeys behavior in this app.')
 else:r=synthetic(a.elf,a.preview,a.log)
 a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'MMI shell scenarios')
