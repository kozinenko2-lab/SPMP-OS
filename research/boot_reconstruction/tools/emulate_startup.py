#!/usr/bin/env python3
"""Execute new source-built startup/probe on Unicorn ARM926, no OEM inputs.
Only synthetic code/data, an inherited stack and explicit callback traps are mapped.
The raw OEM API argument remains opaque and is never dereferenced.
"""
import argparse,json,struct
from pathlib import Path
from startup_elf import StartupELF,ENTRY,BUDGET

STACK_BASE=0x1100000
STACK_BYTES=0x10000
RETURN=0x1200000
TRAMPOLINES=0x1201000
COOKIE=0x53504d50
NAMES=['init_graph','origin','double_buffer','dimensions','get_color','set_color','fill','paint','keys']

def execute(image,sp_mod8=0,raw_api=0x5efb74,thumb_return=False,scenario=None,unused_args=(0x13572468,)*3):
    from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
    from unicorn import arm_const as ar
    u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(ar.UC_CPU_ARM_926)
    u.mem_map(ENTRY,BUDGET);u.mem_map(STACK_BASE,STACK_BYTES);u.mem_map(RETURN,0x2000)
    for s in image.segments:u.mem_write(s['address'],image.data[s['offset']:s['offset']+s['file_bytes']])
    u.mem_write(image.bss_start,b'\xa5'*(image.bss_end-image.bss_start))
    prefix_start=max(ENTRY,image.bss_start-16)
    u.mem_write(image.bss_end,struct.pack('<I',0xdecafbad))
    regs=[getattr(ar,'UC_ARM_REG_R'+str(i)) for i in range(13)]
    saved=[0x45670000+4*i for i in range(8)]
    original_sp=STACK_BASE+0x8000+sp_mod8
    for register,value in zip(regs[4:12],saved):u.reg_write(register,value)
    for register,value in zip(regs,[raw_api,*unused_args]):u.reg_write(register,value)
    u.reg_write(ar.UC_ARM_REG_SP,original_sp);u.reg_write(ar.UC_ARM_REG_LR,RETURN+int(thumb_return))
    callback_addresses={TRAMPOLINES+4*i:name for i,name in enumerate(NAMES)}
    events=[];main_checkpoint={};writes=[];current_color=[12,129,250];key_index=0;minimum_sp=original_sp
    if scenario is not None:
        pointers=[TRAMPOLINES+4*i for i in range(len(NAMES))]
        if scenario.get('missing_keys'):pointers[-1]=0
        values=[0x564d3031,raw_api,*pointers,scenario.get('max_steps',3),1,2,4,8,0x80000]
        u.mem_write(image.address('spmp_vm_configuration'),struct.pack('<17I',*values))
    prefix=bytes(u.mem_read(prefix_start,image.bss_start-prefix_start))
    def read32(address):return struct.unpack('<I',u.mem_read(address,4))[0]
    def write_output(address,data):
        if not (image.bss_start<=address and address+len(data)<=image.bss_end or
                original_sp-0x2000<=address and address+len(data)<=original_sp):
            raise AssertionError('Mock output escaped application/stack RAM')
        u.mem_write(address,data)
    def callback(name):
        nonlocal current_color,key_index
        args=[u.reg_read(r) for r in regs[:4]];row={'name':name};result=0
        if u.reg_read(ar.UC_ARM_REG_SP)%8:raise AssertionError('C API call SP is not 8-byte aligned')
        if name=='origin':
            row['xy']=list(struct.unpack('<hh',u.mem_read(args[0],4)))
            if row['xy']!=[0,0]:raise AssertionError('Display origin mismatch')
        elif name=='double_buffer':
            row['enabled']=args[0]
            if args[0]!=1:raise AssertionError('Double buffer argument mismatch')
        elif name=='dimensions':
            width=319 if scenario.get('bad_dimensions') else 320
            write_output(args[0],struct.pack('<H',width));write_output(args[1],struct.pack('<H',240))
        elif name=='get_color':write_output(args[0],bytes(current_color))
        elif name=='set_color':
            current_color=list(u.mem_read(args[0],3));row['rgb']=current_color
        elif name=='fill':
            row['rect']=list(struct.unpack('<hhHH',u.mem_read(args[0],8)))
            if scenario.get('fill_error'):result=0x406
        elif name=='paint':
            if scenario.get('paint_error'):result=1
        elif name=='keys':
            keys=scenario.get('keys',[0]);key=keys[min(key_index,len(keys)-1)];key_index+=1
            row['keys']=key;row['first_word_before']=read32(args[0])
            write_output(args[0]+4,struct.pack('<I',key));row['first_word_after']=read32(args[0])
            result=0 if scenario.get('keys_error') else 1
        events.append(row)
        # Exercise caller-clobbered registers; preserving them would hide ABI bugs.
        for r in regs[1:4]+[regs[12]]:u.reg_write(r,0xa5a5a5a5)
        u.reg_write(regs[0],result);u.reg_write(ar.UC_ARM_REG_PC,u.reg_read(ar.UC_ARM_REG_LR))
    def code(u,address,size,user):
        nonlocal minimum_sp
        sp=u.reg_read(ar.UC_ARM_REG_SP);minimum_sp=min(minimum_sp,sp)
        if not original_sp-0x2000<=sp<=original_sp:raise AssertionError('SP escaped inherited stack window')
        if address==image.address('spmp_app_main'):
            if u.reg_read(ar.UC_ARM_REG_SP)%8:raise AssertionError('Main SP is not aligned')
            if u.reg_read(regs[0])!=raw_api:raise AssertionError('Startup did not forward r0')
            if bytes(u.mem_read(image.bss_start,image.bss_end-image.bss_start))!=bytes(image.bss_end-image.bss_start):
                raise AssertionError('BSS not zero before main')
            if bytes(u.mem_read(prefix_start,len(prefix)))!=prefix:raise AssertionError('Startup changed data/padding before BSS')
            if read32(image.bss_end)!=0xdecafbad:raise AssertionError('Startup wrote beyond BSS')
            main_checkpoint.update(bss_zero_before_main=True,data_prefix_preserved=True,main_sp_aligned=True)
        if address in callback_addresses:
            if scenario is None:raise AssertionError('Unbound application called API')
            callback(callback_addresses[address]);return
        text=image.sections['.text']
        if not text['address']<=address<text['address']+text['bytes']:
            raise AssertionError('Execution escaped startup text/explicit VM callbacks')
    def write(u,access,address,size,value,user):
        in_bss=image.bss_start<=address and address+size<=image.bss_end
        in_stack=original_sp-0x2000<=address and address+size<=original_sp
        if not (in_bss or in_stack):raise AssertionError('ARM store escaped BSS/inherited stack: '+hex(address))
        writes.append((address,size))
    u.hook_add(UC_HOOK_CODE,code);u.hook_add(UC_HOOK_MEM_WRITE,write)
    u.emu_start(image.entry,RETURN,count=1000000,timeout=10000000)
    if u.reg_read(ar.UC_ARM_REG_PC)!=RETURN:raise AssertionError('Startup exceeded VM execution limit')
    if u.reg_read(ar.UC_ARM_REG_SP)!=original_sp:raise AssertionError('SP not restored')
    if [u.reg_read(r) for r in regs[4:12]]!=saved:raise AssertionError('r4-r11 not restored')
    if bool(u.reg_read(ar.UC_ARM_REG_CPSR)&32)!=thumb_return:raise AssertionError('Return instruction set mismatch')
    if not main_checkpoint:raise AssertionError('Main was not reached')
    if read32(image.bss_end)!=0xdecafbad:raise AssertionError('BSS end guard changed')
    if bytes(u.mem_read(prefix_start,len(prefix)))!=prefix:raise AssertionError('Data/padding prefix changed')
    result={'initial_sp_mod8':sp_mod8,'raw_api_argument':hex(raw_api),'return_instruction_set':'Thumb' if thumb_return else 'ARM',
        **main_checkpoint,'sp_and_r4_r11_restored':True,'bss_end_guard_preserved':True,
        'stores_within_bss_or_inherited_stack':True,'stack_bytes_touched':original_sp-min(a for a,n in writes if STACK_BASE<=a<STACK_BASE+STACK_BYTES),
        'maximum_sp_displacement_bytes':original_sp-minimum_sp,
        'return':u.reg_read(regs[0]),'api_events':events}
    if scenario is None:
        record=list(struct.unpack('<4I',u.mem_read(image.address('spmp_startup_record'),16)))
        expected=1 if raw_api else 2
        if record!=[1,raw_api,COOKIE,expected] or result['return']!=expected:raise AssertionError('Unbound application record mismatch')
        result['startup_record']=record
    else:
        if read32(image.address('spmp_vm_raw_api'))!=raw_api:raise AssertionError('Fixture raw API mismatch')
        if read32(image.address('spmp_vm_result'))!=result['return']:raise AssertionError('Fixture result mismatch')
        state=image.address('spmp_vm_probe_state')
        steps=read32(state+16);x,y=struct.unpack('<hh',u.mem_read(state+24,4))
        finished=bytes(u.mem_read(state+28,4))
        result['probe']={'steps':steps,'x':x,'y':y,'finished':bool(finished[2]),'active':bool(finished[0]),'final_color':current_color}
        expected=scenario.get('expected_return',1)
        if result['return']!=expected or steps!=scenario['expected_steps'] or (x,y)!=scenario.get('expected_xy',(152,112)):
            raise AssertionError('Probe result differs from scenario: '+repr(result['probe']))
        if not finished[2] or finished[0]:raise AssertionError('Probe did not finish')
        if current_color!=[12,129,250]:raise AssertionError('Original FG color not restored')
        frames=sum(e['name']=='paint' for e in events)
        if frames!=scenario.get('expected_paints',steps):raise AssertionError('Paint count mismatch')
        fill_rects=[e['rect'] for e in events if e['name']=='fill']
        if steps and fill_rects[-1]!=[x,y,16,16]:raise AssertionError('Final cursor rectangle mismatch')
    return result

