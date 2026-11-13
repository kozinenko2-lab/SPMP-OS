#!/usr/bin/env python3
"""Original ARM pool/lifecycle/resource observations in synthetic RAM only.
No MMIO, console, real files or games. Every backend interception is explicit.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H
import native_lifecycle_models as model

OUT=0x1002000;PATH=0x1001000;TABLE=0x1003000;DATA=0x1004000
FIELDS={'requests':(0x564200,1),'midi_state':(0x564201,1),'wave_state':(0x564208,1),
        'midi_remaining':(0x564220,4),'midi_repeats':(0x564204,4),
        'wave_base':(0x564210,4),'wave_repeats':(0x564218,4)}
INITIAL={'requests':0xab,'midi_state':1,'wave_state':11,'midi_remaining':64,
         'midi_repeats':3,'wave_base':DATA,'wave_repeats':4}

def new_vm(blob,pool=False):
    v=ArmVM(blob)
    if pool:
        # Fill holes around the harness's existing RAM, stack and callback pages.
        # This is synthetic RAM covering the allocator's configured range.
        for start,end in [(0x9ff000,0x1000000),(0x1120000,0x1200000),(0x1210000,0x2000000)]:
            v.u.mem_map(start,end-start)
        def unlock(vm,args):vm.put32(0x55b738,0);return 0
        def assertion(vm,args):raise RuntimeError('OEM assertion: '+vm.string(args[0]))
        v.put32(0x55b738,0);v.stub(0x33bf08,'scheduler_unlock',unlock,1)
        v.stub(0x2815bc,'assert',assertion,1)
    return v

def write_state(v,state):
    for name,(address,size) in FIELDS.items():v.u.mem_write(address,state[name].to_bytes(size,'little'))

def read_state(v):
    return {name:int.from_bytes(v.u.mem_read(address,size),'little') for name,(address,size) in FIELDS.items()}

def pool_info(v):
    v.run(0x2e37c0,[OUT])
    return {name:v.get32(OUT+offset) for name,offset in [('managed',0),('used',28),('free',32),('max_request',40)]}

def wav_fixture(rate=8000,bits=8):
    samples=bytes([0,128,255,128])*4
    return (b'RIFF'+struct.pack('<I',36+len(samples))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,1,1,rate,rate*bits//8,bits//8,bits)
            +b'data'+struct.pack('<I',len(samples))+samples)

def compare(rap):
    blob=rap.read_bytes()
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Wrong RAP')
    rows=[]
    def save(group,inputs,result):rows.append({'group':group,'inputs':inputs,'original_arm':result})
    # Startup constructor loop runs backwards; all constructors except the pool
    # initializer are mocked explicitly to avoid unrelated hardware/RTOS effects.
    v=new_vm(blob,pool=True)
    constructors=[v.im.word(a) for a in range(0x56ad08,0x56ad5c,4)]
    for address in constructors:
        if address!=0x2e3908:v.stub(address,'other_ctor_'+hex(address),0)
    v.run(0x299a98)
    assert [c[0] for c in v.calls if c[0].startswith('other_ctor_')]==['other_ctor_'+hex(a) for a in reversed(constructors) if a!=0x2e3908]
    assert pool_info(v)==model.fresh_pool_layout()['initial']
    save('constructor_pool',{}, {'table_entries':len(constructors),'pool_initializer':'0x2e3908','pool_is_last':True,'pool_created':True})
    # Execute the actual allocator, its pool-info and its coalescing free path.
    for prefix in [[],[0x100000],[0x200000,0x8000]]:
        v=new_vm(blob,pool=True);v.run(0x2e3908);before=pool_info(v)
        allocations=[];frames=[]
        for size in prefix+[0x25800,0x25800]:
            address=v.run(0x2e3724,[size]);allocations.append({'requested':size,'charged':v.get32(address-4),'address':address})
            frames.append(address)
        maximum=pool_info(v)['max_request'];main=v.run(0x2e3724,[maximum])
        allocations.append({'requested':maximum,'charged':v.get32(main-4),'address':main})
        result={'initial':before,'allocations':allocations,'frames':frames[-2:],
                'main':main,'requested':maximum,'capacity':(main+maximum-model.BIN_ENTRY)&0xffffffff}
        assert result==model.fresh_pool_layout(prefix)
        for a in [main]+frames[-2:]+list(reversed(frames[:-2])):v.run(0x2e3784,[a])
        assert pool_info(v)==before
        save('fresh_pool',{'prefix_requests':prefix},{**result,'restored':True})
    # Full original Game thread with real allocator, mocked BIN-loader result.
    v=new_vm(blob,pool=True);v.run(0x2e3908);v.u.mem_write(PATH,b'/fixture/control.BIN\0')
    v.stub(0x339be8,'delay',0,2);v.stub(0x2d8b88,'message',0,3)
    for loader_result in [-5,-4,-3,-2,-1,0,1,2]:
        snapshot={}
        def loader(vm,args):
            snapshot.update(kind=vm.get32(0x5641dc),base=vm.get32(0x693c5c),capacity=vm.get32(0x5641d8),
                            frame0=vm.get32(0x5641e0),frame1=vm.get32(0x5641e4))
            return loader_result
        v.calls=[];v.stub(0x460800,'bin_loader',loader,1)
        result=v.run(0x460ab0,[PATH]);assert result==loader_result&0xffffffff
        expected=model.fresh_pool_layout()
        assert snapshot=={'kind':6,'base':expected['main'],'capacity':expected['capacity'],
                          'frame0':expected['frames'][0],'frame1':expected['frames'][1]}
        messages=[x[1] for x in v.calls if x[0]=='message'];assert messages==model.loader_messages(loader_result)
        assert [v.get32(a) for a in [0x693c5c,0x5641e0,0x5641e4]]==[0,0,0]
        assert v.get32(0x5641d8)==snapshot['capacity'] # OEM leaves capacity stale.
        assert bytes(v.u.mem_read(0x5641b4,28))==bytes(28)
        assert pool_info(v)==expected['initial']
        save('game_thread',{'loader_result':loader_result},{'return':result,'at_loader':snapshot,
             'messages':messages,'freed_and_cleared':True,'capacity_after':v.get32(0x5641d8)})
    # Fault predicates: allocation warning is not a control-flow abort.
    for base,maximum,frame0 in [(0,0x100000,0x1005000),(0xa01000,0x100000,0x1005000),(0x9fff10,0x100000,0)]:
        v=new_vm(blob);v.u.mem_write(PATH,b'/fixture/control.BIN\0');previous=0x12345678
        v.put32(0x5641d8,previous);requests=[];answers=iter([frame0,0x1006000,base])
        def allocate(vm,args):requests.append(args[0]);return next(answers)
        def info(vm,args):vm.put32(args[0]+40,maximum);return args[0]
        captured=[]
        def loader(vm,args):captured.append(vm.get32(0x5641d8));return 0
        for at,name,fn,nargs in [(0x2e3724,'alloc',allocate,1),(0x2e37c0,'info',info,1),
            (0x2e3784,'free',0,1),(0x460800,'bin_loader',loader,1),(0x339be8,'delay',0,2),(0x2d8b88,'message',0,3)]:v.stub(at,name,fn,nargs)
        assert v.run(0x460ab0,[PATH])==0 and requests==[0x25800,0x25800,maximum]
        expected=model.reservation(base,maximum,previous)
        actual={'capacity_written':captured[0]!=previous,'capacity':captured[0],
                'warning':any(x[0]=='message' for x in v.calls),'loader_reached':bool(captured)}
        assert actual==expected
        save('reservation_fault',{'base':base,'maximum':maximum,'previous_capacity':previous,'frame0':frame0},actual)
    # Audio resource state wrappers: original byte stores and stop's early return.
    for op,entry in [('pause',0x4623bc),('resume',0x462448),('stop',0x4624d4)]:
        cases=[(features,mask,INITIAL) for features in [0,1,2,15] for mask in [0,1,2,3,255]]
        if op=='stop':cases.extend([(15,3,{**INITIAL,'midi_state':3}),(15,3,{**INITIAL,'wave_state':13})])
        for features,mask,state in cases:
            v=new_vm(blob);v.put32(0x5641fc,features);write_state(v,state);v.run(entry,[mask])
            actual=read_state(v);assert actual==model.resource_control(op,mask,features,state)
            save('resource_control',{'operation':op,'mask':mask,'features':features,'state':state},actual)
    # Resource lookup may classify missing names without writing the output.
    for name in ['tone.wav','missing.wav','missing.mid','vibration','backlight','TONE.WAV','unknown.dat']:
        v=new_vm(blob);v.u.mem_write(PATH,name.encode()+b'\0');v.u.mem_write(DATA,wav_fixture())
        v.u.mem_write(TABLE,b'tone.wav'.ljust(16,b'\0')+struct.pack('<I',DATA)+b'TAEND'.ljust(16,b'\0')+bytes(4))
        v.put32(0x697d10,TABLE);v.put32(OUT,0xfeedbeef);v.put32(OUT+4,0xdeadbeef)
        kind=v.run(0x461fb4,[PATH,OUT]);assert kind==model.resource_kind(name)
        written=name=='tone.wav';values=[v.get32(OUT),v.get32(OUT+4)]
        assert values==([DATA,len(wav_fixture())] if written else [0xfeedbeef,0xdeadbeef])
        save('resource_get',{'name':name},{'kind':kind,'output_written':written,'output':values})
    for missing_table,null_output in [(True,False),(False,True)]:
        v=new_vm(blob);v.u.mem_write(PATH,b'tone.wav\0');v.put32(0x697d10,0 if missing_table else TABLE)
        v.put32(0x5641fc,15);v.put32(OUT,0xfeedbeef)
        status=v.run(0x461fb4,[PATH,0 if null_output else OUT])
        assert status==0 and v.get32(0x5641fc)==0 and v.get32(OUT)==0xfeedbeef
        save('resource_get_invalid',{'missing_table':missing_table,'null_output':null_output},
             {'kind':status,'features_after':0,'output_written':False})
    # Play uses the original WAV parser; only the MIDI decoder/sample backend is mocked.
    for rate,bits,repeats,features,null_descriptor in [(8000,8,0,15,False),(8000,8,1,15,False),
            (8000,8,255,15,False),(8000,8,256,15,False),(11025,8,1,15,False),
            (8000,16,1,15,False),(8000,8,1,0,False),(8000,8,1,15,True)]:
        v=new_vm(blob);v.u.mem_write(DATA,wav_fixture(rate,bits));v.put32(OUT,DATA);v.put32(OUT+4,len(wav_fixture(rate,bits)))
        v.put32(0x5641fc,features);write_state(v,INITIAL)
        status=v.run(0x462118,[1,repeats,0 if null_descriptor else OUT])
        accepted=bool(features&1) and not null_descriptor and rate==8000 and bits==8
        expected_status=0 if null_descriptor else 0xffffffff if features&1 and not accepted else 1
        assert status==expected_status and int.from_bytes(v.u.mem_read(0x564200,1),'little')==INITIAL['requests']|1
        if accepted:
            assert v.get32(0x564210)==DATA+44 and v.get32(0x56420c)==DATA+44
            assert v.get32(0x564214)==16 and v.get32(0x564218)==model.repeat_count(repeats)
            assert v.u.mem_read(0x564208,1)==b'\x0b'
        else:assert read_state(v)['wave_base']==INITIAL['wave_base']
        save('resource_play_wave',{'rate':rate,'bits':bits,'repeats':repeats,'features':features,'null_descriptor':null_descriptor},
             {'status':status,'scheduled':accepted,'repeat_count':v.get32(0x564218) if accepted else None})
    for cached,decoder_result in [(False,1),(False,-3),(True,1)]:
        v=new_vm(blob);v.put32(OUT,DATA);v.put32(OUT+4,20);v.put32(0x5641fc,15)
        v.put32(0x564228,0x1005000);v.put32(0x697d14,DATA if cached else 0);v.put32(0x56421c,8)
        v.stub(0x46438c,'midi_decode',decoder_result,1);pending=iter([1,0])
        v.stub(0x464d9c,'samples_pending',lambda vm,args:next(pending),0)
        decoded=struct.pack('<4h',1,-1,32767,-32768)
        def samples(vm,args):vm.u.mem_write(args[0],decoded);return len(decoded)
        v.stub(0x464da8,'read_samples',samples,2)
        status=v.run(0x462118,[2,0,OUT]);assert status==1
        assert v.get32(0x564220)==8 and v.get32(0x564204)==model.repeat_count(0)
        assert v.u.mem_read(0x564201,1)==b'\x01'
        decoded_called=any(x[0]=='midi_decode' for x in v.calls)
        assert decoded_called==(not cached)
        if not cached:assert bytes(v.u.mem_read(0x1005000,8))==decoded
        save('resource_play_midi',{'cached':cached,'decoder_result':decoder_result},
             {'status':status,'decoded_called':decoded_called,'remaining_bytes':v.get32(0x564220),'repeat_count':v.get32(0x564204)})
    # Workspace helper's cached-kind-5 path returns the fixed default, not cached.
    for kind,cached,backend in [(6,0,DATA),(5,0,DATA),(5,DATA,0x1005000),(5,0,0)]:
        v=new_vm(blob);v.put32(0x564238,cached);v.stub(0x45ffbc,'kind',kind)
        v.stub(0x2e3724,'alloc',backend,1);v.stub(0x3b9624,'notification',0,1)
        returned=v.run(0x461e14);actual={'result':returned,'cached':v.get32(0x564238),'allocated':any(x[0]=='alloc' for x in v.calls)}
        assert actual==model.audio_workspace(kind,cached,backend)
        save('audio_workspace',{'kind':kind,'cached':cached,'backend_result':backend},actual)
    # InitRes: original clears fixed workspace; failed file/setup results are ignored.
    for initialized,table in [(0,TABLE),(1,TABLE),(0,0)]:
        v=new_vm(blob,pool=True);v.stub(0x45ffbc,'kind',6)
        v.u.mem_write(0x5641ec,bytes([initialized]));v.u.mem_write(model.AUDIO_BASE,b'\xaa'*model.AUDIO_PCM_BYTES)
        v.stub(0x45ffcc,'tonelib_read',2,4);v.stub(0x464f78,'tonelib_setup',0xffffffff,1)
        v.run(0x461e7c,[0,table]);acted=not initialized and bool(table)
        assert bytes(v.u.mem_read(model.AUDIO_BASE,model.AUDIO_PCM_BYTES))==(bytes(model.AUDIO_PCM_BYTES) if acted else b'\xaa'*model.AUDIO_PCM_BYTES)
        reads=[c[1] for c in v.calls if c[0]=='tonelib_read']
        if acted:
            assert len(reads)==1 and reads[0][0]==0x1000000 and reads[0][2]==model.TONELIB_CAPACITY
            assert v.get32(0x697d10)==table and v.u.mem_read(0x5641ec,1)==b'\x01'
        else:assert reads==[]
        save('resource_init',{'initialized':initialized,'table':table},
             {'cleared_bytes':model.AUDIO_PCM_BYTES if acted else 0,'tonelib_destination':reads[0][0] if reads else None,
              'initialized_after':int.from_bytes(v.u.mem_read(0x5641ec,1),'little')})
    # Exit call order: backends return in this fixture; actual self-suspend can block.
    for mode,handle in [(0x80000000,DATA),(0xffffffff,DATA),(0,DATA),(1,DATA),(0,0)]:
        v=new_vm(blob);seq=0x1005000;requests=3
        v.put32(0x5567e0,handle);v.put32(0x5641f8,seq);v.u.mem_write(0x564200,bytes([requests]))
        for at,name,nargs in [(0x4623bc,'pause',1),(0x339b58,'suspend',1),(0x4629cc,'sound_stop',0),
            (0x2d6d60,'command',3),(0x339b94,'kill',1)]:v.stub(at,name,0,nargs)
        status=v.run(0x4619ac,[mode]);assert status==0 and v.calls==model.exit_calls(mode,handle,seq,requests)
        after=v.get32(0x5567e0);assert after==(0 if mode<0x80000000 else handle)
        save('exit_control',{'mode':mode,'game_handle':handle,'seq_handle':seq,'requests':requests},
             {'calls':v.calls,'handle_after':after,'status':status})
    # Actual stopSound continues to release buffers even when mocked delete returns 0.
    for delete_result in [0,1]:
        v=new_vm(blob);write_state(v,INITIAL);v.put32(0x5641fc,15)
        v.put32(0x564238,DATA);v.put32(0x56422c,DATA+0x1000);v.put32(0x5641f8,DATA+0x2000);v.put32(0x56423c,1)
        for at,name,result,nargs in [(0x2e3784,'pool_free',0,1),(0x2995d0,'general_free',0,1),
            (0x339b18,'delete',delete_result,1),(0x2c5850,'pcm_stop',0,0),(0x339be8,'delay',0,2)]:v.stub(at,name,result,nargs)
        status=v.run(0x4629cc)
        assert status==0 and [v.get32(a) for a in [0x564238,0x56422c,0x56423c]]==[0,0,0]
        assert [x[0] for x in v.calls]==['pool_free','delete','pcm_stop','general_free','delay']
        save('sound_stop',{'delete_result':delete_result},{'status':status,'calls':v.calls,'pointers_cleared':True,'seq_handle_after':v.get32(0x5641f8)})
    rows.extend(loader_return_observations(blob))
    rows.extend(worker_observations(blob))
    return {'source_sha256':H['8000_MMI.RAP'],'case_count':len(rows),'cases':rows,
        'limits':['Synthetic RAM, no MMIO or device execution. Allocator internals execute; scheduler unlock is mocked.',
                  'File/PCM/thread operations are mocked; stop/delete progress does not prove real scheduler liveness.',
                  'Worker is observed until first submit/idle delay, not a returning function or an audio driver test.',
                  'Configured OEM pool range does not certify physical RAM or unused application memory.']}

def loader_return_observations(blob):
    from audit_stage3 import extract_native_api
    rows=[]
    for load_status,entry_return in [(0,12345),(0,0xfffffff9),(2,0)]:
        v=new_vm(blob,pool=True);v.run(0x2e3908);v.u.mem_write(PATH,b'/fixture/control.BIN\0')
        game_handle=0x1009000;seq_handle=0x100a000
        v.put32(0x5567e0,game_handle);v.put32(0x56423c,0)
        v.stub(0x339be8,'delay',0,2);v.stub(0x4600b0,'cache_prepare',0)
        v.stub(0x4617e4,'ngame_loader',load_status,4);v.stub(0x339d18,'clock',0)
        v.stub(0x2c554c,'pcm_configure',0,2);v.stub(0x3e30bc,'get_volume',15);v.stub(0x2c4de8,'set_volume',0,1)
        thread_created=[]
        def create(vm,args):
            from unicorn.arm_const import UC_ARM_REG_SP
            sp=vm.u.reg_read(UC_ARM_REG_SP);stack=[vm.get32(sp+i*4) for i in range(4)]
            thread_created.append(args+stack);vm.put32(stack[2],seq_handle);return 0
        v.stub(0x339aac,'create_audio_thread',create,4);v.stub(0x339b6c,'resume',0,1)
        for at,name,nargs in [(0x339b58,'suspend',1),(0x339b18,'delete',1),(0x2c5850,'pcm_stop',0),(0x2d8b88,'message',3)]:v.stub(at,name,0,nargs)
        commands=[]
        def command(vm,args):
            commands.append({'id':args[0],'data':vm.get32(args[1]) if args[2]==4 else None,'bytes':args[2]});return 0
        v.stub(0x2d6d60,'command',command,3);entry_seen=[]
        def entry(vm,args):
            assert args[0]==0x5efb74
            slots=extract_native_api(vm.im)
            assert [vm.get32(args[0]+4*i) for i in range(85)]==[int(s['target'],16) for s in slots]
            entry_seen.append({'r0':args[0],'callbacks':[vm.get32(0x5641b4+i) for i in [4,8,12]],
                               'capacity':vm.get32(0x5641d8)})
            return entry_return
        v.stub(0xa00000,'application_return_fixture',entry,1)
        returned=v.run(0x460ab0,[PATH]);expected=model.fresh_pool_layout()
        assert returned==(0 if load_status==0 else 0xfffffffe)
        assert pool_info(v)==expected['initial']
        if load_status==0:
            assert len(entry_seen)==1 and entry_seen[0]['callbacks']==[0x46190c,0x46195c,0x4619ac]
            assert len(thread_created)==1 and thread_created[0][:3]==[23,0x4625b8,0]
            assert thread_created[0][4:]==[0x693c60,0x4000,0x5641f8,0x697c70]
            assert commands==[{'id':0xd06,'data':None,'bytes':0},{'id':0xd08,'data':3,'bytes':4},
                              {'id':0xd07,'data':None,'bytes':0},{'id':0xd07,'data':None,'bytes':0}]
            assert v.get32(0x56423c)==0 and v.get32(0x5567e0)==game_handle
        else:assert not entry_seen and not thread_created and not commands
        assert [v.get32(a) for a in [0x693c5c,0x5641e0,0x5641e4]]==[0,0,0]
        rows.append({'group':'loader_return','inputs':{'mocked_load_status':load_status,'mocked_application_return':entry_return},
             'original_arm':{'return':returned,'entry':entry_seen,'audio_threads_created':len(thread_created),
                             'commands':commands,'pool_restored':True,'game_handle_after':v.get32(0x5567e0)}})
    return rows

def worker_observations(blob):
    from unicorn import UC_HOOK_CODE
    from unicorn.arm_const import UC_ARM_REG_SP,UC_ARM_REG_LR
    rows=[]
    for case in ['wave','midi','mixed','partial','idle']:
        v=new_vm(blob);mix=0x1005000;wave=0x1007000;midi=0x1008000
        w=(bytes([0,128,255])*171)[:512] if case in ('wave','mixed') else bytes([0,128,255]) if case=='partial' else b''
        m=([-32768,32767,1,-1]*128) if case in ('midi','mixed') else []
        captured=[];injected=False;stopped=False
        def inject(u,address,size,user):
            nonlocal injected
            if injected:return
            injected=True
            v.u.mem_write(mix,b'\x6a'*1024);v.put32(0x564230,0)
            if w:
                v.u.mem_write(wave,w);v.put32(0x564210,wave);v.put32(0x56420c,wave)
                v.put32(0x564214,len(w));v.put32(0x564218,1);v.u.mem_write(0x564208,b'\x0b')
            if m:
                v.u.mem_write(midi,struct.pack('<512h',*m));v.put32(0x564224,midi);v.put32(0x564228,midi)
                v.put32(0x564220,len(m)*2);v.put32(0x56421c,len(m)*2);v.put32(0x564204,1);v.u.mem_write(0x564201,b'\x01')
        def submit(vm,args):
            nonlocal stopped
            captured.append(bytes(vm.u.mem_read(args[0],args[1])));stopped=True;vm.u.emu_stop();return 0
        def delay(vm,args):
            nonlocal stopped
            stopped=True;vm.u.emu_stop();return 0
        v.stub(0x299588,'alloc_mix_buffer',mix,1);v.stub(0x2c567c,'pcm_submit',submit,2);v.stub(0x339be8,'idle_delay',delay,2)
        v.u.hook_add(UC_HOOK_CODE,inject,begin=0x46269c,end=0x46269c)
        v.u.reg_write(UC_ARM_REG_SP,0x1110000);v.u.reg_write(UC_ARM_REG_LR,0x1200000)
        v.u.emu_start(0x4625b8,0x1200000,timeout=10000000,count=1000000)
        assert stopped and injected
        if case=='idle':assert not captured
        else:
            expected=struct.pack('<'+'h'*max(len(w),len(m)),*model.mixed_samples(w,m))
            if case=='partial':expected=model.observed_partial_flush(expected+b'\x6a'*(1024-len(expected)),len(expected))
            assert captured==[expected]
        payload=captured[0] if captured else b''
        rows.append({'group':'audio_worker','inputs':{'case':case},'original_arm':{
            'submitted_bytes':len(payload),'sha256':hashlib.sha256(payload).hexdigest(),
            'first_samples':list(struct.unpack('<'+str(min(8,len(payload)//2))+'h',payload[:16])),
            'last_bytes_hex':payload[-8:].hex()}})
    return rows

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=compare(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',result['case_count'],'original ARM pool/lifecycle/resource scenarios')
