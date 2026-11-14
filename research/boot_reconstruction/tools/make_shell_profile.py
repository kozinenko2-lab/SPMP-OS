#!/usr/bin/env python3
"""Exact-image RAM lock gate provenance; no device writes."""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from audit_boot import H,Image
TARGETS=[('input_gate',0x2d8b78,0x2d8b84,0x556aa0),('gui_lock',0x3c3264,0x3c3270,0x561078)]
def synthetic_prefix(address):
 data=bytearray(((address>>2)+i*17+31)&255 for i in range(16))
 for name,a,literal,value in TARGETS:
  if a==address:struct.pack_into('<I',data,12,value)
 return bytes(data)
def produce(blob):
 if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Exact RAP required')
 im=Image(blob);rows=[]
 for name,address,literal,value in TARGETS:
  assert im.word(literal)==value
  data=b''.join(struct.pack('<I',im.word(address+i)) for i in range(0,16,4))
  rows.append({'name':name,'address':address,'literal':literal,'value':value,'bytes':16,'crc32':zlib.crc32(data)})
 return {'source_sha256':H['8000_MMI.RAP'],'prefixes':rows,'limits':'Read-only RAM flags; runtime CRC is compatibility, not full RAM-image identity.'}
def header(r,synthetic=False):
 out=['/* Generated provenance, no OEM code bytes. */','static const struct shell_fingerprint shell_prefixes[2]={']
 for p in r['prefixes']:
  crc=zlib.crc32(synthetic_prefix(p['address'])) if synthetic else p['crc32']
  out.append(' {'+','.join(hex(x)+'u' for x in [p['address'],p['literal'],p['value'],crc])+'},')
 return '\n'.join(out+['};',''])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for n in ['rap','json','header','synthetic-header']:p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();r=produce(a.rap.read_bytes());a.json.write_text(json.dumps(r,indent=2)+'\n');a.header.write_text(header(r));a.synthetic_header.write_text(header(r,True))
