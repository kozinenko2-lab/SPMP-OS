#!/usr/bin/env python3
"""320x240 screens from FM05 C renderer, theme colors and real OS font callback."""
from pathlib import Path
import ctypes as C
import json
import numpy as np
from PIL import Image,ImageFont
B=Path(__file__).resolve().parents[1]
LIB=C.CDLL(str(B/'build/libfm05_host.so'))
THEMES=Path('/mnt/data/fm01_install_test/features/multimedia/theme_v9/themes')
FONT='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf'
class P(C.Structure):
    _fields_=[(a,C.c_uint16) for a in ('background','panel','panel_inactive','line','text','secondary','active_border','highlight','highlight_text','footer')]
FUNC=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,C.c_uint,C.c_uint,C.c_char_p,C.c_uint16,C.c_uint,C.c_uint)
LIB.fm05_demo_create.argtypes=[C.c_char_p,C.c_char_p,C.c_uint,C.c_uint];LIB.fm05_demo_create.restype=C.c_void_p
LIB.fm05_demo_destroy.argtypes=[C.c_void_p]
LIB.fm05_demo_draw.argtypes=[C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,FUNC,C.c_void_p,C.POINTER(P),C.c_uint]
fonts={}
def text_draw(ctx,fb,pitch,x,y,value,color,max_width,px):
    if not value or x>=320 or y>=240 or not max_width:return
    size=int(px) or 11
    if size not in fonts:fonts[size]=ImageFont.truetype(FONT,size)
    bitmap=fonts[size].getmask(value.decode('utf8','replace'),mode='L');w,h=bitmap.size
    raw=bytes(bitmap)
    for iy in range(min(h,240-int(y))):
        for ix in range(min(w,int(max_width),320-int(x))):
            if raw[iy*w+ix]>105:fb[(y+iy)*pitch+x+ix]=color
CALLBACK=FUNC(text_draw)
def rgb565(col):
    col=col.lstrip('#');r,g,b=[int(col[i:i+2],16) for i in (0,2,4)]
    return (r>>3)<<11|(g>>2)<<5|(b>>3)
def pal(theme):
    cfg=json.loads((THEMES/theme/'theme.json').read_text());co=cfg.get('colors',{})
    p=P();p.background=rgb565('#07101a');p.panel=rgb565(co.get('panel','#0b1729'))
    p.panel_inactive=rgb565('#141c29');p.line=rgb565(co.get('border','#42536e'))
    p.text=rgb565(co.get('text','#ffffff'));p.secondary=rgb565('#c5d4dd');p.active_border=rgb565(co.get('sel','#00cfff'))
    p.highlight=rgb565('#243e58');p.highlight_text=rgb565('#ffffff');p.footer=rgb565('#0b1020')
    return p
sample='''# Настройки SPMP-OS
[display]
resolution=320x240
theme=beach_glow
language=ru

[audio]
volume=75
sample_rate=44100

# Тест UTF-8
Проверка сохранения ✓
Последняя строка
'''
def one(theme,path,locale,state,keyboard=False):
    canvas=np.zeros((240,320),dtype=np.uint16)
    h=LIB.fm05_demo_create(path.encode(),sample.encode(),locale,state)
    assert h
    try:
        p=pal(theme)
        LIB.fm05_demo_draw(h,canvas.ctypes.data_as(C.POINTER(C.c_uint16)),320,CALLBACK,None,C.byref(p),1 if keyboard else 0)
    finally:LIB.fm05_demo_destroy(h)
    v=canvas.astype('uint32')
    rgb=np.stack([((v>>11)&31)*255//31,((v>>5)&63)*255//63,(v&31)*255//31],axis=-1).astype('uint8')
    im=Image.fromarray(rgb,'RGB');im.save(B/'previews'/f'FM05_{theme}_{state}.png')
    return im
if __name__=='__main__':
    ex=[('beach_glow','settings.cfg',0,0,False),('circuit_storm','Race_2026.log',1,1,False),('velvet_pulse','Журнал.txt',0,2,False),('ocean_curve','notes.ini',0,3,True)]
    imgs=[one(*e) for e in ex]
    board=Image.new('RGB',(640,480))
    for i,im in enumerate(imgs):board.paste(im,((i%2)*320,(i//2)*240))
    board.save(B/'previews/FM05_four_screens_320x240.png')
    print('FM05 four 320x240 C-rendered screens generated')
