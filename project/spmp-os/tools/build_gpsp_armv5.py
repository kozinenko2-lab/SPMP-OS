#!/usr/bin/env python3
"""Cross-compile gpSP interpreter core for ARM926; audit unresolved HAL/runtime imports.
No generated .o is claimed bootable on a Defender board.
"""
import json, pathlib, shutil, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[1]
CORE = ROOT/'third_party/gpsp'
OUT = ROOT/'build/stage27/gpsp_armv5'
OUT.mkdir(parents=True, exist_ok=True)
FLAGS = ['--target=armv5te-none-eabi','-march=armv5te','-mfloat-abi=soft','-O2','-fno-exceptions','-fno-unwind-tables','-fno-asynchronous-unwind-tables','-DNDEBUG',
         '-D__LIBRETRO__','-DINLINE=inline','-Dmalloc=gpsp_heap_malloc','-Dcalloc=gpsp_heap_calloc','-Drealloc=gpsp_heap_realloc','-Dfree=gpsp_heap_free','-DHAVE_STRINGS_H','-DHAVE_STDINT_H','-DHAVE_INTTYPES_H',
         '-I'+str(ROOT/'tools/arm_gpsp_shim'),'-I'+str(CORE),'-I'+str(CORE/'libretro'),
         '-I'+str(CORE/'libretro/libretro-common/include')]
C = ['main.c','gba_memory.c','savestate.c','input.c','sound.c','cheats.c','memmap.c',
     'serial.c','gbp.c','rfu.c','serial_proto.c','libretro/libretro.c','gba_cc_lut.c']
CPP = ['video.cc','cpu.cc']
report = {'target':'armv5te-none-eabi','mode':'interpreter; HAVE_DYNAREC not defined', 'files':{}, 'objects':[], 'linked':False}
objects=[]
for name in C+CPP:
    src=CORE/name
    dst=OUT/(name.replace('/','_').replace('.cc','.o').replace('.c','.o'))
    cmd=[shutil.which('clang++') if name.endswith('.cc') else shutil.which('clang')]+FLAGS+(['-std=gnu++14'] if name.endswith('.cc') else ['-std=gnu11'])+['-c',str(src),'-o',str(dst)]
    try:
        p=subprocess.run(cmd, capture_output=True,text=True,timeout=40)
        report['files'][name]={'ok':p.returncode==0,'stderr':p.stderr[-3000:]}
        if p.returncode==0:
            objects.append(dst); report['objects'].append(str(dst.relative_to(ROOT)))
    except Exception as ex:
        report['files'][name]={'ok':False,'stderr':str(ex)}
