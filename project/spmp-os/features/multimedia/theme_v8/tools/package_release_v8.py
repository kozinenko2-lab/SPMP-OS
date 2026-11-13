#!/usr/bin/env python3
"""Release full additive patch + asset-only zip. No full-concept bitmap posters bundled."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import hashlib,sys,json,collections
R=Path(__file__).resolve().parents[1]
OUT=Path('/mnt/data')
sys.path.insert(0,str(R/'tools'))
from theme_patch import entries
files=entries()
# Bundle the overall collection of panels, bitmap art, icon masks, source, ARMv5 libs and API.
full=OUT/'SPMP-OS_Themes_v8_Full_Patch.zip'
with ZipFile(full,'w',ZIP_DEFLATED,compresslevel=7) as z:
 for rel,p in sorted(files.items()):z.write(p,arcname=f'SPMP-OS_Themes_v8/{rel}')
 # Previews are not installed via additive patch, but useful in archive
 for name in ('SIX_NEW_THEMES_LIST_GRID.png',):
  z.write(R/'previews'/name,arcname='SPMP-OS_Themes_v8/previews/'+name)
assets=OUT/'SPMP-OS_Themes_v8_Assets_320x240.zip'
with ZipFile(assets,'w',ZIP_DEFLATED,compresslevel=7) as z:
 for d in (R/'themes').rglob('*'):
  if d.is_file():z.write(d,arcname='SPMP-OS_Themes_v8_Assets/themes/'+str(d.relative_to(R/'themes')))
 for p in [R/'catalog.json',R/'locales/ru.json',R/'locales/en.json',R/'config/selected_theme.sptc']:
  z.write(p,arcname='SPMP-OS_Themes_v8_Assets/'+str(p.relative_to(R)))
 # Only preview overview, not host rendered 50 images
 z.write(R/'previews'/'ALL_THEMES_320x240.png',arcname='SPMP-OS_Themes_v8_Assets/PREVIEW_ALL.png')
report={"release":"SPMP-OS Theme Collection v8","count":25,"categories":dict(collections.Counter(x['category'] for x in json.loads((R/'catalog.json').read_text())['themes'])),"files_additive":len(files),
 "full_patch_sha256":hashlib.sha256(full.read_bytes()).hexdigest(),"assets_only_sha256":hashlib.sha256(assets.read_bytes()).hexdigest()}
for a in (full,assets):
 with ZipFile(a) as z:
  assert z.testzip() is None
 print(a,round(a.stat().st_size/1048576,2),'MiB')
(OUT/'SPMP-OS_Themes_v8_Release_Report.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
print(report)
