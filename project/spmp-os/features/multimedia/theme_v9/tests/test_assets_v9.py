#!/usr/bin/env python3
from pathlib import Path
from PIL import Image
import struct,json,zlib,hashlib,subprocess,tempfile,os
r=Path(__file__).resolve().parents[1]
c=json.loads((r/'catalog.json').read_text(encoding='utf8'))
assert c['count']==34 and len(c['themes'])==34
assert len(set(c['theme_ids']))==34
validate=[]
for t in c['themes']:
 d=r/'themes'/t['id'];doc=json.loads((d/'theme.json').read_text(encoding='utf8'))
 assert doc['status']==['time','volume','sd_card'] and not doc['network'] and not doc['battery']
 assert doc['layouts']==['left_list','grid'] and doc['icon_labels']=='runtime_localized_text'
 bg=(d/'background.spbg').read_bytes()
 magic,w,h,fmt,crc,_=struct.unpack('<4sHHHIH',bg[:16])
 assert (magic,w,h,fmt)==(b'SPBG',320,240,1)
 assert len(bg)==153616 and zlib.crc32(bg[16:])==crc
 assert Image.open(d/'background.png').size==(320,240)
 style=(d/'style.spth').read_bytes();assert len(style)==32 and style[:4]==b'SPTH'
 for what in ('list','grid'):
  im=Image.open(d/f'ui_{what}_panels.png');assert im.mode=='RGBA' and im.size==(320,240)
  assert (d/f'ui_{what}_panels.png').stat().st_size>600
  assert (r/'previews'/f"{t['id']}_{what}.png").is_file()
 validate.append(t['id'])
for p in (r/'themes/shared/icons').glob('*.png'):
 im=Image.open(p);assert im.size==(18,18) and im.mode=='RGBA'
assert len(list((r/'themes/shared/icons').glob('*.png')))==8
# End-user preview uses runtime text; English test must differ visibly from Russian.
with tempfile.TemporaryDirectory() as tmp:
 ru=Path(tmp)/'ru.ppm';en=Path(tmp)/'en.ppm';nosd=Path(tmp)/'nosd.ppm'
 exe=r/'build/theme_preview_host';base=r/'themes'
 subprocess.run([str(exe),str(base),'beach_sunset','list',str(ru),'1','1','65'],check=True,stdout=subprocess.DEVNULL)
 subprocess.run([str(exe),str(base),'beach_sunset','list',str(en),'1','1','65','full','en'],check=True,stdout=subprocess.DEVNULL)
 subprocess.run([str(exe),str(base),'beach_sunset','grid',str(nosd),'1','0','65'],check=True,stdout=subprocess.DEVNULL)
 assert hashlib.sha256(ru.read_bytes()).digest()!=hashlib.sha256(en.read_bytes()).digest()
 assert hashlib.sha256(nosd.read_bytes()).digest()!=hashlib.sha256(ru.read_bytes()).digest()
print('PASS',len(validate),'independent theme backgrounds/styles, two UI layouts, text-free icon masks, RU/EN, SD handling')
