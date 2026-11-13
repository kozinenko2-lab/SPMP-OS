#!/usr/bin/env python3
"""Real FM01+FM03 C RGB565 render, using host OS TTF callback; 320x240 screenshots."""
from __future__ import annotations
import ctypes as C
import json
import tempfile
import os
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw, ImageFont
BASE=Path(__file__).resolve().parents[1]
THEMES=Path(os.environ.get('SPMP_THEMES','/mnt/data/fm01_work/SPMP-OS_Stage10/features/multimedia/theme_v9/themes'))
FONTFILE=os.environ.get('SPMP_HOST_FONT','/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf')
class Palette(C.Structure):
    _fields_=[(n,C.c_uint16) for n in ('background','panel','panel_inactive','line','text','secondary','active_border','highlight','highlight_text','footer')]
class Status(C.Structure):
    _fields_=[('time_hhmm',C.c_char*6),('volume_0_100',C.c_uint),('sd_present',C.c_int)]
FONTFUNC=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,C.c_uint,C.c_uint,C.c_char_p,C.c_uint16,C.c_uint,C.c_uint)
lib=C.CDLL(str(BASE/'build/libfm03_host.so'))
lib.fm03_demo_open.argtypes=[C.c_char_p];lib.fm03_demo_open.restype=C.c_void_p
lib.fm03_demo_close.argtypes=[C.c_void_p]
lib.fm03_demo_paths.argtypes=[C.c_void_p,C.c_char_p,C.c_char_p];lib.fm03_demo_paths.restype=C.c_int
lib.fm03_demo_props.argtypes=[C.c_void_p,C.c_char_p,C.c_char_p];lib.fm03_demo_props.restype=C.c_int
lib.fm03_demo_steps.argtypes=[C.c_void_p,C.c_uint,C.c_uint];lib.fm03_demo_steps.restype=C.c_int
lib.fm03_demo_locale.argtypes=[C.c_void_p,C.c_uint]
lib.fm03_demo_draw.argtypes=[C.c_void_p,C.POINTER(C.c_uint16),C.c_uint,FONTFUNC,C.c_void_p,C.POINTER(Palette),C.POINTER(Status)]
lib.fm01_palette_default.argtypes=[C.POINTER(Palette)]
fonts={}
def drawfont(_ctx,frame,pitch,x,y,text,rgb,max_width,px):
    if not text or x>=320 or y>=240:return
    size=int(px) or 11
    if size not in fonts:fonts[size]=ImageFont.truetype(FONTFILE,size)
    mask=fonts[size].getmask(text.decode('utf8','replace'),mode='L')
    mw,mh=mask.size
    if not mw or not mh:return
    data=bytes(mask)
    for yy in range(min(mh,240-int(y))):
        off=(int(y)+yy)*int(pitch)+int(x)
        mo=yy*mw
        for xx in range(min(mw,int(max_width),320-int(x))):
            if data[mo+xx]>115:frame[off+xx]=rgb
callback=FONTFUNC(drawfont)
def to565(hexstr):
    h=hexstr.lstrip('#');r,g,b=(int(h[i:i+2],16) for i in (0,2,4))
    return ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
def render(ctx,theme,locale=0):
    d=THEMES/theme
    a=np.asarray(Image.open(d/'background.png').convert('RGB').resize((320,240)),dtype='uint16')
    frame=np.ascontiguousarray(((a[:,:,0]>>3)<<11)|((a[:,:,1]>>2)<<5)|(a[:,:,2]>>3),dtype='uint16')
    c=json.loads((d/'theme.json').read_text()).get('colors',{})
    pal=Palette();lib.fm01_palette_default(C.byref(pal))
    pal.highlight=to565(c.get('sel','#00cfff'))
    pal.active_border=to565(c.get('border','#00cfff'))
    pal.panel=pal.panel_inactive=to565(c.get('panel','#111d2a'))
    pal.line=to565(c.get('border','#415168'))
    pal.text=to565(c.get('text','#ffffff'))
    pal.footer=to565('#111621')
    lib.fm03_demo_locale(ctx,locale)
    status=Status(b'17:35',75,1)
    lib.fm03_demo_draw(ctx,frame.ctypes.data_as(C.POINTER(C.c_uint16)),320,callback,None,C.byref(pal),C.byref(status))
    v=frame.astype('uint32')
    out=np.dstack([((v>>11)&31)*255//31,((v>>5)&63)*255//63,(v&31)*255//31]).astype('uint8')
    return Image.fromarray(out,'RGB')

if __name__=='__main__':
    shots=[]
    with tempfile.TemporaryDirectory(prefix='spmp-fm03-preview-') as root:
        path=Path(root);(path/'MUSIC').mkdir();(path/'BACKUP').mkdir()
        (path/'MUSIC'/'Альбомы').mkdir()
        (path/'MUSIC'/'Альбомы'/'Album 01').mkdir()
        (path/'MUSIC'/'Альбомы'/'Album 01'/'long song.flac').write_bytes(b'W'*75524)
        (path/'MUSIC'/'Песня.mp3').write_bytes(b'Q'*42055)
        (path/'MUSIC'/'Readme.txt').write_text('UTF-8')
        for i in range(28):(path/'MUSIC'/'Альбомы'/f'track{i:03}.wav').write_bytes(b'A'*(100+i))
        for title,theme,parent,name,ticks,locale in [
            ('FILE PROPERTIES','beach_glow','MUSIC','Песня.mp3',0,0),
            ('FOLDER TOTALS','circuit_storm','MUSIC','Альбомы',200,0),
            ('FOLDER SCANNING','ocean_curve','MUSIC','Альбомы',1,1),
            ('EMPTY FOLDER','action_heat_90s','','BACKUP',200,0),
        ]:
            ctx=lib.fm03_demo_open(os.fsencode(path));assert ctx
            try:
                assert lib.fm03_demo_paths(ctx,b'MUSIC',b'BACKUP')==0
                assert lib.fm03_demo_props(ctx,parent.encode(),name.encode())==0
                if ticks:lib.fm03_demo_steps(ctx,ticks,1 if ticks==1 else 32)
                im=render(ctx,theme,locale)
                out=BASE/'previews'/(theme+'_properties.png')
                im.save(out);shots.append((title,im))
            finally:lib.fm03_demo_close(ctx)
    collage=Image.new('RGB',(650,530),'#0b1220');d=ImageDraw.Draw(collage)
    font=ImageFont.truetype(FONTFILE,13)
    for i,(title,im) in enumerate(shots):
        x=5+(i%2)*325;y=4+(i//2)*262
        d.text((x+2,y),title,fill='#dfebf9',font=font)
        collage.paste(im,(x,y+19))
    collage.save(BASE/'previews/FM03_four_screens_320x240.png')
    print('FM03: rendered',len(shots),'real RGB565 C screens')
