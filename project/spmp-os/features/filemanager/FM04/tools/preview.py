#!/usr/bin/env python3
"""QVGA previews. Original themed backgrounds + actual C-rendered keyboard; external system font."""
import ctypes as C
import json
import os
from pathlib import Path
import numpy as np
from PIL import Image, ImageFont
B=Path(__file__).resolve().parents[1]
LIB=C.CDLL(str(B/'build/libfm04_host.so'))
THEMES=Path(os.environ.get('SPMP_THEMES','/mnt/data/fm01_install_test/features/multimedia/theme_v9/themes'))
FONTFILE=os.environ.get('SPMP_HOST_FONT','/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf')
class P(C.Structure):
    _fields_=[(k,C.c_uint16) for k in ('background','panel','panel_inactive','line','text','secondary','active_border','highlight','highlight_text','footer')]
FUNC=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,C.c_uint,C.c_uint,C.c_char_p,C.c_uint16,C.c_uint,C.c_uint)
LIB.fm04_demo_create.argtypes=[C.c_char_p,C.c_uint];LIB.fm04_demo_create.restype=C.c_void_p
LIB.fm04_demo_destroy.argtypes=[C.c_void_p]
LIB.fm04_demo_layout.argtypes=[C.c_void_p,C.c_uint,C.c_uint]
LIB.fm04_demo_draw.argtypes=[C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,FUNC,C.c_void_p,C.POINTER(P)]
fonts={}
def draw(_,fb,pitch,x,y,txt,rgb,max_w,px):
    if not txt or not max_w or x>=320 or y>=240:return
    size=int(px) or 11
    if size not in fonts:fonts[size]=ImageFont.truetype(FONTFILE,size)
    mask=fonts[size].getmask(txt.decode('utf-8','replace'),mode='L')
    ww,hh=mask.size;raw=bytes(mask)
    for iy in range(min(hh,240-int(y))):
        for ix in range(min(ww,int(max_w),320-int(x))):
            if raw[iy*ww+ix]>105:fb[(int(y)+iy)*pitch+int(x)+ix]=rgb
FONT=FUNC(draw)
def pack(rgb):
    a=np.asarray(rgb,dtype=np.uint16)
    return np.ascontiguousarray(((a[:,:,0]>>3)<<11)|((a[:,:,1]>>2)<<5)|(a[:,:,2]>>3),dtype=np.uint16)
def unpack(a):
    x=a.astype('uint32');rgb=np.stack([((x>>11)&31)*255//31,((x>>5)&63)*255//63,(x&31)*255//31],axis=-1)
    return Image.fromarray(rgb.astype('uint8'))
def c565(s):
    s=s.lstrip('#');r,g,b=(int(s[i:i+2],16) for i in (0,2,4));return ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
def sample(slug,value,locale,layout,focus):
    d=THEMES/slug
    art=Image.open(d/'background.png').convert('RGB').resize((320,240),Image.Resampling.LANCZOS)
    canvas=pack(art)
    data=json.loads((d/'theme.json').read_text())
    colors=data.get('colors',{})
    p=P()
    p.background=c565('#101019');p.panel=c565(colors.get('panel','#0a1728'))
    p.panel_inactive=c565('#17202e');p.line=c565(colors.get('border','#5b6f7f'))
    p.text=c565(colors.get('text','#ffffff'));p.secondary=c565('#c9d6ed')
    p.active_border=c565(colors.get('sel','#00cfff'))
    p.highlight=c565(colors.get('sel','#00cfff'));p.highlight_text=0xffff;p.footer=c565('#090f19')
    handle=LIB.fm04_demo_create(value.encode('utf8'),locale)
    assert handle
    try:
        LIB.fm04_demo_layout(handle,layout,focus)
        LIB.fm04_demo_draw(handle,canvas.ctypes.data_as(C.POINTER(C.c_uint16)),320,FONT,None,C.byref(p))
    finally:LIB.fm04_demo_destroy(handle)
    out=unpack(canvas);out.save(B/'previews'/f'FM04_{slug}.png');return out
if __name__=='__main__':
    examples=[('beach_glow','Журнал 11.11.26.log',0,1,1),('circuit_storm','Race_2026.txt',1,0,10),('velvet_pulse','Новый файл.txt',0,2,38),('ocean_curve','Документ.txt',0,1,43)]
    pics=[sample(*it) for it in examples]
    board=Image.new('RGB',(640,480))
    for i,im in enumerate(pics):board.paste(im,((i%2)*320,(i//2)*240))
    board.save(B/'previews/FM04_four_screens_320x240.png')
    print('FM04 previews: 4 screens from actual C QVGA renderer + OS font callback')
