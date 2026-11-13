#!/usr/bin/env python3
"""Negative link checks for the source-built analysis ELF, no OEM inputs.
Never emits a usable container; all temporary files remain in the build directory.
"""
import argparse,json,struct,subprocess,tempfile
from startup_elf import StartupELF
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]

def invalid_elf_checks(path):
    original=path.read_bytes();image=StartupELF(original);rows=[]
    phoff=struct.unpack_from('<I',original,28)[0]
    patches=[('wrong_machine',18,'<H',183),('hard_float',36,'<I',0x05000400),
        ('wrong_entry',24,'<I',0xa00004),('RWX_segment',phoff+32+24,'<I',7),
        ('runtime_outside_budget',phoff+32+20,'<I',0x400000)]
    variants=[]
    for name,offset,fmt,value in patches:
        data=bytearray(original);struct.pack_into(fmt,data,offset,value);variants.append((name,bytes(data)))
    variants.append(('truncated_program_headers',original[:60]))
    attr=image.sections['.ARM.attributes'];data=bytearray(original)
    at=original.find(b'\x06\x04',attr['offset'],attr['offset']+attr['bytes'])
    if at<0:raise AssertionError('Cannot locate ARMv5TE attribute for negative check')
    data[at+1]=10;variants.append(('unsupported_CPU_arch',bytes(data)))
    for name,data in variants:
        try:StartupELF(data)
        except ValueError:rows.append({'case':name,'elf_rejected':True})
        else:raise AssertionError('Invalid ELF accepted: '+name)
    return rows

def check(cc,ld,build):
    script=(BASE/'examples/native_startup/research.ld').read_text();rows=[]
    with tempfile.TemporaryDirectory(prefix='link_limits_',dir=build) as temporary:
        tmp=Path(temporary)
        source=tmp/'oversized.c'
        source.write_text('#include <stdint.h>\nvolatile unsigned char storage[128*1024]={1};\nuint32_t spmp_app_main(uintptr_t api){return storage[api&1];}\n')
        subprocess.run([cc,'--target=arm-none-eabi','-march=armv5te','-marm','-mfloat-abi=soft',
            '-ffreestanding','-fno-builtin','-c',str(source),'-o',str(tmp/'oversized.o')],check=True)
        variants=[('oversized_image',script,[build/'entry.o',tmp/'oversized.o'],'research budget exceeded'),
            ('displaced_entry',script.replace('KEEP(*(.text.spmp_start))','. += 4; KEEP(*(.text.spmp_start))'),
                [build/'entry.o',build/'application.o'],'native entry must be first byte'),
            ('footer_bss_collision',script.replace('.bss ALIGN(__reserved_footer_end__, 16)',
                '.bss ALIGN(__file_image_end__, 16)'),[build/'entry.o',build/'application.o'],
                'content/footer reserve/BSS overlap')]
        for name,variant,objects,expected in variants:
            linker_script=tmp/(name+'.ld');linker_script.write_text(variant)
            result=subprocess.run([ld,'-T',str(linker_script),'--no-undefined','--gc-sections',
                '-o',str(tmp/(name+'.elf')),*map(str,objects)],text=True,capture_output=True)
            if result.returncode==0 or expected not in result.stderr:
                raise AssertionError(name+': expected linker rejection: '+result.stderr)
            rows.append({'case':name,'link_rejected':True,'diagnostic':expected})
    return {'negative_link_cases':rows,'negative_elf_cases':invalid_elf_checks(build/'application.elf')}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cc',default='clang')
    p.add_argument('--ld',default='ld.lld');p.add_argument('--build',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    r=check(a.cc,a.ld,a.build.resolve());a.output.write_text(json.dumps(r,indent=2)+'\n')
    print('PASS: three invalid layouts rejected by linker, seven invalid ELFs rejected by audit')
