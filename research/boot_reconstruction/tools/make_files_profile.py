#!/usr/bin/env python3
"""Exact RAP references for read-only directory traversal; no firmware changes."""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from audit_boot import H,Image
TARGETS=[('opendir',0x2a69bc,64),('readdir_r',0x2a6b8c,80),('closedir',0x2a6be0,20),('stat',0x2a6504,80),('read',0x2eb5e0,32)]
def synthetic_prefix(address,n):return bytes(((address>>3)+i*23+49)&255 for i in range(n))
def produce(blob):
 if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Exact RAP required')
 im=Image(blob)
 assert im.branch(0x2a69f0)==0x2a5bf0 and im.word(0x2a69ec)==0xe3a00001
 assert im.word(0x2a6bb4)==0xe3a02f41 and im.branch(0x2a6bb8)==0x2eb5e0
 assert im.branch(0x2a6bec)==0x2eb640 and im.word(0x2a65ac)==0xe59cf034
 return {'source_sha256':H['8000_MMI.RAP'],'prefixes':[{'name':name,'address':a,'bytes':n,'crc32':zlib.crc32(b''.join(struct.pack('<I',im.word(a+i)) for i in range(0,n,4)))} for name,a,n in TARGETS],
  'directory_handle':'opendir reserves fd>=1; zero means failure; closedir forwards fd to close',
  'dirent_bytes':260,'dirent_name_offset':0,'dirent_has_type':False,
  'stat_bytes':32,'stat_mode_offset':0,'stat_size_offset':16,'stat_mtime_offset':24,'mode_directory_bit':1,'mode_regular_bit':8,
  'limits':'Selected interface compatibility only; no real filesystem/card/device test.'}
def header(r,synthetic=False):
 out=['/* Generated compatibility references, no OEM instruction bytes. */','static const struct files_fingerprint files_prefixes[5]={']
 for p in r['prefixes']:
  crc=zlib.crc32(synthetic_prefix(p['address'],p['bytes'])) if synthetic else p['crc32']
  out.append(' {'+','.join(hex(x)+'u' for x in [p['address'],p['bytes'],crc])+'}, /* '+p['name']+' */')
 return '\n'.join(out+['};',''])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for n in ['rap','json','header','synthetic-header']:p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();r=produce(a.rap.read_bytes());a.json.write_text(json.dumps(r,indent=2)+'\n');a.header.write_text(header(r));a.synthetic_header.write_text(header(r,True))
