#!/usr/bin/env python3
"""Generate ACTUAL 320×240 FM01+FM02 RGB565 C-renderer screenshots, not mockups."""
from __future__ import annotations
import ctypes as C
import json
import os
from pathlib import Path
import sys
import tempfile
import numpy as np
from PIL import Image, ImageDraw, ImageFont
BASE=Path(__file__).resolve().parents[1]
THEMES=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/mnt/data/fm01_work/SPMP-OS_Stage10/features/multimedia/theme_v9/themes')
FONTFILE=os.environ.get('SPMP_HOST_FONT','/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf')
class Palette(C.Structure):
    _fields_=[(n,C.c_uint16) for n in ('background','panel','panel_inactive','line','text','secondary','active_border','highlight','highlight_text','footer')]
class Status(C.Structure):
    _fields_=[('time_hhmm',C.c_char*6),('volume_0_100',C.c_uint),('sd_present',C.c_int)]
FONTFUNC=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,C.c_uint,C.c_uint,C.c_char_p,C.c_uint16,C.c_uint,C.c_uint)
lib=C.CDLL(str(BASE/'build/libfm02_host.so'))
lib.fm02_demo_open.argtypes=[C.c_char_p];lib.fm02_demo_open.restype=C.c_void_p
lib.fm02_demo_close.argtypes=[C.c_void_p]
lib.fm02_demo_paths.argtypes=[C.c_void_p,C.c_char_p,C.c_char_p]
lib.fm02_demo_paths.restype=C.c_int
lib.fm02_demo_keys.argtypes=[C.c_void_p,C.c_uint]
lib.fm02_demo_action.argtypes=[C.c_void_p,C.c_uint,C.c_char_p]
lib.fm02_demo_action.restype=C.c_int
lib.fm02_demo_draw.argtypes=[C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,FONTFUNC,C.c_void_p,C.POINTER(Palette),C.POINTER(Status)]
lib.fm02_demo_state.argtypes=[C.c_void_p];lib.fm02_demo_state.restype=C.c_uint
lib.fm02_demo_marks.argtypes=[C.c_void_p];lib.fm02_demo_marks.restype=C.c_uint
lib.fm01_palette_default.argtypes=[C.POINTER(Palette)]
fonts={}
def font_draw(_ctx,fb,pitch,x,y,txt,rgb,max_width,pixels):
    if not txt:return
    size=int(pixels) or 11
    if size not in fonts:fonts[size]=ImageFont.truetype(FONTFILE,size)
    mask=fonts[size].getmask(txt.decode('utf8','replace'),mode='L');mw,mh=mask.size
    raw=bytes(mask)
    for yy in range(min(mh,240-int(y))):
        for xx in range(min(mw,int(max_width),320-int(x))):
            if raw[yy*mw+xx]>115:fb[(int(y)+yy)*pitch+int(x)+xx]=rgb
callback=FONTFUNC(font_draw)
def r565(col):
    h=col.lstrip('#');r,g,b=[int(h[i:i+2],16) for i in (0,2,4)]
    return ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
def render(ctx,slug):
    d=THEMES/slug
    bg=np.asarray(Image.open(d/'background.png').convert('RGB').resize((320,240)),dtype=np.uint16)
    frame=np.ascontiguousarray(((bg[:,:,0]>>3)<<11)|((bg[:,:,1]>>2)<<5)|(bg[:,:,2]>>3),dtype=np.uint16)
    style=json.loads((d/'theme.json').read_text());co=style.get('colors',{})
    p=Palette();lib.fm01_palette_default(C.byref(p))
    p.highlight=r565(co.get('sel','#00cfff'));p.active_border=r565(co.get('border','#00cfff'))
    p.panel=p.panel_inactive=r565(co.get('panel','#131d2a'))
    p.line=r565(co.get('border','#414141'));p.text=r565(co.get('text','#ffffff'))
    p.highlight_text=0xffff;p.footer=r565('#101724')
    st=Status(b'17:35',75,1)
    lib.fm02_demo_draw(ctx,frame.ctypes.data_as(C.POINTER(C.c_uint16)),320,callback,None,C.byref(p),C.byref(st))
    packed=frame.astype('uint32')
    rgb=np.dstack([((packed>>11)&31)*255//31,((packed>>5)&63)*255//63,(packed&31)*255//31]).astype('uint8')
    return Image.fromarray(rgb,'RGB')
if __name__=='__main__':
    out=BASE/'previews';out.mkdir(exist_ok=True)
    shots=[]
    with tempfile.TemporaryDirectory(prefix='spmp-fm02-ui-') as t:
        root=Path(t);(root/'MUSIC').mkdir();(root/'BACKUP').mkdir()
        (root/'MUSIC'/'Songs').mkdir();(root/'MUSIC'/'Живая музыка.flac').write_bytes(b'0'*26144)
        (root/'MUSIC'/'music.mp3').write_bytes(b'1'*52500)
        (root/'MUSIC'/'Заметки.txt').write_bytes(b'Hello UTF-8')
        (root/'MUSIC'/'My playlist.m3u').write_bytes(b'#EXTM3U\n')
        (root/'BACKUP'/'README.log').write_text('log')
        for name,slug,state in [
            ('FM02_beach_marks','beach_glow','marks'),
            ('FM02_circuit_actions','circuit_storm','actions'),
            ('FM02_ocean_delete','ocean_curve','delete'),
            ('FM02_action_copy_done','action_heat_90s','done'),
            ('FM02_beach_actions','beach_glow','actions'),
            ('FM02_circuit_marks','circuit_storm','marks'),
        ]:
            ctx=lib.fm02_demo_open(os.fsencode(root));assert ctx
            try:
                assert lib.fm02_demo_paths(ctx,b'MUSIC',b'BACKUP')==0
                if state=='marks':
                    lib.fm02_demo_keys(ctx,1<<7)
                    lib.fm02_demo_keys(ctx,1<<1)
                    lib.fm02_demo_keys(ctx,1<<7)
                    assert lib.fm02_demo_marks(ctx)==2
                elif state=='actions':
                    lib.fm02_demo_keys(ctx,1<<6);assert lib.fm02_demo_state(ctx)==1
                elif state=='delete':
                    assert lib.fm02_demo_action(ctx,4,None)==0 and lib.fm02_demo_state(ctx)==3
                elif state=='done':
                    assert lib.fm02_demo_action(ctx,1,None)==0 and lib.fm02_demo_state(ctx)==5
                image=render(ctx,slug);image.save(out/(name+'.png'))
                shots.append((name.replace('FM02_','').replace('_',' ').upper(),image))
            finally:lib.fm02_demo_close(ctx)
    w=2*320+30;h=3*265+12
    sheet=Image.new('RGB',(w,h),'#121821');draw=ImageDraw.Draw(sheet)
    capfont=ImageFont.truetype(FONTFILE,13)
    for i,(caption,image) in enumerate(shots):
        x=10+(i%2)*330;y=8+(i//2)*265
        sheet.paste(image,(x,y+20));draw.text((x+3,y),caption,font=capfont,fill='#dbeafe')
    sheet.save(out/'FM02_6screens_320x240.png')
    print('Rendered',len(shots),'real RGB565 C screens at 320x240')
