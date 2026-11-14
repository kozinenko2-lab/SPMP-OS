#!/usr/bin/env python3
"""Source-built NGame/binder/probe execution, synthetic callbacks and RAM only.
Public: synthetic profile and own code-prefix patterns. Optional private RAP:
actual native BIN/file/header/cipher/return code; all OS/device backends mocked.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_pack import pack
from make_binding_profile import synthetic_prefix
from native_lifecycle_models import fresh_pool_layout
BASE=Path(__file__).resolve().parents[1]
PROFILE=json.loads((BASE/'evidence/native_binding_profile.json').read_text())
LAYOUT=fresh_pool_layout([0xb7840,0xb7840,0x25800,0x25800,0x100010])
OWNER=LAYOUT['allocations'][4]['address']
DEFAULT_SP=OWNER+0x100000
GLOBALS={0x693c5c:LAYOUT['main'],0x5641d8:LAYOUT['capacity'],0x5641dc:6,
    0x6769b0:OWNER,0x5641e0:LAYOUT['frames'][0],0x5641e4:LAYOUT['frames'][1]}

class ProbeObserver:
    def __init__(self,u,image,sp,scenario,original=False):
        from unicorn import UC_HOOK_CODE,UC_HOOK_MEM_WRITE
        from unicorn import arm_const as ar
        self.u=u;self.image=image;self.sp=sp;self.scenario=scenario;self.original=original
        self.regs=[getattr(ar,'UC_ARM_REG_R'+str(i)) for i in range(13)]
        self.pc=ar.UC_ARM_REG_PC;self.lr=ar.UC_ARM_REG_LR;self.spreg=ar.UC_ARM_REG_SP
        self.events=[];self.color=[8,128,248];self.entered=False;self.main_seen=False;self.return_value=None
        self.targets={p['target']:p['name'] for p in PROFILE['prefixes']}
        u.hook_add(UC_HOOK_CODE,self.code);u.hook_add(UC_HOOK_MEM_WRITE,self.write)
    def get32(self,address):return struct.unpack('<I',self.u.mem_read(address,4))[0]
    def put32(self,address,value):self.u.mem_write(address,struct.pack('<I',value))
    def mutate(self):
        case=self.scenario
        if 'slot' in case:self.put32(PROFILE['api_base']+case['slot']*4,PROFILE['slots'][case['slot']]^4)
        if 'prefix' in case:
            address=PROFILE['prefixes'][case['prefix']]['target'];b=bytes(self.u.mem_read(address,1))
            self.u.mem_write(address,bytes([b[0]^1]))
        for address,value in case.get('globals',{}).items():self.put32(address,value)
    def output(self,address,data):
        if not (self.image.bss_start<=address and address+len(data)<=self.image.bss_end or
                self.sp-0x2000<=address and address+len(data)<=self.sp):raise AssertionError('Callback output outside app/stack')
        self.u.mem_write(address,data)
    def callback(self,name):
        args=[self.u.reg_read(r) for r in self.regs[:4]];event={'api':name};status=0
        if self.u.reg_read(self.spreg)%8:raise AssertionError('API call stack not AAPCS aligned')
        if name=='origin':
            event['xy']=list(struct.unpack('<hh',self.u.mem_read(args[0],4)))
            if event['xy']!=[0,0]:raise AssertionError('Origin mismatch')
        elif name=='double_buffer':
            event['enabled']=args[0]
            if args[0]!=1:raise AssertionError('Double buffer mismatch')
        elif name=='dimensions':
            self.output(args[0],struct.pack('<H',320));self.output(args[1],struct.pack('<H',240))
        elif name=='get_color':self.output(args[0],bytes(self.color))
        elif name=='set_color':self.color=list(self.u.mem_read(args[0],3));event['rgb']=self.color
        elif name=='fill':
            event['rect']=list(struct.unpack('<hhHH',self.u.mem_read(args[0],8)))
            if self.scenario.get('fill_error'):status=0x406
        elif name=='keys':
            key=self.scenario.get('keys',0);self.output(args[0]+4,struct.pack('<I',key));event['keys']=key;status=1
        self.events.append(event)
        for reg in self.regs[1:4]+[self.regs[12]]:self.u.reg_write(reg,0xa5a5a5a5)
        self.u.reg_write(self.regs[0],status);self.u.reg_write(self.pc,self.u.reg_read(self.lr))
    def code(self,u,address,size,user):
        if address==ENTRY:
            self.entered=True;self.mutate()
            u.mem_write(self.image.bss_start,b'\xa5'*(self.image.bss_end-self.image.bss_start))
            self.put32(self.image.bss_end,0xdecafbad)
        if address==self.image.address('spmp_app_main'):
            self.main_seen=True
            if bytes(u.mem_read(self.image.bss_start,self.image.bss_end-self.image.bss_start))!=bytes(self.image.bss_end-self.image.bss_start):
                raise AssertionError('BSS not zero before main')
        if address==0x4608ec and self.original:self.return_value=u.reg_read(self.regs[0])
        if address in self.targets and self.entered:self.callback(self.targets[address]);return
        if self.entered and ENTRY<=address<ENTRY+128*1024:
            text=self.image.sections['.text']
            if not text['address']<=address<text['address']+text['bytes']:raise AssertionError('Fetch outside app text')
            sp=u.reg_read(self.spreg)
            if not self.sp-0x2000<=sp<=self.sp:raise AssertionError('SP outside inherited window')
        elif self.entered and not self.original and address!=0x1200000:
            raise AssertionError('Unexpected app instruction outside code/callbacks')
    def write(self,u,access,address,size,value,user):
        pc=u.reg_read(self.pc)
        if ENTRY<=pc<ENTRY+128*1024:
            if not (self.image.bss_start<=address and address+size<=self.image.bss_end or
                    self.sp-0x2000<=address and address+size<=self.sp):raise AssertionError('App store outside BSS/inherited stack')
    def result(self,value):
        expected=self.scenario.get('status',2 if self.scenario.get('fill_error') else 1)
        if value!=expected:raise AssertionError('Unexpected native status '+repr((value,expected,self.scenario)))
        failed=expected>=0x900
        if self.main_seen==failed:raise AssertionError('Preflight main gating mismatch')
        bss=bytes(self.u.mem_read(self.image.bss_start,self.image.bss_end-self.image.bss_start))
        if failed:
            if self.events or bss!=b'\xa5'*len(bss):raise AssertionError('Rejected preflight changed BSS or called API')
        else:
            at=self.image.address('spmp_native_record');record=list(struct.unpack('<4I',self.u.mem_read(at,16)))
            steps=0 if self.scenario.get('keys',0)&0x80000 or self.scenario.get('fill_error') else 1
            if record!=[0,expected,steps,self.scenario.get('keys',0)]:raise AssertionError('Probe record mismatch '+repr(record))
            if self.color!=[8,128,248]:raise AssertionError('Foreground not restored')
        if self.get32(self.image.bss_end)!=0xdecafbad:raise AssertionError('BSS boundary guard changed')
        return {'status':value,'main_called':self.main_seen,'api_calls':len(self.events),
            'bss_untouched_on_rejection':failed,'bss_guard_preserved':True,'events':self.events if not failed else []}

def scenarios():
    rows=[{'name':'one_frame'},{'name':'quit_first','keys':0x80000},{'name':'fill_error','fill_error':True}]
    rows.extend({'name':'slot_'+str(i),'slot':i,'status':0x902} for i in range(85))
    rows.extend({'name':'prefix_'+str(i),'prefix':i,'status':0x903} for i in range(9))
    rows.extend([{'name':'null_api','raw_api':0,'status':0x901},
        {'name':'foreign_api','raw_api':0xfffffffc,'status':0x901}])
    changes=[('kind',{0x5641dc:8},0x904),('null_allocation',{0x693c5c:0},0x904),
        ('high_base',{0x693c5c:0xa00010},0x904),('unaligned_base',{0x693c5c:0x9fff14},0x904),
        ('short_capacity',{0x5641d8:32},0x904),('wrapped_capacity',{0x5641d8:0xffffffff},0x904),
        ('null_stack',{0x6769b0:0},0x905),('overlapping_stack',{0x6769b0:0xa10000},0x905),
        ('wrong_stack',{0x6769b0:OWNER-0x200000},0x905),
        ('null_frame',{0x5641e0:0},0x906),('frame_in_app',{0x5641e0:0xa10000},0x906),
        ('frame_in_stack',{0x5641e0:OWNER},0x906),('frames_overlap',{0x5641e4:GLOBALS[0x5641e0]},0x906)]
    rows.extend({'name':name,'globals':change,'status':status} for name,change,status in changes)
    rows.append({'name':'insufficient_stack_room','initial_sp':OWNER+0x2000,'status':0x905})
    return rows

def synthetic(image_path):
    from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
    from unicorn import arm_const as ar
    image=StartupELF(image_path.read_bytes());container,plain,manifest=pack(image.data);rows=[]
    for case in scenarios():
        for mod8 in (0,4) if case['name'] in ('one_frame','quit_first','fill_error') else (0,):
            u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
            u.mem_map(0x280000,0x480000);u.mem_map(0x9ff000,0x2000000-0x9ff000)
            u.mem_write(ENTRY,container);u.mem_write(ENTRY,plain)
            u.mem_write(PROFILE['api_base'],struct.pack('<85I',*PROFILE['slots']))
            for p in PROFILE['prefixes']:u.mem_write(p['target'],synthetic_prefix(p['slot']))
            for address,value in GLOBALS.items():u.mem_write(address,struct.pack('<I',value))
            sp=case.get('initial_sp',DEFAULT_SP)+mod8
            observer=ProbeObserver(u,image,sp,case)
            saved=[0x45670000+4*i for i in range(8)]
            for r,v in zip(observer.regs[4:12],saved):u.reg_write(r,v)
            u.reg_write(ar.UC_ARM_REG_SP,sp);u.reg_write(ar.UC_ARM_REG_LR,0x1200000)
            u.reg_write(observer.regs[0],case.get('raw_api',PROFILE['api_base']))
            for r in observer.regs[1:4]:u.reg_write(r,0x13572468)
            u.emu_start(ENTRY,0x1200000,count=1000000,timeout=10000000)
            if u.reg_read(ar.UC_ARM_REG_PC)!=0x1200000 or u.reg_read(ar.UC_ARM_REG_SP)!=sp or [u.reg_read(r) for r in observer.regs[4:12]]!=saved:
                raise AssertionError('Synthetic startup return/SP/callee registers mismatch')
            rows.append({'case':case['name'],'sp_mod8':sp%8,**observer.result(u.reg_read(observer.regs[0]))})
    return {'execution':'ARM926 source-built synthetic compatibility profile; no OEM code',
        'case_count':len(rows),'package':manifest,'cases':rows,
        'limits':['Own code-prefix patterns and synthetic allocation globals; not a hardware capture.',
                  'All nine API functions are intercepted; no real graphics/input/RTOS.',
                  'Compatibility CRC32 is not full-image identity or a security boundary.']}

def private_rap(image_path,rap_path,*,observer_class=ProbeObserver,title="SPMP Probe",cases=None,max_instructions=20000000,timeout_us=30000000):
    from unicorn.arm_const import UC_CPU_ARM_926
    from emulate_native_lifecycle import new_vm,pool_info
    from make_binding_profile import extract
    from audit_boot import H
    rap=rap_path.read_bytes()
    if extract(rap)!=PROFILE:raise ValueError('RAP differs from checked binding profile')
    image=StartupELF(image_path.read_bytes());container,plain,manifest=pack(image.data,title);rows=[]
    cases=cases if cases is not None else [{'name':'one_frame'},{'name':'quit_first','keys':0x80000},
           {'name':'slot_corruption','slot':14,'status':0x902},
           {'name':'code_corruption','prefix':0,'status':0x903},
           {'name':'bad_magic','corrupt_magic':True}, {'name':'bad_footer','corrupt_footer':True}]
    for case in cases:
        v=new_vm(rap,pool=True,cpu_model=UC_CPU_ARM_926)
        v.run(0x2e3908);allocations=[]
        for request in [0xb7840,0xb7840,0x25800,0x25800,0x100010,0x25800,0x25800]:
            allocations.append(v.run(0x2e3724,[request]))
        maximum=pool_info(v)['max_request'];main=v.run(0x2e3724,[maximum])
        if main!=LAYOUT['main'] or maximum!=LAYOUT['requested'] or allocations!=[a['address'] for a in LAYOUT['allocations'][:-1]]:
            raise AssertionError('Actual RAP pool does not match supplied memory scenario')
        for address,value in GLOBALS.items():v.put32(address,value)
        v.put32(0x5567e0,1);v.put32(0x5641f8,2);v.put32(0x5641fc,0)
        v.u.mem_write(0x564200,b'\0');v.u.mem_write(0x1001000,b'/vm/SPMP_PROBE_VM.bin\0')
        data=bytearray(container)
        if case.get('corrupt_magic'):data[0]^=1
        if case.get('corrupt_footer'):data[-16]^=1
        def stat(vm,args):vm.put32(args[1]+16,len(data));return 0
        def read(vm,args):
            if args[0]!=1 or args[1]!=ENTRY or args[2]!=len(data):raise AssertionError('Unexpected virtual file read')
            vm.u.mem_write(args[1],bytes(data));return len(data)
        def scratch(vm,args):
            if args[0]!=1024:raise AssertionError('Unexpected scratch allocation')
            return 0x1004000
        def clock(vm,args):vm.u.reg_write(vm.regs[1],0);return 0
        for address,name,callback,nargs in [
            (0x4600b0,'cache_prepare_mock',0,0),(0x2a5ed4,'file_open_mock',1,2),
            (0x2eb7b0,'file_stat_mock',stat,2),(0x2eb5e0,'file_read_mock',read,3),
            (0x2eb640,'file_close_mock',0,1),(0x299588,'scratch_alloc_mock',scratch,1),
            (0x2995d0,'scratch_free_mock',0,1),(0x339be8,'delay_mock',0,2),
            (0x339d18,'clock_mock',clock,0),(0x298b64,'diagnostic_BX_LR',lambda vm,args:args[0],1),
            (0x4628b8,'sound_start_mock',0,0),(0x4629cc,'sound_stop_mock',0,0),
            (0x339b58,'suspend_mock',0,1),(0x2d6d60,'command_mock',0,3),
            (0x2d8b88,'message_mock',0,3)]:v.stub(address,name,callback,nargs)
        observer=observer_class(v.u,image,DEFAULT_SP,case,original=True)
        observer.firmware_vm=v
        v.calls=[];result=v.run(0x460800,[0x1001000],initial_sp=DEFAULT_SP,
            max_instructions=max_instructions,timeout_us=timeout_us)
        rejected=case.get('corrupt_magic') or case.get('corrupt_footer')
        if rejected:
            if result!=0xfffffffe or observer.entered:raise AssertionError('Malformed container reached entry')
            app=None
        else:
            if result!=0 or observer.return_value is None:raise AssertionError('MMI normal return did not complete')
            app=observer.result(observer.return_value)
            if bytes(v.u.mem_read(ENTRY,len(plain)))!=plain:raise AssertionError('Original RAP decode differs from own packer')
        commands=[args[0] for name,args in v.calls if name=='command_mock']
        expected_commands=[] if rejected else [0xd06,0xd08,0xd07,0xd07]
        if commands!=expected_commands:
            raise AssertionError('MMI command/cleanup sequence mismatch '+repr(commands))
        rows.append({'case':case['name'],'mmi_wrapper_return':result,'entry_reached':observer.entered,
            'application':app,'commands':[hex(c) for c in commands],
            'sp_and_r4_r11_restored':True,'actual_pool_layout_matches_model':True})
    if hashlib.sha256(rap_path.read_bytes()).hexdigest()!=H['8000_MMI.RAP']:raise AssertionError('Original RAP changed')
    return {'source_sha256':H['8000_MMI.RAP'],'execution':'Actual RAP ARM926 BIN loader through own source-built NGame, binder/probe, normal return',
        'case_count':len(rows),'package':manifest,'cases':rows,
        'original_code_executed':['game-pool constructor/allocator/info','native_bin_loader 0x460800',
            'native_read_file 0x45ffcc with mocked file backends','header parser, all memcpy, cipher and footer compare',
            'native API initializer 0x2d32b0','negative-mode native_exit_control 0x4619ac'],
        'mocked':['cache/CP15','virtual file open/stat/read/close','general heap decoder scratch',
            'clock/delay','sound start/stop and thread suspend','command/message backends','all nine graphics/input API calls'],
        'limits':['Synthetic pool RAM and launch state, not a board memory capture.',
                  'No physical cache/LCD/GPU/input/audio/RTOS behavior tested.',
                  'Runtime compatibility checks are not full-image SHA validation.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--synthetic-elf',type=Path)
    p.add_argument('--native-elf',type=Path);p.add_argument('--rap',type=Path)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    if a.synthetic_elf:result=synthetic(a.synthetic_elf)
    elif a.native_elf and a.rap:result=private_rap(a.native_elf,a.rap)
    else:p.error('Provide --synthetic-elf or --native-elf with --rap')
    a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',result['case_count'],'packaged native binding/probe scenarios')
