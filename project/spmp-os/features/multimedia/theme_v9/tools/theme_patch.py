#!/usr/bin/env python3
"""Transactional additive-only theme patch. No game/core/boot/flash files modified."""
from pathlib import Path
import argparse,hashlib,json,os,shutil,sys,tempfile
SOURCE=Path(__file__).resolve().parents[1]
DEST='features/multimedia/theme_v9'
STATE='.spmp_theme_v9_manifest.json'
EXCLUDE={'__pycache__','.git'}
def sh(data):return hashlib.sha256(data).hexdigest()
def entries():
 a={}
 for path in SOURCE.rglob('*'):
  if not path.is_file() or any(x in EXCLUDE for x in path.relative_to(SOURCE).parts):continue
  if path.name in ('make_pack.py','fix_source_crops.py','background_contact.png') or (path.relative_to(SOURCE).parts[0]=='build' and path.suffix not in ('.a','.o')) or path.suffix in ('.ppm','.spft') or path.name in ('test_log.txt','crop_backgrounds.py') or path.name in ('test_theme_sanitized','theme_preview_host','theme_host.o') or path.name.endswith(('_black.ppm','_white.ppm')):continue
  if path.relative_to(SOURCE).parts[0] in ('previews',) and path.name!='ALL_THEMES_320x240.png':continue
  a[str(path.relative_to(SOURCE))]=path
 return a
def run(op,root):
 root=root.resolve(); dst=root/DEST;state=root/STATE
 if not (root/'include/defos_hal.h').exists() or not (root/'src/host/main.c').exists():raise RuntimeError('Not an SPMP-OS Stage10 project root')
 files=entries();
 if op=='install':
  if state.exists():raise RuntimeError('v9 manifest already exists; use verify or uninstall')
  collisions=[str(p) for rel in files if (dst/rel).exists()]
  if collisions:raise RuntimeError('Refusing overwrite: '+', '.join(collisions[:8]))
  hashes={rel:sh(path.read_bytes()) for rel,path in files.items()}
  installed=[]
  try:
   for rel,p in files.items():
    out=dst/rel;out.parent.mkdir(parents=True,exist_ok=True)
    # exclusive: fails if a file appears between preflight and write
    with out.open('xb') as f:f.write(p.read_bytes())
    installed.append(out)
   with state.open('x',encoding='utf8') as f:json.dump({'version':9,'root':DEST,'files':hashes},f,indent=2,ensure_ascii=False)
  except BaseException:
   for x in reversed(installed):x.unlink(missing_ok=True)
   state.unlink(missing_ok=True)
   raise
  print(f'Installed {len(files)} additive files; no existing files changed')
  return
 if not state.exists():raise RuntimeError('Missing v8 install manifest')
 m=json.loads(state.read_text());
 if m.get('version')!=9 or m.get('root')!=DEST:raise RuntimeError('Invalid manifest')
 diffs=[]
 for rel,h in m['files'].items():
  p=dst/rel
  if not p.is_file() or sh(p.read_bytes())!=h:diffs.append(rel)
 if diffs:raise RuntimeError(f'{len(diffs)} modified/missing theme files; refusing {op}: '+', '.join(diffs[:8]))
 if op=='verify':print(f'Verified {len(m["files"])} unchanged theme files; gaming files excluded');return
 if op=='uninstall':
  for rel in m['files']: (dst/rel).unlink()
  state.unlink()
  for path in sorted(dst.rglob('*'),key=lambda x:len(x.parts),reverse=True):
   if path.is_dir():
    try:path.rmdir()
    except OSError:pass
  for p in (dst,dst.parent,dst.parent.parent):
   try:p.rmdir()
   except OSError:pass
  print(f'Uninstalled {len(m["files"])} theme files; no gaming files touched')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('operation',choices=('install','verify','uninstall'));p.add_argument('stage10_root',type=Path)
 a=p.parse_args()
 try:run(a.operation,a.stage10_root)
 except Exception as e: print('ERROR:',e,file=sys.stderr);sys.exit(2)
