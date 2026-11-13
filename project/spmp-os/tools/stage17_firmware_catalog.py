#!/usr/bin/env python3
"""Generate honest staging inventory; does not construct a flashable IMG."""
from pathlib import Path
import json, hashlib, subprocess
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'firmware'

def details(rel):
    p=ROOT/rel
    if not p.is_file():
        raise FileNotFoundError(p)
    d=p.read_bytes()
    return {'path':str(rel), 'bytes':len(d), 'sha256':hashlib.sha256(d).hexdigest()}

psx='firmware/modules/psx4all/psx4all_armv5_core_unresolved.o'
md='firmware/modules/megadrive/spmp_md_rom_armv5.o'
info=subprocess.run(['readelf','-h',str(ROOT/psx)],text=True,capture_output=True,check=True).stdout
assert 'ELF32' in info and 'ARM' in info and 'REL (Relocatable file)' in info
u=subprocess.run(['nm','-u',str(ROOT/psx)],text=True,capture_output=True,check=True).stdout
unresolved=sorted(set(line.split()[-1] for line in u.splitlines() if line.strip()))
if not unresolved:raise RuntimeError('PSX module unexpectedly linked/modified')
manifest={
 'name':'SPMP-OS module storage STAGING ONLY',
 'firmware_flashable':False,
 'hardware_verified':False,
 'redboot_modified':False,
 'modules':[
  {'id':'snes','state':'host_libretro_tested_not_ARMv5',
   'source':'user-supplied snes9x-master.zip',
   'host_shared_object':'third_party/snes9x/libretro/snes9x_libretro.so',
   'native_firmware_available':False},
  {'id':'megadrive','state':'REAL_PICODRIVE_HOST_TESTED_not_ARMv5_runnable',
   'source':'user-supplied picodrive-master.zip; Mega Drive-only profile',
   'core_source_in_package':True,'native_firmware_available':False,
   'host_libretro':details('third_party/picodrive/picodrive_libretro.so'),
   'rom_converter_armv5':details(md),
   'disabled_features':['Master System YM2413','Sega CD MP3/CHD'],'needs':['full ARMv5 runtime','verified SPMP-OS device HAL','actual Defender testing']},
  {'id':'ps1','state':'ARMv5_relocatable_UNLINKED_experimental',
   'native_firmware_available':False,'core_armv5':details(psx),
   'unresolved_symbol_count':len(unresolved),
   'source_bundle':details('firmware/modules/psx4all/PSX4ALL_Stage14_Source.zip'),
   'needs':['ARMv5 runtime/libgcc','SPU/CD glue','BIOS loader','PS1 video output','JIT cache privilege','device RAM map']}
 ]
}
(OUT/'module_catalog.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('PSX: %d unresolved symbols; manifest generated (NOT flashable)'%len(unresolved))
