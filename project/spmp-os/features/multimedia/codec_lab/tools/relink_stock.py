#!/usr/bin/env python3
"""RESEARCH ONLY. Re-link original user-supplied legacy ARM ELF objects.

This does not decompile/recompile closed firmware source, does not adapt runtime
semantics, does not create a runnable program and does not modify Flash.
Re-linked proprietary objects remain in a LOCAL output folder; never included
in the redistributable SPMP-OS CodecLab package.
"""
import argparse
import hashlib
import pathlib
import shutil
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]

def run(cmd):
    result = subprocess.run(cmd, capture_output=True, text=True, check=False)
    if result.returncode:
        raise RuntimeError('link failed: '+' '.join(map(str,cmd))+'\n'+result.stderr)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--stock', type=pathlib.Path, required=True,
                    help='folder containing original LIBMP3.O / LIBFLAC.O extracted from your FSP')
    ap.add_argument('--out', type=pathlib.Path, required=True,
                    help='output directory OUTSIDE redistributable patch')
    args = ap.parse_args()
    src = args.stock.resolve()
    dst = args.out.resolve()
    if not src.is_dir(): raise SystemExit('stock folder is missing')
    for tool in ['clang','ld.lld','readelf']:
        if not shutil.which(tool): raise SystemExit('missing tool: '+tool)
    dst.mkdir(parents=True, exist_ok=True)
    for kind in ('MP3','FLAC'):
        orig = src / f'LIB{kind}.O'
        if not orig.is_file(): raise SystemExit('missing original: '+str(orig))
        header = subprocess.run(['readelf','-h',str(orig)],capture_output=True,text=True,check=True).stdout
        if ('Machine:                           ARM' not in header or
            'Type:                              REL' not in header or
            'little endian' not in header):
            raise SystemExit('unexpected original ELF: '+str(orig))
        wrapper = dst / f'query_{kind.lower()}_armv5.o'
        relinked = dst / f'legacy_{kind.lower()}_linked_armv5.o'
        run(['clang','--target=armv5te-none-eabi','-march=armv5te','-marm',
             '-mfloat-abi=soft','-std=c11','-O2','-ffreestanding','-fno-builtin',
             '-fno-unwind-tables',f'-DSPMP_RELINK_{kind}','-c',
             str(ROOT/'src/spmp_legacy_query.c'),'-o',str(wrapper)])
        run(['ld.lld','-m','armelf','-r',str(wrapper),str(orig),'-o',str(relinked)])
        sha=hashlib.sha256(orig.read_bytes()).hexdigest()
        print(f'{kind}: original {orig.stat().st_size} bytes, SHA256 {sha}')
        print(f'  linked object: {relinked} ({relinked.stat().st_size} bytes)')
    print('Static re-link COMPLETE; no runtime test, no safe playback, no flash image.')

if __name__ == '__main__': main()
