#!/usr/bin/env python3
"""Extract binding references from exact private RAP; export metadata only.
CRC32 of short code prefixes detects changes, not cryptographic firmware identity.
Synthetic profile uses own byte patterns and is only for public VM tests.
"""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from audit_boot import Image,H
from audit_stage3 import extract_native_api
NAMES=['init_graph','origin','double_buffer','dimensions','get_color','set_color','fill','paint','keys']
OFFSETS=[0x38,0x54,0xa8,0x94,0x48,0x44,0xc4,0x10,0x100]

def synthetic_prefix(slot):return bytes((slot*17+i*13)&255 for i in range(32))

def extract(blob):
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Exact OEM RAP SHA-256 required')
    im=Image(blob);slots=[int(r['target'],16) for r in extract_native_api(im)]
    checks=[]
    for name,offset in zip(NAMES,OFFSETS):
        address=slots[offset//4]
        prefix=b''.join(struct.pack('<I',im.word(address+i)) for i in range(0,32,4))
        checks.append({'name':name,'slot':offset//4,'target':address,'bytes':32,'crc32':zlib.crc32(prefix)})
    return {'source_sha256':H['8000_MMI.RAP'],'api_base':0x5efb74,'slots':slots,'prefixes':checks,
        'limits':'Offline source SHA verified; runtime table and short CRC32 prefixes are compatibility checks, not a full RAM-image SHA or tamper proof.'}

def header(profile,synthetic=False):
    lines=['/* Generated compatibility references; no OEM code bytes. */',
        '/* Source RAP SHA-256: '+profile['source_sha256']+' */',
        'static const uint32_t binding_slots[85]={']
    for i in range(0,85,8):lines.append('    '+','.join(hex(v)+'u' for v in profile['slots'][i:i+8])+',')
    lines.extend(['};','static const struct binding_fingerprint binding_prefixes[9]={'])
    for p in profile['prefixes']:
        checksum=zlib.crc32(synthetic_prefix(p['slot'])) if synthetic else p['crc32']
        lines.append('    {'+str(p['slot'])+'u,'+hex(checksum)+'u}, /* '+p['name']+' */')
    lines.extend(['};',''])
    return '\n'.join(lines)

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',required=True,type=Path)
    p.add_argument('--json',required=True,type=Path);p.add_argument('--header',required=True,type=Path)
    p.add_argument('--synthetic-header',required=True,type=Path);p.add_argument('--check',action='store_true');a=p.parse_args()
    r=extract(a.rap.read_bytes());outputs={a.json:json.dumps(r,indent=2)+'\n',a.header:header(r),a.synthetic_header:header(r,True)}
    for path,content in outputs.items():
        if a.check:
            if path.read_text()!=content:raise ValueError('Stale binding profile: '+str(path))
        else:path.write_text(content)
    print('PASS: exact RAP, 85 slots, nine 32-byte code-prefix checksums')
