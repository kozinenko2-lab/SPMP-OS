"""Host reconstructions of selected image-specific NativeGE predicates.
Not target bindings. Backends are values/call traces, never actual device callbacks.
"""
U32=0xffffffff

def signed(value):
    value &= U32
    return value-0x100000000 if value&0x80000000 else value

def file_result(operation,fd,count,backend_result):
    fd &= U32;count &= U32;backend_result=signed(backend_result)
    if operation=='read' and fd==0:return {'status':2,'count':None,'backend_called':False}
    if operation in ('write','close') and fd>32:return {'status':2,'count':None,'backend_called':False}
    if operation in ('read','write'):
        return {'status':0 if (backend_result&U32)==count else 2,'count':backend_result,'backend_called':True}
    if operation in ('close','seek'):
        return {'status':0 if backend_result>=0 else 2,'count':None,'backend_called':True}
    raise ValueError(operation)

def open_result(fd,stat_result):
    if signed(fd)<0:return {'status':2,'fd':None,'stat_called':False}
    return {'status':0 if signed(stat_result)>=0 else 2,
            'fd':fd if signed(stat_result)>=0 else None,'stat_called':True}

def rgb565(rgb):
    r,g,b=rgb
    return ((r&0xf8)<<8)+((g&0xfc)<<3)+(b>>3)

def rgb888_truncated(value):
    return [(value>>8)&0xf8,(value&0x7e0)>>3,(value&0x1f)<<3]

def image_query(image_id,field,active=True,*,count=4,width=320,height=240,pixel_format=3,shared=0x55):
    image_id &= 255;field &= 255
    if image_id<2 or image_id>=count or not active:return {'status':0x406,'value':None}
    values={1:width,2:height,3:pixel_format&0xffff,4:shared}
    if field not in values:return {'status':0x406,'value':None}
    return {'status':0,'value':values[field]}

def framebuffer_request(width,height):
    return 0 if (width&0xffff)<1024 and (height&0xffff)<1024 else 0x70000401

def pool_allocate(size,backend_result):
    size &= U32
    if backend_result&3:raise ValueError('Unaligned backend assertion path is not modeled')
    return {'result':backend_result if size>=32768 else 0,'backend_called':size>=32768}

def general_allocate(size,backend_result):
    if backend_result&3:raise ValueError('Unaligned backend assertion path is not modeled')
    return {'result':backend_result if size&U32 else 0,'backend_called':bool(size&U32)}

def cooked_keys(kind,raw,volume=15,volume_gate=0):
    if not 0<=volume<=31:raise ValueError('Model requires a valid OEM volume in 0..31')
    raw &= U32;keys=0;events=[]
    if kind==8:
        for mask,code in [(1,0x1c00),(2,0x1e00),(4,0x200),(8,0x400),(0x800,0x1a00),(0x20,0x8000),(0x10,0x4000)]:
            if raw&mask:return {'keys':code,'volume':volume,'events':[]}
        return {'keys':0,'volume':volume,'events':[]}
    if kind!=6:raise ValueError('Only native kinds 6 and 8 are modeled')
    keys=raw&15
    if raw&0x10:keys |= 0x10000
    if raw&0x20:keys |= 0x20000
    if raw&0x80000:keys |= 0x80000
    if raw&0x1000:
        return {'keys':0,'volume':volume,'events':[
            ['command',[0xd07,0,0]],['gui_action',[]],['message',[0x569,0,0]],
            ['pause',[0]],['delay',[30,0]]]}
    if not volume_gate:
        if raw&0x200:
            volume=min(31,volume+1);events=[['save_volume',[volume]],['set_volume',[volume]]];raw=0
        if raw&0x100:
            volume=max(0,volume-1);events=[['save_volume',[volume]],['set_volume',[volume]]];raw=0
    if raw&0x800:keys |= 0x2000
    return {'keys':keys,'volume':volume,'events':events}
