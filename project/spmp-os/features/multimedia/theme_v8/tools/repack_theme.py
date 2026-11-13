#!/usr/bin/env python3
"""Edit background.png and colors in theme.json, then rebuild device-ready SPBG+SPTH.
All work on development PC. This script never writes firmware or NAND.
"""
from pathlib import Path
from PIL import Image
import argparse,struct,zlib,json
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('id');args=p.parse_args();d=ROOT/'themes'/args.id
if not d.is_dir() or args.id not in json.loads((ROOT/'catalog.json').read_text())['theme_ids']:p.error('unknown theme')
doc=json.loads((d/'theme.json').read_text(encoding='utf8'))
im=Image.open(d/'background.png').convert('RGB')
if im.size!=(320,240):p.error('background.png must be exactly 320x240')
pixels=bytearray()
for r,g,b in im.getdata():pixels+=struct.pack('<H',((r>>3)<<11)|((g>>2)<<5)|(b>>3))
(d/'background.spbg').write_bytes(struct.pack('<4sHHHIH',b'SPBG',320,240,1,zlib.crc32(pixels),0)+pixels)
def rgb(hexcode):
 h=hexcode.lstrip('#');assert len(h)==6; r,g,b=bytes.fromhex(h)
 return ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
c=[rgb(doc['colors'][x]) for x in ('accent','panel','text','muted','sel','border')]
(d/'style.spth').write_bytes(struct.pack('<4sHHBB6H10s',b'SPTH',1,32,int(doc['layout_default']=='grid'),1,*c,b'\0'*10))
print('Repacked',args.id,'SPBG 153616 bytes, SPTH 32 bytes')
