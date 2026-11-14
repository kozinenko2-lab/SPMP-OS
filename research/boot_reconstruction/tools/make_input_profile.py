#!/usr/bin/env python3
"""Exact-image additional references for time/delay and absolute SD file I/O."""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from audit_boot import H,Image
TARGETS={'time':0x2d3254,'delay':0x339be8,'open':0x2a5ed4,'write':0x2eb610,'seek':0x2eb69c,'close':0x2eb640,'errno':0x299a14}
def synthetic_input_prefix(address):return bytes(((address>>2)+i*29+7)&255 for i in range(32))
def produce(blob):
 if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Exact original RAP required')
 im=Image(blob);rows=[]
 for name,address in TARGETS.items():
  data=b''.join(struct.pack('<I',im.word(address+i)) for i in range(0,32,4))
  rows.append({'name':name,'address':address,'bytes':32,'crc32':zlib.crc32(data)})
 return {'source_sha256':H['8000_MMI.RAP'],'prefixes':rows,'sd_path':'/fat20a0/hda0/SPMPKEYS.CSV',
  'open_flags':10,'seek_end_whence':2,'time_formula':'uint32((tick_low - native_init_tick_low)*10)',
  'limits':'Compatibility CRC32, not whole firmware identity. Tick period and SD write need device confirmation.'}
def header(profile,synthetic=False):
 out=['/* Generated references; no OEM code bytes. */','static const struct input_fingerprint input_prefixes['+str(len(profile['prefixes']))+']={']
 for r in profile['prefixes']:
  crc=zlib.crc32(synthetic_input_prefix(r['address'])) if synthetic else r['crc32']
  out.append(' {'+hex(r['address'])+'u,'+hex(crc)+'u}, /* '+r['name']+' */')
 return '\n'.join(out+['};',''])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in ['rap','json','header','synthetic-header']:p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();r=produce(a.rap.read_bytes());a.json.write_text(json.dumps(r,indent=2)+'\n');a.header.write_text(header(r));a.synthetic_header.write_text(header(r,True))