if len(objects)==len(C)+len(CPP):
    # Compile bundled open-source BIOS as ARM relocatable data (not commercial BIOS).
    bios=OUT/'bios_data.o'
    p=subprocess.run([shutil.which('clang'), '--target=armv5te-none-eabi','-march=armv5te','-mfloat-abi=soft', '-c','bios_data.S','-o',str(bios)],cwd=CORE,capture_output=True,text=True)
    report['bios']={'ok':p.returncode==0,'stderr':p.stderr}
    if p.returncode==0: objects.append(bios)
    # Explicit core-only heap. These functions have no host OS dependencies.
    for source in [ROOT/'src/runtime/spmp_heap.c', ROOT/'src/modules/gpsp/gpsp_heap_bridge.c', ROOT/'src/runtime/spmp_gpsp_filestream.c', ROOT/'src/runtime/spmp_minilibc.c', ROOT/'src/runtime/spmp_numeric.c', ROOT/'src/runtime/spmp_aeabi_uldivmod.c', ROOT/'src/runtime/spmp_gpsp_services.c', ROOT/'src/runtime/spmp_gpsp_time.c', ROOT/'src/runtime/spmp_aeabi_softfloat.c']:
        dst=OUT/(source.stem+'.o')
        p=subprocess.run([shutil.which('clang'), '--target=armv5te-none-eabi','-march=armv5te','-mfloat-abi=soft','-std=c11','-O2','-fno-builtin','-I'+str(ROOT/'include'),'-I'+str(ROOT/'src/modules/gpsp'),'-I'+str(ROOT/'tools/arm_gpsp_shim'),'-I'+str(CORE),'-I'+str(CORE/'libretro'),'-I'+str(CORE/'libretro/libretro-common/include'),'-c',str(source),'-o',str(dst)],capture_output=True,text=True)
        report[source.name]={'ok':p.returncode==0,'stderr':p.stderr}
        if p.returncode==0: objects.append(dst)
    report['runtime_ready']=report['bios']['ok'] and report['spmp_heap.c']['ok'] and report['gpsp_heap_bridge.c']['ok'] and report['spmp_gpsp_filestream.c']['ok'] and report['spmp_minilibc.c']['ok'] and report['spmp_numeric.c']['ok'] and report['spmp_aeabi_uldivmod.c']['ok'] and report['spmp_gpsp_services.c']['ok'] and report['spmp_gpsp_time.c']['ok'] and report['spmp_aeabi_softfloat.c']['ok']
    # Division helper has a dedicated assembly ABI (r0 quotient, r1 remainder).
    divsrc=ROOT/'src/runtime/spmp_aeabi_uidivmod.S'
    divobj=OUT/'spmp_aeabi_uidivmod.o'
    divp=subprocess.run([shutil.which('clang'), '--target=armv5te-none-eabi','-march=armv5te','-mfloat-abi=soft','-c',str(divsrc),'-o',str(divobj)],capture_output=True,text=True)
    report['aeabi_uidivmod']={'ok':divp.returncode==0,'stderr':divp.stderr}
    if divp.returncode==0: objects.append(divobj)
    report['runtime_ready'] &= divp.returncode==0
    src64=ROOT/'src/runtime/spmp_aeabi_uldivmod.S'
    obj64=OUT/'spmp_aeabi_uldivmod_abi.o'
    p64=subprocess.run([shutil.which('clang'), '--target=armv5te-none-eabi','-march=armv5te','-mfloat-abi=soft','-c',str(src64),'-o',str(obj64)],capture_output=True,text=True)
    report['aeabi_uldivmod']={'ok':p64.returncode==0,'stderr':p64.stderr}
    if p64.returncode==0: objects.append(obj64)
    report['runtime_ready'] &= p64.returncode==0
    compat=CORE/'libretro/libretro-common/compat/compat_strl.c'
    compat_obj=OUT/'compat_strl.o'
    p=subprocess.run([shutil.which('clang'),*FLAGS,'-std=gnu11','-c',str(compat),'-o',str(compat_obj)],capture_output=True,text=True)
    report['compat_strl']={'ok':p.returncode==0,'stderr':p.stderr}
    if p.returncode==0: objects.append(compat_obj)
    report['runtime_ready'] &= report['compat_strl']['ok']

    cmd=[shutil.which('ld.lld'),'-r','-o',str(OUT/'gpsp_interpreter_core.o'),*map(str,objects)]
    p=subprocess.run(cmd,capture_output=True,text=True)
    report['linked']=(p.returncode==0); report['link_error']=p.stderr
    if p.returncode==0:
        nm=subprocess.run([shutil.which('llvm-nm') or shutil.which('nm'),'-u',str(OUT/'gpsp_interpreter_core.o')],capture_output=True,text=True)
        report['unresolved_symbols']=sorted(set(x.split()[-1] for x in nm.stdout.splitlines() if x.strip()))
report['passed']=sum(x['ok'] for x in report['files'].values())
report['total']=len(C)+len(CPP)
(OUT/'audit.json').write_text(json.dumps(report,indent=2,ensure_ascii=False)+'\n')
print(f"ARMv5 gpSP core: {report['passed']}/{report['total']} compiled; partial linked: {report['linked']}; unresolved: {len(report.get('unresolved_symbols',[]))}")
for k,v in report['files'].items():
    if not v['ok']: print('FAILED:',k,v['stderr'][:350])
sys.exit(0 if report['linked'] and report.get('runtime_ready') else 1)
