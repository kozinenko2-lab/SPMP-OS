#!/usr/bin/env python3
"""Read-only PRAW/MMPs audit. Treat extracted images as untrusted data."""
import argparse
import csv
import hashlib
import struct
from pathlib import Path

PATH=b'/Rom/image/8000_mmi.rap'

def scan(p):
    raw=p.read_bytes()
    assert raw[:4] == b'PRAW', 'not a PRAW container record'
    assert len(raw)-16 == struct.unpack_from('<I',raw,4)[0], 'bad PRAW size'
    segments=[]
    i=0
    while True:
        off=raw.find(b'MMPs',i)
        if off<0:break
        if off+24<=len(raw):
            length,virtual=struct.unpack_from('<II',raw,off+4)
            if 0<length<=len(raw)-off-24:
                segments.append({'header_offset':off, 'data_offset':off+24,
                                 'length':length,'virtual_address':virtual})
        i=off+4
    found=raw.find(PATH)
    vaddr=None
    if found>=0:
        for s in segments:
            if s['data_offset'] <= found < s['data_offset']+s['length']:
                vaddr=s['virtual_address']+found-s['data_offset'];break
    xrefs=[]
    if vaddr is not None:
        for s in segments:
            payload=raw[s['data_offset']:s['data_offset']+s['length']]
            word=struct.pack('<I',vaddr)
            pos=0
            while (pos:=payload.find(word,pos))>=0:
                xrefs.append(hex(s['virtual_address']+pos))
                pos+=4
    return {
        'variant':p.name,
        'sha256':hashlib.sha256(raw).hexdigest(),
        'bytes':len(raw),
        'entry_elf32_arm':hex(struct.unpack_from('<I',raw,0x88+24)[0]) if raw[0x88:0x8c]==b'\x7fELF' else None,
        'original_romfs_path':PATH.decode() if found>=0 else None,
        'path_file_offset':hex(found) if found>=0 else None,
        'path_vaddr':hex(vaddr) if vaddr is not None else None,
        'pointer_xrefs_virtual':';'.join(xrefs),
        'mmp_segments':len(segments),
        'contains_bootcfg':b'bootcfg' in raw,
        'contains_romfs':b'romfs_mount' in raw,
        'contains_clk_sd0':b'CLK_SD0' in raw,
        'contains_fatfs_signature':b'fatfs_' in raw
    }

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('directory',type=Path)
    ap.add_argument('--output',type=Path,required=True)
    a=ap.parse_args()
    rows=[scan(p) for p in sorted(a.directory.glob('*.praw'))]
    if not rows:raise SystemExit('no PRAW files')
    with a.output.open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
    for r in rows: print(r['variant'],r['original_romfs_path'],r['path_vaddr'],r['pointer_xrefs_virtual'])

if __name__=='__main__':main()
