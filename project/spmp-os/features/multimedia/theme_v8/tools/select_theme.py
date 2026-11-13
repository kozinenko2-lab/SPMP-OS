#!/usr/bin/env python3
"""Choose theme offline and write device-portable SPTC config to an output file."""
from pathlib import Path
import argparse,json,struct,zlib
r=Path(__file__).resolve().parents[1]
c=json.loads((r/'catalog.json').read_text(encoding='utf8'))
p=argparse.ArgumentParser(description='Persist selected SPMP theme into portable SPTC config (not Flash)')
p.add_argument('id',choices=c['theme_ids']);p.add_argument('--layout',choices=['list','grid'],default='list')
p.add_argument('--output',type=Path,default=r/'selected_theme.sptc')
a=p.parse_args()
raw=bytearray(40);raw[:4]=b'SPTC';raw[4]=1;raw[5]=int(a.layout=='grid')
raw[8:8+len(a.id)]=a.id.encode('ascii');raw[36:]=struct.pack('<I',zlib.crc32(raw[:36]))
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes(raw)
print(f'Selected {a.id} / {a.layout}; wrote {a.output} ({len(raw)} bytes, CRC32)')
