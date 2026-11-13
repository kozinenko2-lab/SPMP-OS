#!/usr/bin/env python3
"""Reproduce a CPU-only ARMv5 compile of the user-supplied TGEmu source.

This is not a complete emulator/link/load test. The archive remains external,
so the SPMP-OS project can be distributed without repackaging third-party code.
"""
import argparse
import pathlib
import subprocess
import tempfile
import zipfile

ROOT=pathlib.Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--archive',type=pathlib.Path,required=True)
    ap.add_argument('--output',type=pathlib.Path,required=True)
    ap.add_argument('--clang',default='clang')
    args=ap.parse_args()
    if not args.archive.is_file():ap.error('Missing TGEmu source archive')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='spmp-tgemu-') as td:
        top=pathlib.Path(td); root=top/'tgemu-spmp8000-master'
        # Extract only the controlled source subtree (no .git, binaries, or assets).
        with zipfile.ZipFile(args.archive) as z:
            for info in z.infolist():
                name=pathlib.PurePosixPath(info.filename)
                if info.is_dir() or len(name.parts)<3 or name.parts[0]!='tgemu-spmp8000-master' or name.parts[1]!='src':continue
                if any(x in ('..','.') for x in name.parts) or info.file_size>8*1024*1024:raise ValueError('Unexpected ZIP entry')
                path=top.joinpath(*name.parts)
                path.parent.mkdir(parents=True,exist_ok=True)
                path.write_bytes(z.read(info))
        patch=ROOT/'patches/tgemu/0001-clang-armv5-portable-registers.patch'
        subprocess.run(['patch','-p1','-i',str(patch)],cwd=root,check=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        shim=top/'shim';shim.mkdir()
        (shim/'stdio.h').write_text('/* CPU-only header shim: no stdio calls in h6280.c */\n')
        (shim/'string.h').write_text('#include <stddef.h>\nextern void *memset(void*,int,size_t);\nextern void *memcpy(void*,const void*,size_t);\n')
        cmd=[args.clang,'--target=armv5te-none-eabi','-mcpu=arm926ej-s','-marm',
             '-ffreestanding','-O2','-DLSB_FIRST','-DFAST_MEM','-DSPMP_PORTABLE_C',
             '-I'+str(shim),'-Isrc','-Isrc/cpu','-c','src/cpu/h6280.c','-o',str(args.output.resolve())]
        subprocess.run(cmd,cwd=root,check=True)
    print('ARMv5 HuC6280 CPU object created:',args.output)
    print('This code still requires TGEmu memory, I/O, video, audio, and SPMP-OS HAL.')
if __name__=='__main__':main()