def audit(application,fixture):
    images=[StartupELF(p.read_bytes()) for p in (application,fixture)]
    cases=[]
    for sp in (0,4):
        for raw in (0,0x5efb74,0xfffffffc):
            for thumb in (False,True):
                cases.append({'group':'unbound_application','result':execute(images[0],sp,raw,thumb)})
    scenarios=[
        {'name':'bounded_no_keys','keys':[0],'expected_steps':3},
        {'name':'quit_first_poll','keys':[0x80000],'expected_steps':0},
        {'name':'movement_then_quit','keys':[8,2,0x80000],'expected_steps':2,'expected_xy':(158,118)},
        {'name':'bad_dimensions','bad_dimensions':True,'expected_return':2,'expected_steps':0},
        {'name':'keys_error','keys_error':True,'expected_return':2,'expected_steps':0},
        {'name':'fill_error','fill_error':True,'expected_return':2,'expected_steps':0},
        {'name':'paint_error','paint_error':True,'expected_return':2,'expected_steps':0,'expected_paints':1},
        {'name':'missing_keys','missing_keys':True,'expected_return':2,'expected_steps':0},
        {'name':'zero_steps','max_steps':0,'expected_return':2,'expected_steps':0}]
    for sp in (0,4):
        for scenario in scenarios:
            r=execute(images[1],sp,scenario=scenario,unused_args=(0xffffffff,0,0xdeadbeef))
            cases.append({'group':'compiled_probe_fixture','scenario':scenario['name'],'result':r})
    return {'status':'ARM_VM_TESTED_ONLY','cpu':'Unicorn ARM926','case_count':len(cases),
        'application':images[0].report(),'fixture':images[1].report(),'cases':cases,
        'limits':['Source-built analysis ELFs; not NGame or a deployable Flash image.',
                  'No original firmware or game bytes loaded; raw OEM API argument is opaque.',
                  'Typed callbacks are synthetic VM traps, not bindings to OEM slots.',
                  'No console, real framebuffer/input/audio/IRQ/cache/RTOS execution.',
                  '128 KiB image and 8 KiB touched-stack windows are VM policies, not observed free device RAM.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--application',type=Path,required=True)
    p.add_argument('--fixture',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();result=audit(a.application,a.fixture);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',result['case_count'],'source-built ARM926 startup/probe scenarios')
