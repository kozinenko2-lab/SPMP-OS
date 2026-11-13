#!/usr/bin/env python3
"""Read-only evidence generator for OEM SPMP8000 SD_P430 driver.
Works with the user-supplied 8000_MMI.RAP only; no firmware changes.
"""
from __future__ import annotations
import argparse,csv,hashlib,json,struct
from collections import defaultdict
from pathlib import Path

EXPECTED_SHA="5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e"
BASE=0x280000
FILE_PAYLOAD=0x4c
SD_START=0x2b9000
SD_END=0x2bd800
MARKERS=(
  b"SD_P430.c",b"[SD]::%s(%d)",b"[SD]::",
  b"SD card not insert",b"force reset sd card",b"SD_POLL_WAIT",
  b"dump SD Cardinfo",b"SD resume OK",b"SD already init",
  b"fail to stop SD ctler",b"fatSDUnmount_FatDev",b"fatfs_mount",
  b"CLK_SD0",b"CLK_SD1")

def u32(blob,address):
    off=FILE_PAYLOAD+address-BASE
    if off<0 or off+4>len(blob):return None
    return struct.unpack_from('<I',blob,off)[0]

def report(raw:bytes):
    digest=hashlib.sha256(raw).hexdigest()
    if digest!=EXPECTED_SHA:
        raise ValueError("Unsupported RAP SHA-256: refusing address attribution")
    if raw[:4]!=b'MMPi' or raw[0x34:0x38]!=b'MMPs':
        raise ValueError("Unexpected MMP package")
    # A PC-relative ARM LDR literal points to an aligned inline word.
    references=defaultdict(list)
    for address in range(SD_START,SD_END,4):
        instruction=u32(raw,address)
        if instruction is None: continue
        if instruction & 0x0f7f0000 != 0x051f0000:continue
        off=instruction &0xfff
        pool=(address+8 & ~3)+(off if instruction&0x800000 else -off)
        value=u32(raw,pool)
        if value is not None:
            references[value].append(dict(ldr_pc=f"0x{address:08X}",pool=f"0x{pool:08X}",word=f"0x{instruction:08X}"))
    marker_rows=[]
    for key in MARKERS:
        start=0
        while True:
            fileoffset=raw.find(key,start)
            if fileoffset<0:break
            address=BASE+(fileoffset-FILE_PAYLOAD)
            marker_rows.append(dict(marker=key.decode('ascii'),file_offset=f"0x{fileoffset:08X}",
                                    runtime_address=f"0x{address:08X}",
                                    code_ldr_references=references.get(address,[])))
            start=fileoffset+len(key)
    mmio=[]
    for value,sites in sorted(references.items()):
        if 0x92000000<=value<0x93000000:
            mmio.append(dict(candidate_address=f"0x{value:08X}",sites=sites))
    return dict(sha256=digest,segment_1_load="0x00280000",
                static_sd_window=[f"0x{SD_START:08X}",f"0x{SD_END:08X}"],
                driver_marker_count=len(marker_rows),
                direct_ldr_literal_references=sum(map(len,references.values())),
                found_sd_strings=marker_rows,
                possible_mmio_literals=mmio,
                caveat="MMIO values are candidate addresses referenced by ARM code, not a verified register specification; never write to them based on this report")

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('rap',type=Path)
    ap.add_argument('--outdir',type=Path,required=True)
    args=ap.parse_args()
    r=report(args.rap.read_bytes())
    args.outdir.mkdir(parents=True,exist_ok=True)
    (args.outdir/'sd_p430_static_map.json').write_text(json.dumps(r,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    with (args.outdir/'sd_p430_string_xrefs.csv').open('w',newline='',encoding='utf-8') as f:
        w=csv.writer(f);w.writerow(['string','file_offset','runtime_address','ldr_reference','literal_pool','instruction'])
        for s in r['found_sd_strings']:
            if s['code_ldr_references']:
                for x in s['code_ldr_references']:
                    w.writerow([s['marker'],s['file_offset'],s['runtime_address'],x['ldr_pc'],x['pool'],x['word']])
            else:w.writerow([s['marker'],s['file_offset'],s['runtime_address'],'','',''])
    with (args.outdir/'sd_p430_mmio_candidates.csv').open('w',newline='',encoding='utf-8') as f:
        w=csv.writer(f);w.writerow(['candidate_address','ldr_reference','literal_pool','instruction','classification'])
        for x in r['possible_mmio_literals']:
            for site in x['sites']:
                w.writerow([x['candidate_address'],site['ldr_pc'],site['pool'],site['word'],'unverified_candidate'])
    print(json.dumps({k:r[k] for k in ('sha256','driver_marker_count','direct_ldr_literal_references')},indent=2))
    print('candidate MMIO literals:',len(r['possible_mmio_literals']))
    print('files:',args.outdir)
if __name__=='__main__':main()
