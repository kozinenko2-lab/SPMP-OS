#!/usr/bin/env python3
"""Refresh hashes for tracked public files (git add new files first)."""
import hashlib,json,subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
files=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')
manifest={}
for rel in sorted(filter(None,files)):
    if rel=='docs/PUBLIC_SHA256.json':continue
    f=root/rel
    if f.is_file():manifest[rel]=hashlib.sha256(f.read_bytes()).hexdigest()
(root/'docs/PUBLIC_SHA256.json').write_text(json.dumps(manifest,indent=2,ensure_ascii=False)+'\n')
print('Hashed',len(manifest),'tracked source files')
