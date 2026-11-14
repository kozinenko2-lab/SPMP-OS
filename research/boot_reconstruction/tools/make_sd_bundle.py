#!/usr/bin/env python3
"""Build our SD folder skeleton and RAM app only; never copies ROMs or OEM files."""
import argparse,hashlib,json,zipfile
from pathlib import Path
DIRECTORIES=['System/Apps','System/Cores/Atari2600','System/Lib','System/Fonts','System/Themes','User/ROMs/Atari2600','User/Ports','User/Music','User/Video','User/Pictures','User/Books','User/Saves/Atari2600','User/Settings','User/Logs']
def build(binary,output):
 data=binary.read_bytes()
 if not data.startswith(b'NGame1.0'):raise ValueError('Source-built NGame required')
 manifest={'format':'SPMP_OS_SD_LAYOUT_V1','entry':'SPMP_OS_FILES.bin','entry_sha256':hashlib.sha256(data).hexdigest(),'system':'SPMP-OS/System','user':'SPMP-OS/User','roms_atari2600':'SPMP-OS/User/ROMs/Atari2600','log':'SPMP-OS/User/Logs/SPMPFM.CSV','filesystem':'SD0','mode':'RAM browser, no ROM execution or user-file mutations yet','directories':DIRECTORIES}
 with zipfile.ZipFile(output,'w',compression=zipfile.ZIP_DEFLATED) as z:
  for folder in DIRECTORIES:z.writestr('SPMP-OS/'+folder+'/',b'')
  z.writestr('SPMP_OS_FILES.bin',data)
  fonts=Path(__file__).resolve().parents[3]/'project/spmp-os/assets/fonts'
  for filename in ['Roboto-Regular.ttf','LICENSE-Apache-2.0.txt','NOTICE.txt','font_manifest.json']:
   z.write(fonts/filename,'SPMP-OS/System/Fonts/'+filename)
  fallback=fonts.parents[1]/'features/multimedia/theme_v9/docs/DEJAVU_LICENSE_NOTICE.txt'
  z.write(fallback,'SPMP-OS/System/Fonts/Fallback-DejaVu-NOTICE.txt')
  z.writestr('SPMP-OS/System/INSTALL.json',json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
  z.writestr('README_SD_RU.txt','Распакуйте в корень SD. Запустите SPMP_OS_FILES.bin обычным способом.\nSystem — файлы и библиотеки SPMP-OS; User — ваши данные.\nRoboto-Regular.ttf читается напрямую из System/Fonts, с кэшем символов.\nROM положите в SPMP-OS/User/ROMs/Atari2600. ROM и код Atari player в пакет не включены.\nЭто первый read-only файловый менеджер; A показывает сведения о файле, игру пока не запускает.\nA открыть, B родитель/назад, L/R панель, X обновить, START главное меню, короткое POWER меню продолжения/выхода.\nЛог SPMP-OS/User/Logs/SPMPFM.CSV. Ничего существующего автоматически не переносится и не удаляется.\n')
 return manifest
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bin',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=build(a.bin,a.output);print('PASS: SD skeleton and own RAM app;',len(DIRECTORIES),'directories; no game assets')
