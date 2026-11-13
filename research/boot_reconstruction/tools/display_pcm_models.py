"""Exact-build host predicates for observed LCD and PCM paths.
Register values describe synthetic fixtures, not supported hardware programming.
"""
from native_abi_models import U32,signed

def lcd_swap(render,scanout,update):
    writes=[]
    if render!=scanout:
        writes.append([0x93000054,render])
        if update:writes.append([0x93000078,1])
        render,scanout=scanout,render
    return {'render':render,'scanout':scanout,'writes':writes}

def dma_program(channel,first,first_bytes,second,second_bytes,control):
    offset=channel*4
    writes=[[0x93010008+offset,first],[0x93010018+offset,(first+first_bytes-4)&U32],
        [0x9301004c+offset,second],[0x9301005c+offset,(second+second_bytes-4)&U32],
        [0x93010028+offset,0x93012004],[0x9301006c+offset,0xfe if control&0x100000 else 0xde]]
    cleared=control&~0x2000
    writes.extend([[0x93012000,cleared],[0x93012000,cleared|1]])
    return writes

def reserve_try(request,read,write,capacity,threshold,base=0x588b60):
    free=read-write
    if free<0:free=capacity-write+read
    if free<threshold:return 0
    request=signed(request)
    if request==0:return 0x588b60+write if free>8192 else 0
    if request>0:return base+write if free>request else 0
    return U32

def fill_rect_status(x,y,width,height):
    def s16(value):value&=0xffff;return value-65536 if value&32768 else value
    return 0 if (width&65535)<1024 and (height&65535)<1024 and -1024<=s16(x)<1024 and -1024<=s16(y)<1024 else 0x70000401
