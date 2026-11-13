#!/usr/bin/env python3
"""OPTIONAL host UI integration. Never auto-applied; checks base SHA256 and backups.
Warning: not an installed console boot path. Does not modify any emulator sources.
"""
import hashlib,sys,shutil,argparse
from pathlib import Path
BASE={
'src/host/main.c':None,
'Makefile':None,
}
def sha(s):return hashlib.sha256(s).hexdigest()
def make_new(main,make):
 anchor='int main(int argc,char **argv) {'
 assert main.count(anchor)==1
 main=main.replace(anchor,'''extern int spmp_ui_v7_preview_cli(int argc,char**argv);\n'''+anchor+'''
 if(argc>=6 && !strcmp(argv[1],"--theme-v8-preview")) {
   char *args[5]={argv[0],argv[2],argv[3],argv[4],argv[5]};
   return spmp_ui_v7_preview_cli(5,args);
 }
''')
 anchor='.PHONY: all host probe arm-core test clean'
 assert make.count(anchor)==1
 make=make.replace(anchor,'''# Optional independent 320x240 theme engine; no emulator files altered.
HOST_SRC += features/multimedia/theme_v8/src/spmp_ui_theme_v7.c features/multimedia/theme_v8/src/spmp_ui_builtin_glyphs.c features/multimedia/theme_v8/src/theme_preview_host.c
CFLAGS += -Ifeatures/multimedia/theme_v8/include -DSPMP_THEME_V7_NO_MAIN -DSPMP_THEME_V8_HOOK
'''+anchor)
 return main,make
def main():
 p=argparse.ArgumentParser();p.add_argument('stage10',type=Path);p.add_argument('--uninstall',action='store_true');p.add_argument('--check',action='store_true');a=p.parse_args()
 root=a.stage10;main=root/'src/host/main.c';make=root/'Makefile';backup=root/'.spmp_theme_v8_host_backup'
 if a.uninstall:
  if not backup.is_dir():raise SystemExit('No optional host hook backup exists')
  for k in BASE:
   filename=root/k; saved=backup/k
   if not saved.exists():raise SystemExit('Incomplete backup')
   orig=saved.read_bytes();current=filename.read_text()
   target=make_new(*[(backup/'src/host/main.c').read_text(),(backup/'Makefile').read_text()])[0 if k=='src/host/main.c' else 1]
   if current!=target:raise SystemExit('Conflict: host source edited since hook; refusing to overwrite '+k)
   filename.write_bytes(orig)
  shutil.rmtree(backup);print('Optional host hook restored, other files untouched');return
 if backup.exists():raise SystemExit('Host hook already installed or backup exists')
 if not (root/'features/multimedia/theme_v8/src/spmp_ui_theme_v7.c').is_file():raise SystemExit('Install additive theme package first')
 old_main=main.read_text();old_make=make.read_text()
 if 'spmp_ui_v7_preview_cli' in old_main or 'SPMP_THEME_V7_NO_MAIN' in old_make or 'SPMP_THEME_V8_HOOK' in old_make:raise SystemExit('Hook detected, do not overwrite')
 new_main,new_make=make_new(old_main,old_make)
 if a.check:print('Host hook can be applied, two project files would change');return
 for k in BASE:
  path=backup/k;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes((root/k).read_bytes())
 main.write_text(new_main);make.write_text(new_make)
 print('OPTIONAL hook installed: make host; ./build/spmp-os-host --theme-v8-preview features/multimedia/theme_v8/themes nature list /tmp/ui.ppm')
if __name__=='__main__':main()
