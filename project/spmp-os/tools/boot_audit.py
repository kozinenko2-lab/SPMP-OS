#!/usr/bin/env python3
"""Inspect user's SPMP8000 RedBoot PRAW bundle + FSP module, no modifications.

Report fields are observed data, not a board compatibility judgment.
"""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

ELF_FMT='<16sHHIIIIIHHHHHH'
PH_FMT='<8I'

def sha(data):return hashlib.sha256(data).hexdigest()
def audit_praw(path):
    data=Path(path).read_bytes(); off=0; variants=[]
    while off<len(data):
        if data[off:off+4]!=b'PRAW':
            raise ValueError(f'PRAW magic absent at {off:#x}')
        size=struct.unpack_from('<I',data,off+4)[0]+16
        if size<256 or off+size>len(data):
            raise ValueError(f'Invalid PRAW size at {off:#x}')
        b=data[off:off+size]
        eo=b.find(b'\x7fELF')
        if eo<0 or eo+52>len(b):
            raise ValueError(f'Missing ELF header in PRAW at {off:#x}')
        h=struct.unpack_from(ELF_FMT,b,eo)
        ident,typ,machine,version,entry,phoff,shoff,flags,ehsize,phsize,phnum,shsize,shnum,shstr=h
        if ident[:6]!=b'\x7fELF\x01\x01' or machine!=40:
            raise ValueError('Not an ARM ELF32 little endian header')
        if phnum>32 or phsize!=32 or eo+phoff+phnum*phsize>len(b):
            raise ValueError('Implausible header table')
        phdr=[]
        for i in range(phnum):
            p_type,p_offset,vaddr,paddr,filesz,memsz,pflags,align=struct.unpack_from(PH_FMT,b,eo+phoff+i*phsize)
            phdr.append(dict(type=p_type,offset=hex(p_offset),vaddr=hex(vaddr),paddr=hex(paddr),filesz=filesz,memsz=memsz,flags=hex(pflags),align=align))
        m=re.search(rb'\+ScanRam ver ([^\x00]+)',b)
        v=re.search(rb'Non-certified release, version ([^\x00]+)',b)
        variants.append(dict(index=len(variants)+1,offset=hex(off),container_bytes=len(b),
             header_field_plus8=hex(struct.unpack_from('<I',b,8)[0]),
             scanram=m.group(1).decode('ascii','replace') if m else None,
             redboot=(v.group(1).decode('ascii','replace').strip() if v else None),
             elf_header_offset=hex(eo),entry_point=hex(entry),elf_program_headers=phdr,
             strings={k:(v.encode('ascii') in b) for k,v in {'linux_kernel.rap':'linux_kernel.rap',
               '8000_mmi.rap':'8000_mmi.rap','bootcfg':'bootcfg','bootkernel':'bootkernel:',
               'go':'go_with_addr:','redboot_prompt':'RedBoot> '}.items()},sha256=sha(b)))
        off+=size
    return {'file':str(path),'bytes':len(data),'sha256':sha(data),'variants':variants}
def audit_mmi(path):
    b=Path(path).read_bytes()
    names=['MMPi','MMPd','MMPs','getBootConfig','RedBoot_ver=%08x',
           'Scanram_ver=%08x','fatfs_mkdir','fatfs_rename','fatfs_rmdir',
           'fatfs_unlink','/fat20a0','/fat20a1','/fat20a2','linux_kernel.rap']
    return {'file':str(path),'bytes':len(b),'sha256':sha(b),
            'signatures':{name:(hex(b.find(name.encode('ascii'))) if name.encode('ascii') in b else None) for name in names}}
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--redboot',required=True)
    p.add_argument('--mmi',required=True)
    p.add_argument('--output',required=True)
    a=p.parse_args()
    out={'redboot':audit_praw(a.redboot),'mmi':audit_mmi(a.mmi),
     'warnings':['Do not flash PRAW to Defender without confirmed board match and recovery.',
       'ELF headers embedded in PRAW describe an uncompressed memory layout; do not slice PRAW as a complete ELF.',
       'bootcfg and linux_kernel.rap strings do not establish a supported boot mode on Defender hardware.',
       'The ARM probe is relocatable and must not run until entry ABI and an unused RAM range are verified.']}
    Path(a.output).write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(f"Analyzed {len(out['redboot']['variants'])} RedBoot variants, wrote {a.output}")
if __name__=='__main__':main()
