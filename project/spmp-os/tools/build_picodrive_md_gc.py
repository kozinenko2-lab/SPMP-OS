#!/usr/bin/env python3
"""Build function-section ARMv5 MD profile and analyze LIVE unresolved dependencies.
No fake symbols, no claim of executable firmware; link fails closed if key deps absent.
"""
from pathlib import Path
import json, subprocess, sys
root=Path(__file__).resolve().parents[1]
src=root/'third_party/picodrive'
out=root/'build/picodrive_armv5_md_gc';out.mkdir(parents=True, exist_ok=True)
files='''pico/pico.c pico/cart.c pico/memory.c pico/state.c pico/sek.c pico/z80if.c pico/videoport.c pico/draw2.c pico/draw.c pico/mode4.c pico/misc.c pico/eeprom.c pico/patch.c pico/debug.c pico/media.c pico/pico/pico.c pico/pico/memory.c pico/pico/xpcm.c pico/carthw/carthw.c pico/carthw/eeprom_spi.c pico/carthw/svp/svp.c pico/carthw/svp/memory.c pico/carthw/svp/ssp16.c pico/sound/sound.c pico/sound/resampler.c pico/sound/sn76496.c pico/sound/ym2612.c pico/sound/ym2413_disabled.c pico/sound/mix.c cpu/fame/famec.c cpu/cz80/cz80.c'''.split()
flags=['clang','--target=armv5te-none-eabi','-mcpu=arm926ej-s','-marm','-mfloat-abi=soft','-Os','-ffreestanding','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-DFAMEC_NO_GOTOS','-DEMU_F68K','-D_USE_CZ80','-DNO_SMS','-DNO_32X','-D__LIBRETRO__','-DNDEBUG','-Wno-implicit-function-declaration',f'-I{src}/zlib',f'-I{src}',f'-I{root}/tools/arm_picodrive_shim',f'-I{root}/tools/arm_sms_shim',f'-I{root}/tools/arm_shim']
objects=[]
for f in files:
 p=out/(f.replace('/','_')+'.o');r=subprocess.run(flags+['-c',str(src/f),'-o',str(p)],capture_output=True,text=True)
 if r.returncode: print('FAILED',f,r.stderr[-1000:]);sys.exit(1)
 objects.append(str(p))
roots=['PicoInit','PicoExit','PicoFrame','PicoCartInsert','PicoCartUnload','PicoReset','PicoState','PicoDrawSetOutBuf','PicoDrawSetOutFormat','PicoGetSound','PicoPower']
linked=out/'picodrive_md_live.o'
cmd=['ld.lld','-m','armelf_linux_eabi','-r','--gc-sections',*[v for x in roots for v in ('-u',x)],*objects,'-o',str(linked)]
r=subprocess.run(cmd,capture_output=True,text=True)
if r.returncode:print('LINK FAILED',r.stderr);sys.exit(2)
def undefined(path):
 p=subprocess.run(['nm','-u',str(path)],capture_output=True,text=True,check=True)
 return sorted(set(s.split()[-1] for s in p.stdout.splitlines() if s.strip()))
old=root/'build/picodrive_armv5_expanded/picodrive_expanded_armv5_unresolved.o'
prev=undefined(old) if old.exists() else []
now=undefined(linked)
external={'libc_or_compiler':[x for x in now if x.startswith('__aeabi') or x in ('malloc','calloc','realloc','free','memcpy','memmove','memset','memcmp','printf','sprintf','fopen','fclose','fread','fwrite','fseek','ftell','feof','fgets','strcmp','strncmp','strcpy','strncpy','strlen','strchr','strrchr','strtol','strtoul','strcasecmp','strncasecmp','rand','abs','log','sin','tan','sqrt','pow','floor','fabs','gettimeofday')], 'compression_io':[x for x in now if x.startswith(('gz','inflate','crc32','openzip','readzip','closezip','seekcompresszip'))], 'cd_32x_and_optional':[x for x in now if ('32x' in x.lower() or 'mcd' in x.lower() or 'cd' in x.lower() or 's68k' in x.lower() or 'pcm' in x.lower() or 'mp3' in x.lower() or 'wram' in x.lower())], 'platform':[x for x in now if x.startswith(('plat_','emu_','lprintf'))]}
classified=set(sum(external.values(),[]));external['other']=[x for x in now if x not in classified]
report={'architecture':'armv5te-none-eabi ARM926EJ-S softfloat','selected_objects':len(objects),'roots':roots,'before_count':len(prev),'after_gc_count':len(now),'removed_count':len(set(prev)-set(now)),'remaining':now,'removed':sorted(set(prev)-set(now)),'categories':external,'live_obj_bytes':linked.stat().st_size,'executable':False}
(out/'report.json').write_text(json.dumps(report,indent=2,ensure_ascii=False))
print(json.dumps({k:report[k] for k in ('selected_objects','before_count','after_gc_count','removed_count','live_obj_bytes','executable')},indent=2))
