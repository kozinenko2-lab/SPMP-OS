#!/usr/bin/env python3
"""Read-only exact-build boot audit. Python 3 standard library; never emits firmware."""
import argparse, hashlib, json, struct
from pathlib import Path
H={'8000_MMI.RAP':'5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e','Images_RedBoot_201000427.img':'bc00123babe27af9af313ec8b19737f2b6fd3c533b901728a4f0112b55c9d108'}
def u(b,o):return struct.unpack_from('<I',b,o)[0]
def segs(b):
 out=[];pos=0
 while (pos:=b.find(b'MMPs',pos))>=0:
  if pos+24<=len(b):
   n,a,d,z,t=struct.unpack_from('<5I',b,pos+4)
   if 24<=n<=len(b)-pos and a==d and z==0 and t in (0,1):out.append({'address':a,'offset':pos+24,'size':n-24,'type':t})
  pos+=4
 return out
class Image:
 def __init__(self,b):self.b=b;self.s=segs(b)
 def word(self,a):
  for s in self.s:
   if s['address']<=a and a+4<=s['address']+s['size']:return u(self.b,s['offset']+a-s['address'])
  raise ValueError(f'Unmapped word {a:#x}')
 def branch(self,a):
  w=self.word(a);assert w&0x0f000000==0x0b000000,hex(a)
  d=w&0xffffff;d=d-0x1000000 if d&0x800000 else d
  return (a+8+4*d)&0xffffffff
 def literal(self,a):
  w=self.word(a);assert w&0x0f7f0000==0x051f0000
  return self.word(a+8+(w&4095)*(1 if w&0x800000 else -1))
 def string(self,a):
  for s in self.s:
   if s['address']<=a<s['address']+s['size']:
    o=s['offset']+a-s['address'];return self.b[o:o+100].split(b'\0')[0].decode('ascii')
  raise ValueError('Unmapped string')
def checksum(b):
 n=u(b,4);assert n==len(b) and (n-16)%4==0
 words=struct.unpack('<%dI'%((n-16)//4),b[16:]);q=(n-16)//16
 return [(sum(words[:q])+sum(words[2*q:3*q]))&0xffffffff,(sum(words[q:2*q])+sum(words[3*q:]))&0xffffffff]
def audit(root):
 originals=root/'firmware/originals'; blobs={}
 for name,sha in H.items():
  b=(originals/name).read_bytes()
  if hashlib.sha256(b).hexdigest()!=sha:raise ValueError('SHA256 mismatch: '+name)
  blobs[name]=b
 b=blobs['8000_MMI.RAP'];im=Image(b)
 assert b[:4]==b'MMPi' and b[16:20]==b'MMPd'
 sums=checksum(b);assert sums==[u(b,8),u(b,12)]
 assert u(b,0x28)==0x280040 and u(b,0x24)==4 and u(b,0x30)==1
 assert [(s['address'],s['size']) for s in im.s]==[(0x280000,3059088),(0x6c8fe0,223296),(0,0),(0x9d807180,2024)]
 checks={}
 for a,t in [(0x28013c,0x299c90),(0x280140,0x2976d0),(0x280144,0x299a98),(0x280150,0x297ce8),(0x297cfc,0x2807c8),(0x297d00,0x33c0fc),(0x280810,0x339aac),(0x3be5cc,0x447640)]:assert im.branch(a)==t;checks[hex(a)]=hex(t)
 assert [im.literal(a) for a in [0x28011c,0x280120]]==[0x56ad90,0x6c8fdc]
 variants=[];raw=blobs['Images_RedBoot_201000427.img'];pos=0
 for n in range(1,6):
  assert raw[pos:pos+4]==b'PRAW';size=u(raw,pos+4)+16;v=raw[pos:pos+size];r=Image(v)
  fam=n in (1,2,4)
  call,path,ram,go,parse,fast=(0x4a9c4,0x4a9bc,0x4a9c0,0x4b304,0x427a8,None) if fam else (0x46818,0x46810,0x46814,0x46220,0x3e734,0x36ab4)
  loader=0x38f0c if fam else 0x36bf4
  assert r.branch(call)==loader
  assert r.string(r.literal(path))=='/Rom/image/8000_mmi.rap' and r.literal(ram)==0xa00000
  assert r.branch(0x38f90 if fam else 0x36c4c)==parse
  assert r.branch(0x4aa08 if fam else 0x4685c)==go
  assert r.string(r.literal(0x4a9d0 if fam else 0x46824))=='go'
  assert r.string(r.literal(0x4a9d8 if fam else 0x4682c))=='-c'
  variants.append({'variant':n,'source_offset':hex(pos),'bytes':size,'sha256':hashlib.sha256(v).hexdigest(),'stock_function_entry':hex(0x4a994 if fam else 0x467f4),'stock_call':hex(call),'file_loader':hex(loader),'mmp_parser':hex(parse),'go_handler':hex(go),'fast_loader':hex(fast) if fast else 'not traced in this audit','staging_buffer':'0xa00000','go_argv':['go','-c','0x280040'],'segments':r.s})
  pos+=size
 assert pos==len(raw)
 return {'input_sha256':H,'mmi':{'bytes':len(b),'segments':im.s,'entry':hex(u(b,0x28)),'mmpd_segment_count':u(b,0x24),'checksum_stored':[hex(u(b,8)),hex(u(b,12))],'checksum_computed':[hex(x) for x in sums],'checked_direct_calls':checks,'startup_zero_writes':{'first':'0x56ad90','last_inclusive':'0x6c8fdc','end_exclusive':'0x6c8fe0'}},'redboot_variants':variants,'limits':['Static analysis, no console execution.','PRAW selection for Defender board remains unresolved.','GZF databases are preserved but not opened by this audit.','go argv reconstructed on stock success path; runtime branches depend on boot configuration.']}
if __name__=='__main__':
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('archive_root',type=Path);ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
 result=audit(a.archive_root);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n');print('PASS: both original hashes, MMI checksum, segment map, startup calls, five RedBoot loader/go paths')
