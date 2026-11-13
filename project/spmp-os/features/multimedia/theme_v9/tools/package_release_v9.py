#!/usr/bin/env python3
"""Package checked source patch and 9-asset editor pack; no Flash/NAND modifications."""
from pathlib import Path
from zipfile import ZipFile,ZIP_DEFLATED
import json,hashlib,sys,collections
ROOT=Path(__file__).resolve().parents[1]
OUT=Path('/mnt/data')
sys.path.insert(0,str(ROOT/'tools'))
from theme_patch import entries
patch_files=entries()
new_ids=['action_heat_90s','neon_pursuit_90s','urban_strike_90s','beach_glow','velvet_pulse','runway_shine','circuit_storm','ocean_curve','gallery_velocity']
full=OUT/'SPMP-OS_Themes_v9_Full_34_Patch.zip'
with ZipFile(full,'w',ZIP_DEFLATED,compresslevel=7) as z:
 for rel,path in sorted(patch_files.items()):z.write(path,arcname='SPMP-OS_Themes_v9/'+rel)
 for name in ('NINE_NEW_THEMES_v9.png',):z.write(ROOT/'previews'/name,arcname='SPMP-OS_Themes_v9/previews/'+name)
assets=OUT/'SPMP-OS_Themes_v9_New_9_Assets.zip'
with ZipFile(assets,'w',ZIP_DEFLATED,compresslevel=7) as z:
 for tid in new_ids:
  for p in sorted((ROOT/'themes'/tid).rglob('*')):
   if p.is_file():z.write(p,arcname='SPMP-OS_Themes_v9_New_9_Assets/themes/'+str(p.relative_to(ROOT/'themes')))
 for p in (ROOT/'catalog.json',ROOT/'locales/en.json',ROOT/'locales/ru.json',ROOT/'previews/NINE_NEW_THEMES_v9.png'):
  z.write(p,arcname='SPMP-OS_Themes_v9_New_9_Assets/'+str(p.relative_to(ROOT)))
 z.writestr('SPMP-OS_Themes_v9_New_9_Assets/README.txt',
 '''ONLY EDITABLE ASSETS FOR 9 NEW THEMES. To use them in SPMP-OS, install the FULL v9 patch containing the renderer and catalog of 34 themes. Do not just copy these folders into the old v8 directory: v8 catalog knows 25 theme IDs.\n''')
for a in (full,assets):
 with ZipFile(a) as z:assert z.testzip() is None
print('Full patch',full,full.stat().st_size,'bytes',len(patch_files),'files')
print('Nine new assets',assets,assets.stat().st_size,'bytes')
report={'release':'SPMP-OS Themes v9','previous_themes':25,'new_themes':9,'total_themes':34,'preview_modes':2,'rendered_screens':68,'new_ids':new_ids,
 'categories':dict(collections.Counter(t['category'] for t in json.loads((ROOT/'catalog.json').read_text(encoding='utf8'))['themes'])),
 'patch_file_count':len(patch_files),
 'patch_sha256':hashlib.sha256(full.read_bytes()).hexdigest(),
 'new_assets_sha256':hashlib.sha256(assets.read_bytes()).hexdigest()}
(OUT/'SPMP-OS_Themes_v9_Release_Report.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
print(json.dumps(report,ensure_ascii=False,indent=2))
