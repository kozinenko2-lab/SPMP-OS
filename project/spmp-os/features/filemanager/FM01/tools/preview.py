#!/usr/bin/env python3
"""FM01 preview using the REAL C RGB565 overlay and a font supplied by host OS.
The device build receives the font callback from SPMP-OS multilingual font HAL.
"""
from __future__ import annotations
import ctypes as C
import json
import os
from pathlib import Path
import sys
import tempfile
from PIL import Image, ImageFont, ImageDraw
import numpy as np

BASE=Path(__file__).resolve().parents[1]
LIB=BASE/'build/libfm01_host.so'
THEMES=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/mnt/data/fm01_work/SPMP-OS_Stage10/features/multimedia/theme_v9/themes')
FONTFILE=os.environ.get('SPMP_HOST_FONT','/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf')
class Palette(C.Structure):
    _fields_=[(n,C.c_uint16) for n in ('background','panel','panel_inactive','line','text','secondary','active_border','highlight','highlight_text','footer')]
class Status(C.Structure):
    _fields_=[('time_hhmm',C.c_char*6),('volume_0_100',C.c_uint),('sd_present',C.c_int)]
FONTFUNC=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,C.c_uint,C.c_uint,C.c_char_p,C.c_uint16,C.c_uint,C.c_uint)
lib=C.CDLL(str(LIB))
lib.fm01_demo_open.argtypes=[C.c_char_p];lib.fm01_demo_open.restype=C.c_void_p
lib.fm01_demo_close.argtypes=[C.c_void_p];lib.fm01_demo_set_paths.argtypes=[C.c_void_p,C.c_char_p,C.c_char_p]
lib.fm01_demo_set_paths.restype=C.c_int
lib.fm01_demo_locale.argtypes=[C.c_void_p,C.c_uint]
lib.fm01_demo_keys.argtypes=[C.c_void_p,C.c_uint]
lib.fm01_demo_overlay.argtypes=[C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,FONTFUNC,C.c_void_p,C.POINTER(Palette),C.POINTER(Status)]
lib.fm01_palette_default.argtypes=[C.POINTER(Palette)]

fonts={}
def font_draw(_ctx, fb,pitch,x,y,text,rgb,max_width,pixels):
    if not text or max_width==0:return
    size=int(pixels) or 11
    if size not in fonts:fonts[size]=ImageFont.truetype(FONTFILE,size)
    font=fonts[size]
    content=text.decode('utf8','replace')
    bbox=font.getbbox(content)
    mask=font.getmask(content,mode='L');mw,mh=mask.size
    if mw<1 or mh<1:return
    data=bytes(mask)
    # origin from image font drawing (top-left glyph mask), clipped to the C region.
    for yy in range(min(mh,240-int(y))):
        for xx in range(min(mw,int(max_width),320-int(x))):
            if data[yy*mw+xx]>115:fb[(int(y)+yy)*pitch+int(x)+xx]=rgb
callback=FONTFUNC(font_draw)

def rgb565(hexv):
    h=hexv.lstrip('#');r,g,b=(int(h[i:i+2],16) for i in (0,2,4))
    return ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
def render_theme(handle, slug, sd=True, locale=0):
    d=THEMES/slug
    bg=np.asarray(Image.open(d/'background.png').convert('RGB').resize((320,240)),dtype=np.uint16)
    packed=((bg[:,:,0]>>3)<<11)|((bg[:,:,1]>>2)<<5)|(bg[:,:,2]>>3)
    frame=np.ascontiguousarray(packed,dtype=np.uint16)
    data=json.loads((d/'theme.json').read_text())
    cl=data.get('colors',{})
    pal=Palette();lib.fm01_palette_default(C.byref(pal))
    pal.highlight=rgb565(cl.get('sel','#00cfff'))
    pal.active_border=rgb565(cl.get('border','#00cfff'))
    pal.panel=rgb565(cl.get('panel','#131d2a'))
    pal.line=rgb565(cl.get('border','#414141'))
    pal.panel_inactive=rgb565(cl.get('panel','#131d2a'))
    pal.text=rgb565(cl.get('text','#ffffff'))
    pal.highlight_text=0xffff
    pal.background=rgb565('#0b1321')
    pal.footer=rgb565('#080e18')
    st=Status(b'11:42',72,int(sd))
    lib.fm01_demo_locale(handle,locale)
    lib.fm01_demo_overlay(handle,frame.ctypes.data_as(C.POINTER(C.c_uint16)),320,callback,None,C.byref(pal),C.byref(st))
    # 565 -> RGB888, identical to target framebuffer content
    pix=frame.astype('uint32');R=((pix>>11)&31)*255//31;G=((pix>>5)&63)*255//63;B=(pix&31)*255//31
    return Image.fromarray(np.dstack([R,G,B]).astype('uint8'),'RGB')

if __name__=='__main__':
    with tempfile.TemporaryDirectory(prefix='spmp-fm01-') as root:
        for folder in ['MUSIC','GAMES','PHOTOS','DOCUMENTS','SAVES','LOGS']:
            (Path(root)/folder).mkdir()
        for name in ['Amiga Forever.mp3','Живая музыка.flac','Night Drive.mp3','README.txt','Wave.wav','Зимний вечер.ogg','Jazz.WMA']:
            (Path(root)/'MUSIC'/name).write_bytes(b'\0'*int(len(name)*330))
        for i in range(14):
            (Path(root)/'PHOTOS'/('IMG_%04d.JPG'%i)).write_bytes(b'\0'*(1024+i*130))
        ctx=lib.fm01_demo_open(os.fsencode(root))
        if not ctx:raise RuntimeError('POSIX adapter open failed')
        try:
            assert lib.fm01_demo_set_paths(ctx,b'MUSIC',b'PHOTOS')==0
            # independent pane movement
            lib.fm01_demo_keys(ctx,1<<1)
            lib.fm01_demo_keys(ctx,1<<1)
            lib.fm01_demo_keys(ctx,1<<3)
            lib.fm01_demo_keys(ctx,1<<1)
            lib.fm01_demo_keys(ctx,1<<2)
            slugs=['beach_glow','circuit_storm','ocean_curve','action_heat_90s']
            outputs=[]
            for i,slug in enumerate(slugs):
                out=render_theme(ctx,slug,sd=True,locale=i%2)
                f=BASE/'previews'/('FM01_%s.png'%slug);out.save(f)
                outputs.append(out)
            # Target-native single image, and contact sheet preview.
            sheet=Image.new('RGB',(640,480))
            for i,im in enumerate(outputs):sheet.paste(im,((i%2)*320,(i//2)*240))
            sheet.save(BASE/'previews/FM01_four_themes.png')
            render_theme(ctx,'alpine_coast',sd=False).save(BASE/'previews/FM01_SD_removed.png')
            print('FM01 previews: 4 themed 320x240 views, contact sheet, SD-absent fallback')
        finally:
            lib.fm01_demo_close(ctx)
