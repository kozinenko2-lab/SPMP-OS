#!/usr/bin/env python3
"""Strict, read-only PRAW OEM boot-site verification. NO patch output."""
from __future__ import annotations
import argparse,hashlib,json,struct
from pathlib import Path
ORIGINAL='bc00123babe27af9af313ec8b19737f2b6fd3c533b901728a4f0112b55c9d108'
VAR_HASH=[
 'eb989ab73900835fdfaf8d9f8ebb43b0d510a9548332bfc0d4fc6548728e3f9d',
 'ae3f375ab146bf16a43da334176416c0213e93907e02f6cbe1de600689ce50b2',
 '1723e3cebdb012e46c014a7215c9cb58978c081b25ba58738687d64b116b1034',
 '795f6138474cdd7b61d087694903584c436a0e82ec2c33ade3904fd20ea21b18',
 'c92122294bcc69d27b16bd159be9870e9bd764eeffcb110c9abca83da69e8d5a']
SITE={1:(0x4a998,0x4a9bc,0x4a9c0,0x4a9c4,(0xe59f0080,0xe59f1080,0xebffb950),0x4aa44,0x4aa48,0x59bb4),
      3:(0x467f4,0x46810,0x46814,0x46818,(0xe59f0084,0xe59f1084,0xebffc0f5),0x4689c,0x468a0,0x54d60)}
def u32(b,p):return struct.unpack_from('<I',b,p)[0]
def main(path,out):
 b=Path(path).read_bytes()
 if hashlib.sha256(b).hexdigest()!=ORIGINAL:raise ValueError('unexpected original IMG SHA-256')
 offset=0;records=[]
 for n in range(1,6):
  if b[offset:offset+4]!=b'PRAW':raise ValueError('PRAW magic missing')
  length=u32(b,offset+4)+16
  part=b[offset:offset+length]
  if hashlib.sha256(part).hexdigest()!=VAR_HASH[n-1]:raise ValueError('PRAW variant checksum mismatch')
  segments=[]
  # Only structural MMPs records matching the PRAW chain. Ignore incidental
  # occurrences of the bytes 'MMPs' within code/data.
  pos=part.find(b'MMPs')
  while pos>=0 and pos+24<=len(part):
   sz=u32(part,pos+4)
   addr=u32(part,pos+8)
   if sz>=24 and pos+sz<=len(part):segments.append((pos,pos+24,sz-24,addr))
   pos=part.find(b'MMPs',pos+4)
  def resolve(va,size):
   options=[(data+va-addr) for pos,data,extent,addr in segments
            if addr<=va and size<=extent and va-addr<=extent-size]
   if len(options)!=1:raise ValueError(f'VA {va:08x} ambiguous/no segment {options}')
   return options[0]
  s=SITE[3 if n in (3,5) else 1]
  init,loadpath,loadram,call,expected,litpath,litram,target=s
  words=[u32(part,resolve(v,4)) for v in (loadpath,loadram,call)]
  if words!=list(expected):raise ValueError(f'boot-site opcode mismatch #{n}')
  if u32(part,resolve(litpath,4))!=target or u32(part,resolve(litram,4))!=0x00a00000:
   raise ValueError(f'boot-site literal mismatch #{n}')
  path_bytes=b'/Rom/image/8000_mmi.rap\0'
  raw=part[resolve(target,len(path_bytes)):resolve(target,len(path_bytes))+len(path_bytes)]
  if raw!=path_bytes:raise ValueError(f'ROMFS factory path mismatch #{n}')
  records.append({'variant':n,'offset_in_original_img':f'0x{offset:08X}',
    'stock_function_va':f'0x{init:08X}', 'stock_path_literal_va':f'0x{litpath:08X}',
    'stock_ram_literal_va':f'0x{litram:08X}', 'load_function_call_va':f'0x{call:08X}',
    'function_call_file_offset_in_variant':f'0x{resolve(call,4):08X}',
    'load_destination':'0x00A00000','path':'/Rom/image/8000_mmi.rap',
    'verified_words':[f'0x{w:08X}' for w in words],
    'patch_written':False})
  offset+=length
 if offset!=len(b):raise ValueError('unexpected trailing data')
 result={'source_sha256':ORIGINAL,'variants':records,
  'warning':'Research only. These virtual addresses are not a validated board-specific flash patch.'}
 if out:Path(out).write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n')
 print(f"verified {len(records)} boot paths, 15 exact machine instructions and 10 literals")
if __name__=='__main__':
 a=argparse.ArgumentParser(description=__doc__);a.add_argument('original_img');a.add_argument('--json')
 ns=a.parse_args();main(ns.original_img,ns.json)
