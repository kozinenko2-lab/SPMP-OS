#!/usr/bin/env python3
"""Bounded private control-game ARM startup audit; no MMIO or physical device.
Inputs must match exact game/RAP hashes. No game code/assets are exported.
The file backend, decoder scratch allocation/delay and bulk memcpy are explicitly replaced with RAM-only operations.
All API calls from the game are intercepted, never passed through to hardware.
"""
import argparse,json,struct
from pathlib import Path
from audit_boot import H
from audit_stage3 import extract_native_api
from emulate_native_lifecycle import new_vm
from control_game_inspect import decode,inspect,ControlImage,sha,ENTRY,GAME_SHA256

API=0x5efb74

def loader_check(rap,data,payload):
    v=new_vm(rap,pool=True);trace=[]
    def read_file(vm,args):
        dest,path,capacity,out=args
        if dest!=ENTRY or len(data)>=capacity:raise AssertionError('VM loader capacity mismatch')
        vm.u.mem_write(dest,data);vm.put32(out,len(data));trace.append({'bytes':len(data),'capacity':capacity})
        return 0
    from unicorn import UC_HOOK_CODE
    def bulk_copy(u,address,size,user):
        if address!=0x298c6c:return
        dest,source,count=[u.reg_read(r) for r in v.regs[:3]]
        if count<=1024:return # Execute original memcpy for crypto, magic and decoded prefix.
        if (dest,source,count)!=(ENTRY,ENTRY+128,len(data)-128):raise AssertionError('Unexpected bulk copy')
        u.mem_write(dest,bytes(u.mem_read(source,count)))
        u.reg_write(v.regs[0],dest);u.reg_write(v.pc,u.reg_read(v.lr))
    v.u.hook_add(UC_HOOK_CODE,bulk_copy)
    v.stub(0x45ffcc,'RAM_file_backend',read_file,4)
    def allocate(vm,args):
        if args[0]!=1024:raise AssertionError('Unexpected decoder scratch size')
        return 0x1004000
    v.stub(0x299588,'RAM_decoder_scratch',allocate,1)
    v.stub(0x2995d0,'RAM_decoder_scratch_free',0,1)
    v.stub(0x339be8,'thread_delay_omitted',0,2)
    v.stub(0x298b64,'diagnostic_BX_LR',lambda vm,args:args[0],1)
    status=v.run(0x4617e4,[ENTRY,0x1001000,0x400000,0x1002000],
                 max_instructions=20000000,timeout_us=30000000)
    actual=bytes(v.u.mem_read(ENTRY,len(payload)))
    if status or actual!=payload:raise AssertionError('Original loader mismatch: status=%s sha=%s calls=%r' % (status,sha(actual),v.calls))
    tail=data[128+len(payload):]
    if bytes(v.u.mem_read(ENTRY+len(payload),len(tail)))!=tail:raise AssertionError('OEM header-shift tail mismatch')
    return {'entry':'0x4617e4','status':status,'decoded_payload_sha256':sha(actual),
        'exact_host_match':True,'tail_preserved_after_header_shift':True,'read_backend':trace,
        'mocked':['0x45ffcc file-read wrapper supplies private uploaded bytes in RAM',
                  '0x298c6c large overlapping header-shift copy replaced with bounded RAM copy; small copies execute original ARM',
                  '0x298b64 bare-return diagnostic intercepted, preserves r0',
                  '0x299588/0x2995d0 1024-byte decoder scratch supplied in synthetic RAM; general heap not initialized',
                  '0x339be8 RTOS delay omitted'],
        'original_execution':'header parser, prefix decoder, crypto helpers, footer comparison and loader branches',
        'max_instructions':20000000,'timeout_us':30000000}

