#!/usr/bin/env python3
"""Read-only, non-executing metadata analysis of original SPMP8000 .bin ports."""
import argparse, hashlib, json, re
from pathlib import Path
MAGICS={"NGame1.0": b'NGame1.0', "ARM ELF": b'\x7fELF', "MMPi": b'MMPi'}
KEYS=["/Rom/IMAGE/GAME/SPDF.BIN", "/fat20a", "doom.wad", "doom2.wad", "config.cfg", "smsgg.cfg", "spmp8k", "scummvm", "SSL_PlayNext"]

def inspect(p):
    b=p.read_bytes()
    kind=next((kind for kind,magic in MAGICS.items() if b.startswith(magic)),"unknown / embedded raw")
    strings=set()
    for m in re.finditer(rb'[\x20-\x7e]{5,100}',b):
        s=m.group().decode('ascii','ignore')
        if any(k.casefold() in s.casefold() for k in KEYS):
            strings.add(s[:100])
    return {"filename":p.name,"size_bytes":len(b),"sha256":hashlib.sha256(b).hexdigest(),
            "format":kind,"identifying_strings":sorted(strings)[:55],
            "notes":("Signature alone does not demonstrate successful execution on Defender."
                     if kind=='NGame1.0' else "Not NGame1.0: investigate loader and memory map separately.")}

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('paths',nargs='+',type=Path);ap.add_argument('-o','--output',type=Path)
    args=ap.parse_args()
    report=[inspect(p) for p in args.paths]
    data=json.dumps(report,ensure_ascii=False,indent=2)
    if args.output: args.output.write_text(data+'\n',encoding='utf-8')
    else: print(data)
