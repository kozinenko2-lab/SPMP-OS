#!/usr/bin/env python3
"""Original ARM wrapper checks with explicitly mocked RAM-only backends.
No real file, framebuffer, device input, sound, RTOS or hardware access occurs.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from arm_abi_vm import ArmVM
from audit_boot import H
from native_abi_models import file_result,open_result,rgb565,rgb888_truncated,image_query,framebuffer_request,pool_allocate,general_allocate,cooked_keys,signed
OUT=0x1001000;BUF=0x1002000

def compare(rap):
    blob=rap.read_bytes();assert hashlib.sha256(blob).hexdigest()==H['8000_MMI.RAP'];rows=[]
    def save(group,inputs,result):rows.append({'group':group,'inputs':inputs,'original_arm':result,'host_model_matches':True})
    # File wrappers: backend return is injected; original ARM gates fd and writes out-count.
    config={'read':(0x462b48,0x2eb5e0,3),'write':(0x462bb0,0x2eb610,3),
            'close':(0x462c14,0x2eb640,1),'seek':(0x462c64,0x2eb69c,3)}
    cases=[]
    for op in ['read','write']:
        cases.extend((op,fd,4,ret) for fd,ret in [(0,4),(1,4),(32,4),(33,4),(0xffffffff,-1),(1,2),(1,-1),(1,0)])
    for op in ['close','seek']:
        cases.extend((op,fd,0,ret) for fd,ret in [(0,0),(32,17),(33,0),(0xffffffff,-1),(1,-1)])
    for op,fd,count,ret in cases:
        vm=ArmVM(blob);entry,backend,nargs=config[op];vm.stub(backend,'backend',ret,nargs)
        vm.put32(OUT,0xfeedbeef)
        args=[fd,BUF,count,OUT] if op in ['read','write'] else [fd] if op=='close' else [fd,-7,2]
        status=vm.run(entry,args);called=bool(vm.calls)
        result={'status':status,'count':signed(vm.get32(OUT)) if called and op in ['read','write'] else None,'backend_called':called}
        assert result==file_result(op,fd,count,ret),(op,fd,ret,result)
        if called:
            expected=[fd,BUF,count] if op in ['read','write'] else [fd] if op=='close' else [fd,0xfffffff9,2]
            assert vm.calls==[['backend',expected]],vm.calls
        if result['count'] is None:assert vm.get32(OUT)==0xfeedbeef
        save('file_'+op,{'fd':fd,'count':count,'backend_result':ret},result)
    # Open: original concatenates a controlled VM-only prefix, and does not close on stat failure.
    for fd,statret in [(0,0),(4,0),(-1,0),(4,-1)]:
        vm=ArmVM(blob);prefix=0x1003000;filename=0x1003100
        vm.u.mem_write(prefix,b'/fixture/\0');vm.u.mem_write(filename,b'file.dat\0')
        vm.put32(vm.im.word(0x462b3c),prefix);vm.put32(OUT,0xfeedbeef)
        observed_paths=[]
        def opened(v,args):observed_paths.append(v.string(args[0]));return fd
        vm.stub(0x2a5ed4,'open',opened,2);vm.stub(0x2eb7b0,'stat',statret,1);vm.stub(0x2eb640,'close',0,1)
        status=vm.run(0x462a74,[filename,1,OUT])
        result={'status':status,'fd':vm.get32(OUT) if status==0 else None,'stat_called':any(x[0]=='stat' for x in vm.calls)}
        assert result==open_result(fd,statret)
        assert observed_paths==['/fixture/file.dat']
        assert not any(x[0]=='close' for x in vm.calls)
        if status!=0:assert vm.get32(OUT)==0xfeedbeef
        save('file_open',{'backend_fd':fd,'stat_result':statret},result)
    # Display getters return dimensions, not a framebuffer address.
    for entry,name in [(0x2de124,'get_framebuffer_dimensions'),(0x2de020,'get_display_rect')]:
        vm=ArmVM(blob);vm.put32(0x557014,0x1201000);vm.put32(0x557018,0x1201010)
        vm.stub(0x1201000,'width',320);vm.stub(0x1201010,'height',240)
        vm.u.mem_write(OUT,b'\xef\xbe'*4)
        status=vm.run(entry,[OUT,OUT+2] if entry==0x2de124 else [OUT])
        values=list(struct.unpack('<2H' if entry==0x2de124 else '<4H',vm.u.mem_read(OUT,4 if entry==0x2de124 else 8)))
        assert status==0 and values==([320,240] if entry==0x2de124 else [0,0,320,240])
        save('graphics_'+name,{}, {'status':status,'output_u16':values})
    # Requested dimensions only gate arguments; clear size comes from mocked backend dimensions.
    for width,height in [(1,1),(0,0),(1024,3),(2,1024),(65538,65539),(65535,1)]:
        vm=ArmVM(blob)
        for var,address,label,result in [(0x557014,0x1201000,'width',2),(0x557018,0x1201010,'height',3),
              (0x55700c,0x1201020,'buffer_55700c',0x1004000),(0x557010,0x1201030,'buffer_557010',0x1005000)]:
            vm.put32(var,address);vm.stub(address,label,result)
        vm.stub(0x29e450,'cache_sync',0)
        vm.u.mem_write(0x1004000,b'\xaa'*16);vm.u.mem_write(0x1005000,b'\xbb'*16)
        status=vm.run(0x2de06c,[width,height]);expected=framebuffer_request(width,height)
        assert status==expected
        cleared=bytes(vm.u.mem_read(0x1004000,16))
        assert cleared==(b'\0'*12+b'\xaa'*4 if expected==0 else b'\xaa'*16)
        assert bytes(vm.u.mem_read(0x1005000,16))==b'\xbb'*16
        save('graphics_set_framebuffer',{'requested_width':width,'requested_height':height,'driver_width':2,'driver_height':3},
             {'status':status,'bytes_cleared':12 if expected==0 else 0})
    for rgb in [(0,0,0),(255,255,255),(255,0,255),(12,129,250)]:
        vm=ArmVM(blob);vm.u.mem_write(BUF,bytes(rgb));vm.stub(0x312d18,'set_color',0,2)
        status=vm.run(0x2dde4c,[BUF]);encoded=vm.calls[0][1][1]
        assert status==0 and encoded==rgb565(rgb)
        save('graphics_set_fg',{'rgb':list(rgb)},{'status':status,'rgb565':encoded})
    for color in [0,0xffff,0xf81f,0x1234]:
        vm=ArmVM(blob);vm.stub(0x312d20,'get_color',color,1)
        status=vm.run(0x2ddea0,[OUT]);rgb=list(vm.u.mem_read(OUT,3))
        assert status==0 and rgb==rgb888_truncated(color)
        save('graphics_get_fg',{'rgb565':color},{'status':status,'rgb':rgb})
    for image_id,field,active in [(2,1,True),(2,2,True),(2,3,True),(2,4,True),(2,5,True),(1,1,True),(4,1,True),(2,1,False),(258,257,True)]:
        vm=ArmVM(blob);cache=0x1030000
        vm.put32(vm.im.word(0x2ddc38),4);vm.put32(vm.im.word(0x2ddc3c),cache)
        record=cache+0x24
        vm.u.mem_write(record,bytes([int(active)]));vm.put32(record+4,3)
        vm.u.mem_write(record+8,struct.pack('<HH',320,240));vm.u.mem_write(record+0x11,b'\x55')
        vm.put32(OUT,0xfeedbeef);status=vm.run(0x2ddb80,[image_id,field,OUT])
        result={'status':status,'value':vm.get16(OUT) if status==0 else None}
        assert result==image_query(image_id,field,active)
        if status!=0:assert vm.get32(OUT)==0xfeedbeef
        else:assert vm.get32(OUT)>>16==0xfeed
        save('graphics_query_image',{'image_id':image_id,'field':field,'active':active},result)
    # Paint copies from getter 557010 to getter 55700c after flip; equal pointers skip it.
    for same_buffer in [False,True]:
        vm=ArmVM(blob);a=0x1004000;b=a if same_buffer else 0x1005000
        for var,address,label,result in [(0x557014,0x1201000,'width',2),(0x557018,0x1201010,'height',3),
              (0x55700c,0x1201020,'buffer_55700c',a),(0x557010,0x1201030,'buffer_557010',b),
              (0x55701c,0x1201040,'flip',0)]:
            vm.put32(var,address);vm.stub(address,label,result)
        vm.stub(0x29e450,'cache_sync',0)
        vm.u.mem_write(a,b'\xaa'*16)
        if not same_buffer:vm.u.mem_write(b,bytes(range(16)))
        status=vm.run(0x2dd20c)
        assert status==0 and bytes(vm.u.mem_read(a,16))==(b'\xaa'*16 if same_buffer else bytes(range(12))+b'\xaa'*4)
        events=[x[0] for x in vm.calls if x[0] in ('flip','cache_sync')]
        assert events==([] if same_buffer else ['flip','cache_sync'])
        save('graphics_paint',{'same_buffer':same_buffer},{'status':status,'copied_bytes':0 if same_buffer else 12,'events':events})
    # LoadImage stores a borrowed pointer. FreeImage must not free those pixels.
    for pixel_type,width in [(1,3),(1,4),(2,6),(2,8),(3,3)]:
        vm=ArmVM(blob);cache=0x1030000
        vm.put32(vm.im.word(0x2dda40),cache);vm.put32(vm.im.word(0x2ddb70),4)
        vm.stub(0x2ddc40,'find_slot',1)
        vm.stub(0x2995d0,'free',0,1)
        descriptor=struct.pack('<IHHIB3xIHBB',BUF,width,2,pixel_type,0,0x1006000,16,0x40,0x80)
        vm.u.mem_write(OUT,descriptor);vm.u.mem_write(OUT+0x100,b'\xff')
        status=vm.run(0x2dd80c,[OUT,OUT+0x100])
        expected=0x406 if (pixel_type==1 and width%2 or pixel_type==2 and width%4) else 0
        assert status==expected
        row={'status':status,'image_id':None,'borrowed_pixels':False,'free_status':None,'pixel_free_called':False}
        if status==0:
            assert vm.u.mem_read(OUT+0x100,1)==b'\x02'
            assert vm.get32(cache+0x24+0xc)==BUF and vm.u.mem_read(cache+0x24+0x1b,1)==b'\0'
            freed=vm.run(0x2dda44,[2]);assert freed==0
            assert bytes(vm.u.mem_read(cache+0x24,0x24))==bytes(0x24)
            assert not any(x[0]=='free' for x in vm.calls)
            row.update(image_id=2,borrowed_pixels=True,free_status=freed)
        else:assert vm.u.mem_read(OUT+0x100,1)==b'\xff'
        save('graphics_load_free_image',{'pixel_type':pixel_type,'width':width},row)
    # Key core: precollected mask and native kind injected; pause/volume/delay are stubs.
    keycases=[(6,r,v,g) for r,v,g in [(0,15,0),(1,15,0),(0x10,15,0),(0x20,15,0),(0x800,15,0),(0x80000,15,0),
        (0x1001,15,0),(0x201,15,0),(0x200,31,0),(0x100,0,0),(0xa00,15,0),(0x900,15,0),(0x300,15,0),(0xa00,15,1)]]
    keycases.extend((8,r,15,0) for r in [0,1,2,4,8,0x800,0x20,0x10,3,0x30,0x1000])
    for kind,raw,volume,gate in keycases:
        vm=ArmVM(blob);vm.stub(0x2d2e6c,'collected_keys',raw);vm.stub(0x45ffbc,'native_kind',kind)
        vm.put32(0x55bd04,0);vm.put32(0x693c58,volume);vm.u.mem_write(0x55bd50,bytes([gate]))
        for address,name,nargs in [(0x2d6d60,'command',3),(0x3c4c5c,'gui_action',0),(0x2d8b88,'message',3),
            (0x46190c,'pause',1),(0x339be8,'delay',2),(0x3e30cc,'save_volume',1),(0x2c4de8,'set_volume',1)]:vm.stub(address,name,0,nargs)
        vm.put32(OUT,0xabcdef01);vm.put32(OUT+4,0xdeadbeef)
        status=vm.run(0x2d2f7c,[OUT]);assert status==1 and vm.get32(OUT)==0xabcdef01
        events=[x for x in vm.calls if x[0] not in ('collected_keys','native_kind')]
        result={'keys':vm.get32(OUT+4),'volume':vm.get32(0x693c58),'events':events}
        assert result==cooked_keys(kind,raw,volume,gate),(kind,hex(raw),result,cooked_keys(kind,raw,volume,gate))
        save('keys',{'kind':kind,'raw':raw,'volume':volume,'volume_gate':gate},result)
    # Alternate input path asks a device helper only when cooked keys are zero.
    for raw in [0,1]:
        vm=ArmVM(blob);vm.stub(0x2d2e6c,'collected_keys',raw);vm.stub(0x45ffbc,'native_kind',6)
        vm.stub(0x350630,'fallback_device',BUF,1);vm.stub(0x350758,'fallback_keys',0x12345678,1)
        vm.put32(0x55bd04,1);vm.u.mem_write(0x55bd50,b'\x01')
        vm.put32(OUT,0xabcdef01)
        status=vm.run(0x2d2f7c,[OUT]);assert status==1 and vm.get32(OUT)==0xabcdef01
        assert vm.get32(OUT+4)==(1 if raw else 0x12345678)
        fallback=[x for x in vm.calls if x[0].startswith('fallback_')]
        assert fallback==([] if raw else [['fallback_device',[0]],['fallback_keys',[BUF]]])
        save('keys_alternate',{'raw':raw},{'keys':vm.get32(OUT+4),'fallback_called':not bool(raw)})
    for size,result in [(0,0x1004000),(1,0x1004000),(32767,0x1004000),(32768,0x1004000),(65536,0)]:
        vm=ArmVM(blob);vm.stub(0x32e5bc,'pool_backend',result,2)
        actual=vm.run(0x2e3724,[size]);r={'result':actual,'backend_called':bool(vm.calls)}
        assert r==pool_allocate(size,result);save('game_pool_alloc',{'bytes':size,'backend_result':result},r)
    for size,result in [(0,BUF),(1,BUF),(32767,BUF),(32768,BUF),(65536,0)]:
        vm=ArmVM(blob);vm.stub(0x4cdab0,'general_heap_backend',result,2)
        actual=vm.run(0x299588,[size]);r={'result':actual,'backend_called':bool(vm.calls)}
        assert r==general_allocate(size,result);save('general_alloc',{'bytes':size,'backend_result':result},r)
    return {'source_sha256':H['8000_MMI.RAP'],'cases':rows,'case_count':len(rows),
        'limits':['Original ARM wrappers only; explicitly mocked backends and RAM fixtures.',
                  'No device framebuffer, file I/O, actual key input, audio or console execution.',
                  'SP and r4-r11 sentinel preservation checked on these paths with mocked backends only.',
                  'Not an all-path ABI, concurrency or complete HAL certification.']}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=compare(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',result['case_count'],'original ARM/backend-fixture cases agree with host contracts')