def startup_check(rap,payload,main_stub):
    from unicorn import UC_HOOK_CODE
    from unicorn import arm_const as ar
    v=new_vm(rap,pool=True)
    v.stub(0x339d18,'clock',0)
    v.run(0x2d32b0)
    expected=[int(s['target'],16) for s in extract_native_api(v.im)]
    if list(struct.unpack('<85I',v.u.mem_read(API,340)))!=expected:raise AssertionError('OEM API initializer mismatch')
    v.u.mem_write(ENTRY,payload)
    v.u.mem_write(0xd3e000,b'\xaa'*0xf0c)
    v.put32(0xd3dffc,0xdecafbad);v.put32(0xd3ef0c,0xcafebabe)
    calls=[];checkpoints={};regs=[getattr(ar,'UC_ARM_REG_R'+str(i)) for i in range(13)]
    allowed={4,0x10,0x38,0x90,0x54,0x8c,0x44,0x3c,0xc4,0xd4,0x124,0x100,0x108,0xe8}
    def api_call(vm,args):
        at=vm.u.reg_read(vm.lr)-4;word=vm.get32(at)
        if word&0x0ff0f000!=0x0590f000:raise AssertionError('Unexpected game API call form')
        slot=word&0xfff;base=(word>>16)&15
        if base>=13 or vm.u.reg_read(regs[base])!=API:raise AssertionError('Game API base mismatch')
        if slot not in allowed:raise AssertionError('Unmodeled API slot '+hex(slot))
        if vm.get32(API+slot)!=vm.u.reg_read(vm.pc):raise AssertionError('Game API target mismatch')
        row={'callsite':hex(at),'slot':hex(slot)}
        if slot in (0x38,0x90):row['r0_r1']=args[:2]
        elif slot==0x54:row['input_u16']=list(struct.unpack('<4H',vm.u.mem_read(args[0],8)))
        elif slot==0x44:row['rgb']=list(vm.u.mem_read(args[0],3))
        elif slot in (0x8c,0x3c,0xe8):row['r0']=args[0]
        elif slot==0xc4:row['rect']=list(struct.unpack('<hhHH',vm.u.mem_read(args[0],8)))
        elif slot==0xd4:row['r0_r1']=args[:2]
        elif slot==0x100:
            row['first_word_before']=vm.get32(args[0]);vm.put32(args[0]+4,0x80000)
            row['first_word_after']=vm.get32(args[0]);row['injected_keys']=0x80000
        calls.append(row)
        if len(calls)>100:raise AssertionError('API call bound exceeded')
        return args[0] if slot in (4,0x108) else 1000 if slot==0x124 else 1 if slot==0x100 else 0
    for target in set(expected)-{0}:v.stub(target,'intercepted_game_API',api_call,4)
    def capture(vm,args):
        checkpoints['heap']={name:vm.get32(address) for name,address in
            [('limit',0xd3e1a8),('base',0xd3e1ac),('general_blocks',0xd3e1b0)]}
        return 0xdeadbeef
    if main_stub:v.stub(0xa322e0,'mock_main_returns_deadbeef',capture)
    def guard(u,address,size,user):
        if address==0xa00030:
            zero=bytes(u.mem_read(0xd3e000,0xf0c))
            if zero!=bytes(0xf0c):raise AssertionError('BSS not fully zeroed')
            if v.get32(0xd3dffc)!=0xdecafbad or v.get32(0xd3ef0c)!=0xcafebabe:
                raise AssertionError('BSS boundary guard changed')
            checkpoints['bss']={'bytes':len(zero),'words':len(zero)//4,'all_zero':True,'boundary_guards_preserved':True}
        if address==0xa322e0 and not main_stub:capture(v,[])
        if not ENTRY<=address<0xa3b6ac and address not in v.stubs:
            raise AssertionError('Execution escaped game text/API interceptions: '+hex(address))
    v.u.hook_add(UC_HOOK_CODE,guard)
    v.calls=[];result=v.run(ENTRY,[API],max_instructions=1000000,timeout_us=10000000)
    if result!=0 or v.get32(0xd3e000)!=API:raise AssertionError('Control entry return/API storage mismatch')
    if not main_stub:
        slots=[x['slot'] for x in calls if x['slot']!='0x4']
        if slots!=['0x38','0x90','0x54','0x8c','0x44','0x3c','0xc4','0x10',
                    '0xd4','0x124','0x100','0x108','0xe8','0xe8']:
            raise AssertionError('Unexpected control initialization/quit sequence: '+repr(slots))
    return {'mode':'main_stub' if main_stub else 'first_poll_exit_key','return':result,
        'bss':checkpoints['bss'],'heap':checkpoints['heap'],'api_pointer_saved':hex(v.get32(0xd3e000)),
        'sp_and_callee_saved_registers_restored':True,'calls':calls,
        'limits':'All game API calls mocked; no rendering, real input, resources, audio or RTOS. '+('Main replaced with return stub.' if main_stub else 'Main uses injected quit key on first poll.')}

def compare(rap_path,game_path):
    rap=rap_path.read_bytes();data=game_path.read_bytes()
    if sha(rap)!=H['8000_MMI.RAP']:raise ValueError('Exact OEM RAP required')
    payload,_=decode(data);inspect(data)
    result={'source_sha256':H['8000_MMI.RAP'],'game_sha256':GAME_SHA256,
        'execution':'Unicorn ARM, synthetic RAM only; not console execution',
        'loader':loader_check(rap,data,payload),
        'startup_cases':[startup_check(rap,payload,True),startup_check(rap,payload,False)]}
    if sha(rap_path.read_bytes())!=H['8000_MMI.RAP'] or sha(game_path.read_bytes())!=GAME_SHA256:
        raise AssertionError('Input changed during analysis')
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True)
    p.add_argument('--game',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();r=compare(a.rap,a.game);a.output.write_text(json.dumps(r,indent=2)+'\n')
    print('PASS: actual OEM NGame loader vs control sample; two bounded game startup cases')
