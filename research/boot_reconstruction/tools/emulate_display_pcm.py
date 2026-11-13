#!/usr/bin/env python3
"""Original ARM display/PCM checks, synthetic register cells and RAM only.
No actual MMIO, device handles, interrupts, console or real DMA/audio output.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H
from emulate_native_lifecycle import new_vm,pool_info
import display_pcm_models as model

def registers(v,initial,write_addresses=()):
    from unicorn import UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE,UC_MEM_READ
    for page in sorted({a&~4095 for a in list(initial)+list(write_addresses)}):v.u.mem_map(page,4096)
    for address,value in initial.items():v.put32(address,value)
    writes=[];reads=[];permitted=set(write_addresses)
    def access(u,kind,address,size,value,user):
        if size!=4:raise RuntimeError('Unexpected synthetic-register access size')
        if kind==UC_MEM_READ:
            if address not in initial:raise RuntimeError('Unlisted synthetic-register read '+hex(address))
            reads.append(address)
        else:
            if address not in permitted:raise RuntimeError('Unlisted synthetic-register write '+hex(address))
            writes.append([address,value&0xffffffff])
    for page in sorted({a&~4095 for a in list(initial)+list(write_addresses)}):
        v.u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=page,end=page+4095)
    return writes,reads

def compare(rap):
    blob=rap.read_bytes()
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Wrong RAP')
    rows=[]
    def save(group,inputs,result):rows.append({'group':group,'inputs':inputs,'original_arm':result})
    # Capture boot display allocations before Game thread: defaults are image-specific.
    v=new_vm(blob,pool=True);v.run(0x2e3908)
    for address in range(0x555efc,0x555f14,4):v.put32(address,0)
    lcd=[]
    for kind,index in [(1,0),(1,1),(0,0),(0,1)]:
        address=v.run(0x2b97cc,[kind,index]);size=0xb7840 if kind else 0x25800
        lcd.append({'kind':kind,'index':index,'base':address,'bytes':size})
        assert v.run(0x2b97cc,[kind,index])==address
    v.put32(0x6769b0,0)
    assert v.run(0x42f910)==0
    stack_base=v.get32(0x6769b0);stack_allocation={'base':stack_base,'bytes':0x100010}
    assert stack_base and v.run(0x42f910)==0 # cached allocation, not another block
    before=pool_info(v);snapshot={};v.u.mem_write(0x1001000,b'/fixture/control.BIN\0')
    def loaded(vm,args):
        snapshot.update(allocation={'base':vm.get32(0x693c5c),'bytes':vm.get32(0x5641d8)+0xa00000-vm.get32(0x693c5c)},
            framebuffers=[{'base':vm.get32(a),'bytes':0x25800} for a in [0x5641e0,0x5641e4]])
        return 0
    v.stub(0x460800,'bin_loader',loaded,1);v.stub(0x339be8,'delay',0,2);v.stub(0x2d8b88,'message',0,3)
    assert v.run(0x460ab0,[0x1001000])==0 and pool_info(v)==before
    assert v.get32(0x6769b0)==stack_base
    save('lcd_pool_before_game',{}, {'lcd_buffers':lcd,'oem_stack_allocation':stack_allocation,
         'oem_stack':{'base':(stack_base+3)&~3,'bytes':0x100000},**snapshot,'remaining_pool_restored':True})
    # Real Paint -> present -> scanout store -> swap -> RAM copy; RTOS wait mocked.
    for same in [False,True]:
        for update in [0,1]:
            for sync in [0,1]:
                v=ArmVM(blob);render=0x1004000;scan=render if same else 0x1044000
                v.put32(0x5dce4c,render);v.put32(0x606418,scan)
                v.u.mem_write(0x606410,struct.pack('<HH',240,320));v.put32(0x60640c,0x10000)
                v.put32(0x5570d4,update);v.stub(0x29e310,'sync_capability',sync)
                for address,name,nargs in [(0x33a048,'lock',1),(0x33a05c,'unlock',1),(0x339f50,'flag_clear',2),(0x339f64,'flag_wait',3),(0x29e450,'cache_sync',0)]:v.stub(address,name,0,nargs)
                writes,_=registers(v,{},[0x93000054,0x93000078])
                pixels=bytes(range(256))*600 # 153600 bytes
                v.u.mem_write(render,pixels)
                if not same:v.u.mem_write(scan,b'\xaa'*len(pixels))
                status=v.run(0x2dd20c);expected=model.lcd_swap(render,scan,update)
                assert status==0 and v.get32(0x5dce4c)==expected['render'] and v.get32(0x606418)==expected['scanout']
                assert writes==expected['writes'] and bytes(v.u.mem_read(expected['render'],len(pixels)))==pixels
                waited=any(c[0]=='flag_wait' for c in v.calls);assert waited==(not same and bool(sync))
                assert v.get32(0x5fabe4)==expected['render'] if not same else True
                save('lcd_paint',{'same':same,'update':update,'sync':sync},
                    {**expected,'status':status,'copied_bytes':0 if same else len(pixels),'waited':waited})
    # InitGraph selects real driver at 320x240, virtual Game buffers otherwise.
    for width,height in [(320,240),(640,480)]:
        v=ArmVM(blob);v.u.mem_write(0x606410,struct.pack('<HH',height,width))
        v.put32(0x5641e0,0x1004000);v.put32(0x5641e4,0x1044000)
        v.stub(0x31458c,'format_mode',1);v.stub(0x313430,'format_descriptor',0x1003000);v.stub(0x2dd110,'image_cache_initialize',0,1)
        assert v.run(0x2df000)==0
        actual=[v.get32(a) for a in [0x55700c,0x557010,0x557014,0x557018,0x55701c]]
        expected=[0x2a4268,0x2e83c0,0x2e83a0,0x2e83b0,0x2a3fd4] if (width,height)==(320,240) else [0x2def30,0x2def40,0x2def50,0x2def58,0x2def60]
        assert actual==expected
        cache=0x1080000;v.put32(v.im.word(0x2de00c),cache)
        v.put32(0x5dce4c,0x1004000);v.put32(0x606418,0x1044000)
        v.u.mem_write(0x1002000,struct.pack('<hh',0,0))
        assert v.run(0x2ddf70,[0x1002000])==0 and v.run(0x2deab8,[1])==0
        assert list(struct.unpack('<HH',v.u.mem_read(0x5fabe8,4)))==[320,240]
        assert v.get32(0x5fabe4)==(0x1004000 if (width,height)==(320,240) else 0x1044000)
        save('lcd_init_graph',{'width':width,'height':height},{'callback_addresses':actual,'virtual':width!=320 or height!=240,
             'configured_context':[320,240],'render_buffer':v.get32(0x5fabe4)})
    # Graphics wait: only status bits predicate and bounded poll count are modeled.
    for status_word in [0x10,0x14,0x0,0xffffffff]:
        v=ArmVM(blob);v.stub(0x299d80,'delay',0,1)
        _,reads=registers(v,{0x92006010:status_word})
        result=v.run(0x2e479c);ready=(status_word&0x3b)==0x10
        assert result==(0 if ready else 1) and len(reads)==(1 if ready else 300)
        save('graphics_wait',{'status_word':status_word},{'return':result,'polls':len(reads),'delays':len(v.calls)})
    # FillRect signature/bounds and ignored graphics timeout; actual blitter mocked.
    for x,y,w,h,busy in [(0,0,320,240,False),(0,0,320,240,True),(-1024,-1024,1,1,False),
        (1023,1023,0,0,False),(-1025,0,1,1,False),(0,1024,1,1,False),(0,0,1024,1,False)]:
        v=ArmVM(blob);v.u.mem_write(0x1002000,struct.pack('<hhHH',x,y,w,h));parameters=[]
        def blitter(vm,args):
            from unicorn.arm_const import UC_ARM_REG_SP
            sp=vm.u.reg_read(UC_ARM_REG_SP);parameters.append(args+[vm.get32(sp+i*4) for i in range(5)]);return 0
        v.stub(0x313f40,'blitter',blitter,4)
        v.stub(0x299d80,'delay',0,1);registers(v,{0x92006010:0 if busy else 0x10})
        result=v.run(0x2dd52c,[0x1002000]);assert result==model.fill_rect_status(x,y,w,h)
        blits=[c for c in v.calls if c[0]=='blitter'];assert bool(blits)==(result==0)
        if result==0:assert parameters[0][1:5]==[x&0xffffffff,y&0xffffffff,w,h] and parameters[0][8]==0xf0, (x,y,w,h,parameters)
        save('fill_rect',{'x':x,'y':y,'width':w,'height':h,'gpu_busy':busy},
             {'status':result,'blitter_called':bool(blits),'coordinates_forwarded':parameters[0][1:5] if blits else None,'delay_calls':sum(c[0]=='delay' for c in v.calls)})
    # DMA descriptor registers: actual ARM, synthetic cells, no transfer takes place.
    for channel,control,n1,n2 in [(c,ctrl,1024,2048) for c in range(4) for ctrl in [0,0x102000]]+[(0,0,0,4)]:
        v=ArmVM(blob);expected=model.dma_program(channel,0x1004000,n1,0x1044000,n2,control)
        writes,_=registers(v,{0x93012000:control},[a for a,value in expected])
        v.run(0x2a14d0,[channel,0x1004000,n1,0x1044000],stack=[n2]);assert writes==expected
        save('pcm_dma',{'channel':channel,'control':control,'first':0x1004000,'first_bytes':n1,'second':0x1044000,'second_bytes':n2},
             {'writes':writes,'allocator_supported_channel':channel<2})
    # Queue reserve predicate: strict free>request and minimum free threshold.
    for request,read,write,threshold in [(4,100,0,16),(100,100,0,16),(0,8192,0,16),(0,8193,0,16),
        (-1,100,0,16),(4,8,0,16),(4,8,65530,1)]:
        v=ArmVM(blob)
        for address,value in [(0x5141a4,read),(0x5141a0,write),(0x514198,65536),(0x514210,threshold),(0x5141b8,0x1004000)]:v.put32(address,value)
        result=v.run(0x2949f0,[request]);expected=model.reserve_try(request,read,write,65536,threshold,0x1004000)
        assert result==expected
        save('pcm_reserve',{'request':request,'read':read,'write':write,'capacity':65536,'threshold':threshold,'base':0x1004000},{'return':result})
    # Mono duplication / first-start failure semantics, downstream queue mocked.
    for started,channels,count,available in [(s,c,n,True) for s in [0,1] for c in [1,2] for n in [2,4]]+[(0,1,4,False),(1,1,4,False),(0,1,0,True),(1,1,1,True)]:
        v=ArmVM(blob);src=0x1004000;dst=0x1044000;v.u.mem_write(src,bytes(range(32)));v.u.mem_write(dst,b'\xaa'*64)
        v.put32(0x556018,started);v.put32(0x55601c,channels);v.put32(0x556028,0)
        for at,name,ret,nargs in [(0x294ab0,'reserve',dst if available else 0,2),(0x294f30,'commit',0,2),
            (0x292d44,'driver_start',1,0),(0x292fd8,'driver_state',2,0)]:v.stub(at,name,ret,nargs)
        v.run(0x2c567c,[src,count])
        copied=(max(1,(count+1)//2)*4 if channels==1 else count) if available else 0
        expected=(b''.join(bytes([2*i,2*i+1])*2 for i in range(max(1,(count+1)//2))) if channels==1 else bytes(range(count))) if available else b''
        assert bytes(v.u.mem_read(dst,64))==expected+b'\xaa'*(64-copied)
        commits=[c[1][0] for c in v.calls if c[0]=='commit']
        expected_commit=(count*(2 if channels==1 else 1)) if available else 0
        assert commits==([expected_commit] if available or not started else [])
        assert v.get32(0x556018)==1 if not started else v.get32(0x556018)==started
        save('pcm_submit',{'started':started,'channels':channels,'count':count,'available':available},
             {'copied_bytes':copied,'prefix_hex':bytes(v.u.mem_read(dst,max(copied,4))).hex(),'commits':commits,'started_after':v.get32(0x556018)})
    # Real thread-delete wrapper requires state EXITED; only kill/destruct mocked.
    for state,kill_exits in [(16,False),(0,True),(0,False)]:
        v=ArmVM(blob);thread=0x1004000;v.put32(thread+0x38,state)
        def kill(vm,args):
            if kill_exits:vm.put32(args[0]+0x38,16)
            return 0
        v.stub(0x33aaac,'kill',kill,1);v.stub(0x33a57c,'destruct',0,1)
        result=v.run(0x339b18,[thread]);expected=state==16 or kill_exits
        assert result==int(expected) and any(c[0]=='destruct' for c in v.calls)==expected
        save('thread_delete',{'state':state,'kill_exits':kill_exits},{'return':result,'destructed':bool(result),'kill_called':state!=16})
    return {'source_sha256':H['8000_MMI.RAP'],'case_count':len(rows),'cases':rows,
        'limits':['Only synthetic register cells are mapped; accesses are allowlisted and no physical device is accessed.',
                  'Register writes verify instruction arithmetic/order, not LCD latching, DMA completion, IRQs or real PCM output.',
                  'RTOS mutex/wait, allocator scheduler unlock, blitter and PCM queue/driver backends are explicitly mocked per scenario.',
                  'Pool scenario assumes default panel table, one shell stack allocation and an otherwise empty pool.',
                  'VM Game thread uses the harness stack; real RTOS stack initialization/scheduling is not executed.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=compare(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',result['case_count'],'original ARM display/PCM/delete scenarios')
