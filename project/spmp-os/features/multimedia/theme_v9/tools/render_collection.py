#!/usr/bin/env python3
"""Use the actual C renderer for all previews; compile via tools/build.sh first."""
from PIL import Image,ImageDraw,ImageFont
from pathlib import Path
import json,subprocess
r=Path(__file__).resolve().parents[1];out=r/'previews';out.mkdir(exist_ok=True)
items=json.loads((r/'catalog.json').read_text(encoding='utf8'))['themes']
ex=r/'build'/'theme_preview_host'
if not ex.exists():raise SystemExit('Run tools/build.sh first')
contact=Image.new('RGB',(320*5,270*((len(items)+4)//5)),(9,15,25));draw=ImageDraw.Draw(contact)
for j,t in enumerate(items):
 id=t['id']
 for mode in ['list','grid']:
  pn=out/f'{id}_{mode}.png';ppm=out/f'{id}_{mode}.ppm'
  subprocess.run([str(ex),str(r/'themes'),id,mode,str(ppm),'1','1','65'],check=True,stdout=subprocess.DEVNULL)
  Image.open(ppm).convert('RGB').save(pn,optimize=True);ppm.unlink()
 x=(j%5)*320;y=(j//5)*270
 im=Image.open(out/(id+('_list.png' if j%2==0 else '_grid.png'))).convert('RGB');contact.paste(im,(x,y))
 draw.text((x+6,y+244),f'{j+1:02}  {id}',fill=(240,244,249))
contact.save(out/'ALL_THEMES_320x240.png',optimize=True)
print('Rendered',len(items),'real C-rendered themes, 2 modes; preview montage saved')
