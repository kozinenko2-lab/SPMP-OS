#!/usr/bin/env python3
"""Read-only provenance/integration audit of user-provided native source ZIPs.
No extracted sources are republished and no firmware flash commands are issued.
"""
import argparse
import hashlib
import json
import zipfile
from pathlib import Path

TARGETS = [
 ('libspmp8k-master.zip', 'ARM926 / eCos host ABI, Doom+z26 embedded'),
 ('xpectrum-spmp8000-master.zip', 'ZX Spectrum ARM source; libgame dependency'),
 ('tgemu-spmp8000-master.zip', 'PC Engine ARM source; libgame dependency'),
 ('Frodo4-spmp8000-master.zip', 'C64 ARM source; libgame dependency'),
 ('wiituka-spmp8000-master.zip', 'Amstrad CPC ARM source; libgame dependency'),
 ('xrick-spmp8000-master.zip', 'Rick Dangerous source; original assets licensing caveat'),
 ('scummvm-spmp8000-master.zip', 'ScummVM tree; no confirmed SPMP backend in archive'),
 ('psx4all-dingoo-master.zip', 'Dingoo PS1 port; MIPS dynarec'),
 ('psx4all-dingoo-master-1.zip', 'Variant of Dingoo PS1 port; compare before reuse'),
 ('actsemi-master.zip', 'Actsemi homebrew SDK, MIPS32 incompatible with SPMP ARM'),
]

def audit(path, label):
    with zipfile.ZipFile(path) as z:
        members=[x for x in z.infolist() if not x.is_dir()]
        root=members[0].filename.split('/')[0] if members else ''
        file_names=[x.filename for x in members]
        code_count=sum(x.filename.lower().endswith(('.c','.cpp','.cc','.h','.hpp','.s','.S')) for x in members)
        extract_size=sum(x.file_size for x in members)
        return dict(name=path.name, sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                    description=label, source_files=code_count, entries=len(members),
                    uncompressed_bytes=extract_size,
                    has_libgame_sources=(root+'/libgame/libgame.c' in file_names),
                    has_doom_source=any('/demos/doom110/' in s for s in file_names),
                    has_z26_source=any('/demos/z26/' in s for s in file_names),
                    has_spmp_scummvm_backend=any('/backends/platform/spmp' in s.lower() for s in file_names))

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--archives', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    args=ap.parse_args()
    results=[]
    for basename,label in TARGETS:
        p=args.archives/basename
        if not p.is_file():raise FileNotFoundError(p)
        results.append(audit(p,label))
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps({'read_only':True,'archives':results},indent=2,ensure_ascii=False)+'\n')
    print(f'Wrote {len(results)} archive audits to {args.out}')
if __name__=='__main__': main()
