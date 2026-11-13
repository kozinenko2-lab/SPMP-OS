#!/usr/bin/env python3
"""Public snapshot checks; optional private OEM/Ghidra checks. Never writes inputs."""
import argparse,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--quick',action='store_true');p.add_argument('--full',action='store_true');p.add_argument('--private',action='store_true');args=p.parse_args()
assert len(list((ROOT/'project/spmp-os/features/multimedia/theme_v9/themes').rglob('theme.json')))==34
assert len(list((ROOT/'project/spmp-os/features/filemanager').glob('FM0*/_FM0*_install_manifest.json')))==5
print('PASS: 34 themes, FM01–FM05 manifests')
if args.full:
    files=json.loads((ROOT/'docs/PUBLIC_SHA256.json').read_text())
    for rel,sha in files.items():
        f=ROOT/rel
        if not f.is_file() or hashlib.sha256(f.read_bytes()).hexdigest()!=sha:raise RuntimeError('file mismatch: '+rel)
    print('PASS: public snapshot',len(files),'file hashes')
if args.private:
    originals={'8000_MMI.RAP':'5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e','Images_RedBoot_201000427.img':'bc00123babe27af9af313ec8b19737f2b6fd3c533b901728a4f0112b55c9d108'}
    for name,sha in originals.items():
        f=ROOT/'firmware/originals'/name
        if not f.is_file():raise SystemExit('Missing local private reference: '+str(f))
        if hashlib.sha256(f.read_bytes()).hexdigest()!=sha:raise SystemExit('Original SHA mismatch: '+name)
    print('PASS: both private original hashes')
