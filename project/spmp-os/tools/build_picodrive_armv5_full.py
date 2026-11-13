#!/usr/bin/env python3
"""Compile an expanded Mega Drive-only PicoDrive code path for ARM926EJ-S.
Objects are relocatable, NOT a bootable/linked firmware module.
"""
from pathlib import Path
import subprocess, json, sys
root=Path(__file__).resolve().parents[1]
source=root/'third_party/picodrive'
out=root/'build/picodrive_armv5_expanded'
out.mkdir(parents=True,exist_ok=True)
files='''pico/pico.c pico/cart.c pico/memory.c pico/state.c pico/sek.c pico/z80if.c pico/videoport.c pico/draw2.c pico/draw.c pico/mode4.c pico/misc.c pico/eeprom.c pico/patch.c pico/debug.c pico/media.c pico/pico/pico.c pico/pico/memory.c pico/pico/xpcm.c pico/carthw/carthw.c pico/carthw/eeprom_spi.c pico/carthw/svp/svp.c pico/carthw/svp/memory.c pico/carthw/svp/ssp16.c pico/sound/sound.c pico/sound/resampler.c pico/sound/sn76496.c pico/sound/ym2612.c pico/sound/ym2413_disabled.c pico/sound/mix.c cpu/fame/famec.c cpu/cz80/cz80.c'''.split()
flags=['clang','--target=armv5te-none-eabi','-mcpu=arm926ej-s','-marm','-mfloat-abi=soft','-Os','-ffreestanding','-fno-builtin','-fno-strict-aliasing','-DFAMEC_NO_GOTOS','-DEMU_F68K','-D_USE_CZ80','-DNO_SMS','-DNO_32X','-D__LIBRETRO__','-DNDEBUG','-Wno-implicit-function-declaration',f'-I{source}/zlib',f'-I{source}',f'-I{root}/tools/arm_picodrive_shim',f'-I{root}/tools/arm_sms_shim',f'-I{root}/tools/arm_shim']
results=[];success=[]
for f in files:
    obj=out/(f.replace('/','_')+'.o')
    run=subprocess.run(flags+['-c',str(source/f),'-o',str(obj)],text=True,capture_output=True)
    results.append({'src':f,'ok':run.returncode==0,'error':run.stderr[-1000:]})
    if run.returncode==0: success.append(obj)
report={'target':'armv5te-none-eabi / arm926ej-s / soft float','total':len(files),'compiled':len(success),'results':results}
(out/'report.json').write_text(json.dumps(report,indent=2))
if success:
    run=subprocess.run(['ld.lld','-r','-m','armelf_linux_eabi',*[str(x) for x in success],'-o',str(out/'picodrive_expanded_armv5_unresolved.o')],capture_output=True,text=True)
    report['partial_link_ok']=run.returncode==0
    report['partial_link_error']=run.stderr[-3000:]
    if run.returncode==0:
        syms=subprocess.run(['nm','-u',str(out/'picodrive_expanded_armv5_unresolved.o')],text=True,capture_output=True)
        report['unresolved_symbols']=[x.strip().split()[-1] for x in syms.stdout.splitlines() if x.strip()]
        report['unresolved_count']=len(report['unresolved_symbols'])
    (out/'report.json').write_text(json.dumps(report,indent=2))
print('ARMv5 compiled',len(success),'/',len(files),'partial link',report.get('partial_link_ok'))
for r in results:
    if not r['ok']: print('FAIL',r['src'],r['error'][:170].replace('\n',' '))
sys.exit(0 if len(success)==len(files) and report.get('partial_link_ok') else 1)
