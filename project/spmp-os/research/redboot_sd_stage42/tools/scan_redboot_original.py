#!/usr/bin/env python3
"""Read-only inspector for Sunplus/Generalplus concatenated PRAW RedBoot image.
Does NOT create a flashable image or write to an attached device.
"""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

MAGIC=b'PRAW'

def parse(path, extract=None):
    data = Path(path).read_bytes()
    starts=[]
    p=0
    while True:
        i=data.find(MAGIC,p)
        if i < 0: break
        starts.append(i)
        p=i+4
    variants=[]
    for num,start in enumerate(starts,1):
        size=struct.unpack_from('<I',data,start+4)[0]
        end=start+size+16
        next_start=starts[num] if num < len(starts) else len(data)
        if end != next_start:
            raise ValueError(f'Variant {num}: PRAW length ({end:#x}) does not match next PRAW boundary ({next_start:#x})')
        segment=data[start:end]
        hdr=struct.unpack_from('<7I',segment,4)
        scan=re.search(rb'\+ScanRam ver ([^\x00]+)',segment)
        scan_ver=scan.group(1).decode('ascii','replace') if scan else None
        elf_positions=[]
        search=0
        while True:
            off=segment.find(b'\x7fELF',search)
            if off < 0: break
            # Must be a true little endian ARM ELF header with sensible program header count.
            if off+52<=len(segment):
                ident=segment[off:off+16]
                mach=struct.unpack_from('<H',segment,off+18)[0]
                phnum=struct.unpack_from('<H',segment,off+44)[0]
                if ident[4]==1 and ident[5]==1 and mach==40 and 0<phnum<32:
                    elf_positions.append(off)
            search=off+4
        redboot_version=re.search(rb'Non-certified release, version ([^\x00]+)',segment)
        details={
            'variant':num,
            'source_offset_hex':hex(start),
            'total_size_bytes':len(segment),
            'payload_size_from_header_bytes':size,
            'praw_header_second_u32_hex':hex(hdr[1]),
            'praw_header_unknown_fields_hex':[hex(v) for v in hdr[2:]],
            'redboot_version':redboot_version.group(1).decode('ascii','replace') if redboot_version else None,
            'scanram_version':scan_ver,
            'arm_elf_headers_offsets_within_praw_hex':[hex(off) for off in elf_positions],
            'contains_linux_boot_string':b'[linux boot]' in segment,
            'contains_linux_kernel_name':b'linux_kernel.rap' in segment,
            'contains_8000_mmi_name':b'8000_mmi.rap' in segment,
            'contains_redboot_prompt':b'RedBoot> ' in segment,
            'sha256':hashlib.sha256(segment).hexdigest(),
        }
        variants.append(details)
        if extract:
            Path(extract).mkdir(parents=True,exist_ok=True)
            (Path(extract)/f'redboot_variant_{num}_scanram_{scan_ver or "unknown"}.praw').write_bytes(segment)
    return {'source_name':Path(path).name,'source_size_bytes':len(data),'source_sha256':hashlib.sha256(data).hexdigest(),'variant_count':len(variants),'variants':variants,
            'warning':'The PRAW variants are not known to match Defender hardware. Do not flash without verified board matching and a tested NAND backup/recovery path.'}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('img')
    p.add_argument('--extract',metavar='DIR',help='Extract complete original PRAW records (read-only source)')
    p.add_argument('--json',metavar='FILE')
    a=p.parse_args()
    result=parse(a.img,a.extract)
    output=json.dumps(result,indent=2,ensure_ascii=False)
    if a.json: Path(a.json).write_text(output+'\n',encoding='utf-8')
    print(output)
