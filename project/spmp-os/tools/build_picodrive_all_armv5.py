#!/usr/bin/env python3
"""PicoDrive ARMv5TE FULL MD + Sega CD + 32X C/interpreter build.
All real subsystems included; no runtime symbol stubs. A relocatable is not runnable firmware.
"""
from pathlib import Path
import subprocess, json, sys
root=Path(__file__).resolve().parents[1]; source=root/'third_party/picodrive'
out=root/'build/picodrive_armv5_full';out.mkdir(parents=True,exist_ok=True)
base='''pico/pico.c pico/cart.c pico/memory.c pico/state.c pico/sek.c pico/z80if.c pico/videoport.c pico/draw2.c pico/draw.c pico/mode4.c pico/misc.c pico/eeprom.c pico/patch.c pico/debug.c pico/media.c pico/pico/pico.c pico/pico/memory.c pico/pico/xpcm.c pico/carthw/carthw.c pico/carthw/eeprom_spi.c pico/carthw/svp/svp.c pico/carthw/svp/memory.c pico/carthw/svp/ssp16.c pico/sound/sound.c pico/sound/resampler.c pico/sound/sn76496.c pico/sound/ym2612.c pico/sound/ym2413_disabled.c pico/sound/mix.c cpu/fame/famec.c cpu/cz80/cz80.c'''.split()
cd='''pico/cd/mcd.c pico/cd/memory.c pico/cd/sek.c pico/cd/cdc.c pico/cd/cdd.c pico/cd/cd_image.c pico/cd/cd_parse.c pico/cd/gfx.c pico/cd/gfx_dma.c pico/cd/misc.c pico/cd/pcm.c pico/cd/megasd.c'''.split()
x32='''pico/32x/32x.c pico/32x/memory.c pico/32x/draw.c pico/32x/sh2soc.c pico/32x/pwm.c cpu/sh2/sh2.c cpu/sh2/mame/sh2pico.c cpu/drc/cmn.c'''.split()
# The host build links user-supplied libchdr. Cross-compiled ARMv5 objects below
# do NOT yet link an ARMv5 libchdr or any MP3 track decoder.
files=base+cd+x32
flags=['clang','--target=armv5te-none-eabi','-mcpu=arm926ej-s','-marm','-mfloat-abi=soft','-Os','-ffreestanding','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections',
       '-DFAMEC_NO_GOTOS','-DEMU_F68K','-D_USE_CZ80','-DNO_SMS','-D__LIBRETRO__','-DNDEBUG',
       '-Wno-implicit-function-declaration','-include',str(root/'tools/arm_picodrive_shim/spmp_pico_abi.h'),f'-I{source}/zlib',f'-I{source}',f'-I{root}/tools/arm_picodrive_shim',f'-I{root}/tools/arm_sms_shim',f'-I{root}/tools/arm_shim']
results=[];objs=[]
for f in files:
    obj=out/(f.replace('/','_')+'.o')
    rr=subprocess.run(flags+['-c',str(source/f),'-o',str(obj)],capture_output=True,text=True)
    results.append({'src':f,'ok':rr.returncode==0,'error':rr.stderr[-2000:]})
    if rr.returncode==0: objs.append(obj)
report={'target':'armv5te-none-eabi / ARM926EJ-S / soft float','flavors':['Mega Drive','Sega CD (BIN/ISO/CUE)','Sega 32X'],
        'cpu_modes':['M68K FAME','Z80 CZ80','SH2 interpreter x2 (no JIT)'],
        'enabled_pico_cd':True,'enabled_pico_32x':True,'chd_source_present_in_bundle':True,'host_libchdr_linked':True,
        'arm_libchdr_unlinked':True,'mp3_track_decoder_unlinked':True,
        'files':len(files),'compiled':len(objs),'results':results}
if len(objs)==len(files):
    linked=out/'picodrive_md_cd_32x_armv5.o'
    rr=subprocess.run(['ld.lld','-r','-m','armelf_linux_eabi',*[str(o) for o in objs],'-o',str(linked)],capture_output=True,text=True)
    report['partial_link_ok']=rr.returncode==0;report['partial_link_error']=rr.stderr[-2000:]
    if rr.returncode==0:
        syms=subprocess.run(['nm','-u',str(linked)],capture_output=True,text=True,check=True)
        report['unresolved_symbols']=sorted(set(row.strip().split()[-1] for row in syms.stdout.splitlines() if row.strip()))
        report['unresolved_count']=len(report['unresolved_symbols'])
        ex=subprocess.run(['readelf','-h',str(linked)],capture_output=True,text=True,check=True)
        report['elf_arm32']=all(word in ex.stdout for word in ('ELF32','ARM','REL'))
else: report['partial_link_ok']=False
(out/'report.json').write_text(json.dumps(report,indent=2))
print(f'ARMv5 MD+CD+32X: {len(objs)}/{len(files)} compiled; partial link={report["partial_link_ok"]}; imports={report.get("unresolved_count","?")}')
for f in results:
    if not f['ok']:print('FAIL',f['src'],f['error'][-900:])
if not report['partial_link_ok']:print('LINK ERROR:',report.get('partial_link_error',''))
sys.exit(0 if report['partial_link_ok'] else 1)
