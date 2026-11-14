#!/usr/bin/env python3
"""Exact-RAP provenance for read-only upstream MMI input snapshots."""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from audit_boot import H,Image
TARGETS=[('state_get',0x2d2e40,16),('event_set',0x2d2e50,20),('collector',0x2d2e6c,32)]
LITERALS={0x2d2e4c:0x556144,0x2d2e60:0x556148,0x2d2f4c:0x556148,0x2d2f50:0x556144}
def synthetic_prefix(address,n):
 data=bytearray(((address>>2)+i*13+67)&255 for i in range(n))
 for a,v in LITERALS.items():
  if address<=a and a+4<=address+n:struct.pack_into('<I',data,a-address,v)
 return bytes(data)
def produce(blob):
 if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Exact original RAP required')
 im=Image(blob);rows=[]
 for a,v in LITERALS.items():assert im.word(a)==v
 for name,address,n in TARGETS:
  data=b''.join(struct.pack('<I',im.word(address+i)) for i in range(0,n,4))
  rows.append({'name':name,'address':address,'bytes':n,'crc32':zlib.crc32(data)})
 return {'source_sha256':H['8000_MMI.RAP'],'prefixes':rows,'literals':{hex(a):v for a,v in LITERALS.items()},
  'state_address':0x556144,'event_address':0x556148,'log_path':'/fat20a0/hda0/SPMPRAW.CSV',
  'limits':'Non-atomic read-only MMI RAM snapshots; not GPIO or all physical buttons. CRC checks are compatibility checks.'}
def header(r,synthetic=False):
 out=['/* Generated exact-build references; no OEM code bytes. */','static const struct raw_fingerprint raw_prefixes[3]={']
 for p in r['prefixes']:
  crc=zlib.crc32(synthetic_prefix(p['address'],p['bytes'])) if synthetic else p['crc32']
  out.append(' {'+hex(p['address'])+'u,'+str(p['bytes'])+'u,'+hex(crc)+'u}, /* '+p['name']+' */')
 return '\n'.join(out+['};',''])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in ['rap','json','header','synthetic-header']:p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();r=produce(a.rap.read_bytes());a.json.write_text(json.dumps(r,indent=2)+'\n');a.header.write_text(header(r));a.synthetic_header.write_text(header(r,True))
