#!/usr/bin/env python3
"""Compiled raw RAM observer checks. All physical input/SD/clock/graphics simulated."""
import argparse,hashlib,json,struct
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
from emulate_native_package import ProbeObserver,PROFILE,GLOBALS,DEFAULT_SP,private_rap
from emulate_input import InputObserver,INPUT
from make_binding_profile import synthetic_prefix as base_prefix
from make_input_profile import synthetic_input_prefix
from make_raw_profile import synthetic_prefix,produce,LITERALS
from raw_log_decode import decode
BASE=Path(__file__).resolve().parents[1]
RAW=json.loads((BASE/'evidence/raw_input_profile.json').read_text())
TITLE='SPMP KEYS RAW'
class RawObserver(InputObserver):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs)
  from unicorn import UC_HOOK_MEM_READ
  self.reads={0x556144:0,0x556148:0};self.pause_checked=False
  def read_hook(u,access,address,size,value,user):
   if address in self.reads and ENTRY<=u.reg_read(self.pc)<ENTRY+128*1024:self.reads[address]+=1
  self.u.hook_add(UC_HOOK_MEM_READ,read_hook)
 def mutate(self):
  super().mutate();self.put32(0x556144,0);self.put32(0x556148,0)
  if 'raw_prefix' in self.scenario:
   a=RAW['prefixes'][self.scenario['raw_prefix']]['address'];b=bytes(self.u.mem_read(a,1));self.u.mem_write(a,bytes([b[0]^1]))
  if 'raw_literal' in self.scenario:self.put32(self.scenario['raw_literal'],0x556140)
 def code(self,u,address,size,user):
  if address==ENTRY:
   _,plain,_=pack(self.image.data,TITLE)
   if bytes(u.mem_read(ENTRY,len(plain)))!=plain:raise AssertionError('RAW payload mismatch before entry')
   if hasattr(self,'firmware_vm'):
    for p in INPUT['prefixes']:self.firmware_vm.stubs.pop(p['address'],None)
  ProbeObserver.code(self,u,address,size,user)
 def finish_callback(self,value):
  if self.u.reg_read(self.spreg)%8:raise AssertionError('API alignment')
  for r in self.regs[1:4]+[self.regs[12]]:self.u.reg_write(r,0xa5a5a5a5)
  self.u.reg_write(self.regs[0],value);self.u.reg_write(self.pc,self.u.reg_read(self.lr))
 def callback(self,name):
  args=[self.u.reg_read(r) for r in self.regs[:4]];fault=self.scenario.get('fault','')
  if name=='open':
   self.counts[name]=self.counts.get(name,0)+1
   if bytes(self.u.mem_read(args[0],64)).split(b'\0')[0]!=b'/fat20a0/hda0/SPMPRAW.CSV' or args[1:3]!=[10,0o600]:raise AssertionError('RAW path/flags')
   if fault=='no_sd':value=0xffffffff
   else:self.opened+=1;value=0
   self.finish_callback(value);return
  if name=='keys':
   self.counts[name]=self.counts.get(name,0)+1
   key=self.get32(0x556144) if self.scenario.get('cooked_pair') else 0
   if self.scenario.get('pause_gap') and self.key_index==4:
    records=decode(self.file.decode('ascii'))['sessions'][0]['events']
    if records[-1]['event']!='PRE' or records[-1]['cooked'] is not None:raise AssertionError('PRE not persisted before pausing API')
    self.pause_checked=True;self.units=(self.units+20000)&0xffffffff
   self.output(args[0]+4,struct.pack('<I',key));self.key_index+=1
   self.put32(0x556148,0) # Simulated collector acknowledges the event latch.
   if fault=='never_release':self.put32(0x556144,0x40)
   self.finish_callback(0 if fault=='keys_error' and self.key_index>1 else 1);return
  super().callback(name)
  if name=='delay' and args[0]==2:
   on=self.key_index&1;mask=1 if self.scenario.get('cooked_pair') else 0x340 if self.scenario.get('chord') else 0x40
   self.put32(0x556144,mask if on else 0);self.put32(0x556148,0x400 if on else 0)
 def result(self,value):
  expected=self.scenario.get('status',0)
  if value!=expected:raise AssertionError('RAW result '+repr((value,expected)))
  if self.get32(self.image.bss_end)!=0xdecafbad:raise AssertionError('RAW BSS guard')
  if value>=0x900:
   if self.main_seen or self.counts or any(self.reads.values()) or bytes(self.u.mem_read(self.image.bss_start,self.image.bss_end-self.image.bss_start))!=b'\xa5'*(self.image.bss_end-self.image.bss_start):raise AssertionError('RAW rejected preflight touched state/API')
   return {'status':value,'bss_untouched':True,'api_calls':0,'input_ram_reads':0}
  state=self.image.address('spmp_raw_state');logstatus=self.get32(state+36);polls=self.get32(state);samples=self.get32(state+188)
  expected_log={'no_sd':1,'seek':2,'write':3,'zero_write':3,'close':4,'file_limit':5,'stalled_clock':5}.get(self.scenario.get('fault',''),0)
  if logstatus!=expected_log:raise AssertionError('RAW journal status '+repr((logstatus,expected_log)))
  if self.get32(state+168)!={'no_sd':2,'seek':5,'write':28,'close':9}.get(self.scenario.get('fault',''),0):raise AssertionError('RAW errno')
  if self.opened!=self.closed or not self.file.startswith(b'PREEXISTING\n') or self.color!=[8,128,248]:raise AssertionError('RAW append/close/FG')
  if samples!=self.counts.get('keys',0) or not all(n>=samples*2 for n in self.reads.values()):raise AssertionError('Missing direct RAM snapshots or extra GetKeys')
  decoded=decode(self.file.decode('ascii'));observed={}
  if logstatus==0:
   session=decoded['sessions'][0];events=session['events']
   if decoded['ignored_or_partial_lines']!=1 or not session['ended']:raise AssertionError('RAW CSV incomplete') # PREEXISTING is intentionally foreign.
   if session['end_event']!=('END_ERROR' if value else 'END'):raise AssertionError('RAW END kind')
   if len(self.file)-12!=self.get32(state+40):raise AssertionError('RAW byte count')
   if value==0 and not self.scenario.get('cooked_pair') and not any(e['state_before'] not in (None,'0x00000000') and e['cooked']=='0x00000000' for e in events if e['event']=='SAMPLE'):raise AssertionError('Hidden raw state not captured')
   observed=session['observed_masks_by_prompt']
   if self.scenario.get('all_phases') and not {'HOLD_SHORT','HOLD_LOCK','HOLD_UNLOCK','POWER_SHORT','SELECT_VOL_PLUS','SELECT_VOL_MINUS','FREE'}<={e['prompt'] for e in events}:raise AssertionError('Missing POWER/HOLD/chord phases')
   if self.scenario.get('pause_gap') and (not self.pause_checked or not any(e['event']=='GAP' and e['call_units']>=20000 for e in events)):raise AssertionError('Pause gap not logged')
  if fault:=self.scenario.get('fault'):
   if fault=='stalled_clock' and polls!=4000:raise AssertionError('RAW poll cap')
  return {'status':value,'polls':polls,'getkeys_calls':samples,'log_status':logstatus,'log_errno':self.get32(state+168),
   'logged_bytes':self.get32(state+40),'input_ram_reads':{hex(a):v for a,v in self.reads.items()},'previous_content_preserved':True,
   'close_attempted_for_every_open':True,'fg_restored':True,'pause_pre_record_verified':self.pause_checked,'file_sha256':hashlib.sha256(self.file).hexdigest()}

def synthetic(path,preview=None,log=None):
 from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
 from unicorn import arm_const as ar
 image=StartupELF(path.read_bytes());container,plain,manifest=pack(image.data,TITLE);rows=[]
 cases=[{'name':'hidden_guided','time_step':1000,'all_phases':True,'render':True},{'name':'chord_raw','chord':True},
  {'name':'cooked_pair','cooked_pair':True},{'name':'short_write','fault':'short_write'},{'name':'clock_wrap','clock_start':0xfffffff0},
  {'name':'held_raw_launcher','fault':'never_release','status':2},{'name':'pause_gap','pause_gap':True},
  {'name':'keys_error','fault':'keys_error','status':2},*({'name':f,'fault':f} for f in ['no_sd','seek','write','zero_write','close','file_limit','stalled_clock']),
  *({'name':'input_prefix_'+str(i),'input_prefix':i,'status':0x907} for i in range(7)),
  *({'name':'raw_prefix_'+str(i),'raw_prefix':i,'status':0x908} for i in range(3)),
  *({'name':'raw_literal_'+hex(a),'raw_literal':a,'status':0x908} for a in LITERALS)]
 for case in cases:
  u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
  u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000);u.mem_write(ENTRY,plain)
  u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
  for p in PROFILE['prefixes']:u.mem_write(p['target'],base_prefix(p['slot']))
  for p in INPUT['prefixes']:u.mem_write(p['address'],synthetic_input_prefix(p['address']))
  for p in RAW['prefixes']:u.mem_write(p['address'],synthetic_prefix(p['address'],p['bytes']))
  for a,v in LITERALS.items():u.mem_write(a,struct.pack('<I',v))
  for a,v in GLOBALS.items():u.mem_write(a,struct.pack('<I',v))
  observer=RawObserver(u,image,DEFAULT_SP,case);saved=[0x45670000+4*i for i in range(8)]
  for reg,v in zip(observer.regs[4:12],saved):u.reg_write(reg,v)
  u.reg_write(ar.UC_ARM_REG_SP,DEFAULT_SP);u.reg_write(ar.UC_ARM_REG_LR,0x1200000);u.reg_write(observer.regs[0],PROFILE['api_base'])
  u.emu_start(ENTRY,0x1200000,count=100000000,timeout=60000000)
  if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=DEFAULT_SP or [u.reg_read(reg) for reg in observer.regs[4:12]]!=saved:raise AssertionError('RAW ABI/return/VM bound')
  rows.append({'case':case['name'],**observer.result(u.reg_read(observer.regs[0]))})
  if case['name']=='hidden_guided':
   if preview:preview.write_bytes(b'P6\n320 240\n255\n'+(observer.preview or bytes(observer.pixels)))
   if log:log.write_bytes(bytes(observer.file))
 return {'execution':'Compiled ARM926 raw RAM observer, synthetic clock/input/SD/graphics','case_count':len(rows),'package':manifest,'cases':rows,
  'limits':['No console/GPIO accessed. Input RAM and every OS/graphics/filesystem callback are simulated.','RAM snapshots execute compiled code; app stores limited to its BSS/inherited stack.']}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,required=True);p.add_argument('--rap',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--preview',type=Path);p.add_argument('--log',type=Path);a=p.parse_args()
 if a.rap:
  if produce(a.rap.read_bytes())!=RAW:raise ValueError('RAW profile mismatch')
  r=private_rap(a.elf,a.rap,observer_class=RawObserver,title=TITLE,cases=[{'name':'hidden'},{'name':'no_sd','fault':'no_sd'},{'name':'short_write','fault':'short_write'}],max_instructions=100000000,timeout_us=60000000)
  r['execution']='Original RAP ARM926 loader -> own raw RAM observer -> normal return'
  r['mocked'].extend(['all seven app time/SD/errno services','upstream input RAM contents and cooked GetKeys results'])
  r['limits'].append('POWER/HOLD/volume physical behavior is not tested. Direct RAM snapshot code runs against simulated inputs.')
 else:r=synthetic(a.elf,a.preview,a.log)
 a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',r['case_count'],'raw input scenarios')
