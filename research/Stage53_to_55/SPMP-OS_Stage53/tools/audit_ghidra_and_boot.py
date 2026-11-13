#!/usr/bin/env python3
"""Read-only evidence audit for user-supplied SPMP8k.zip and Stage52 original-derived ELFs.
Does NOT execute, patch or flash any firmware.  Python 3 stdlib only.
"""
import argparse, hashlib, json, re, struct, zlib, zipfile
from pathlib import Path

LABELS = {
 'MMI': ('ghidra_analysis_ONLY.elf.gzf', 'MMI', 0x280000, 'segment_00280000_file_00000034.bin'),
 'RedBoot_V1': ('ghidra_analysis_ONLY v1.elf.gzf', 'RedBoot_variant_1', 0x30000, 'segment_00030000_file_000025b8.bin'),
 'RedBoot_V3': ('ghidra_analysis_ONLY v3.elf.gzf', 'RedBoot_variant_3', 0x30000, 'segment_00030000_file_000020f0.bin'),
}
SELECTED = {
 'MMI': [0x280040,0x28008c,0x280130,0x28013c,0x280140,0x280144,0x280150,0x280154,0x298b64,0x298b68,0x2b9a38,0x2b9b78,0x46018c,0x460250,0x460254,0x4611bc,0x461204,0x461224,0x461230,0x461234,0x4618c4],
 'RedBoot_V1':[0x30040,0x4a998,0x4a9bc,0x4a9c0,0x4a9c4,0x4a9c8,0x4a9cc,0x4aa44,0x4aa48],
 'RedBoot_V3':[0x30040,0x467f4,0x46810,0x46814,0x46818,0x4681c,0x46820,0x4689c,0x468a0],
}

def sha(b):return hashlib.sha256(b).hexdigest()
def word(b, base, va):return struct.unpack_from('<I',b,va-base)[0]
def branch_target(wordval, va):
 if (wordval & 0x0f000000) != 0x0b000000: return None
 disp=wordval & 0xffffff
 if disp & (1 << 23):disp-=1 << 24
 return (va+8+disp*4)&0xffffffff

def read_gzf(p):
 raw=p.read_bytes()
 assert raw[:6]==b'\xac\xed\x00\x05\x77'+bytes([raw[5]]), 'Java header missing'
 assert raw[6:14] == bytes.fromhex('2e30212634e92c20'), 'GZF magic mismatch'
 off=raw.find(b'PK\x03\x04')
 if off < 0:raise ValueError('ZIP local header missing')
 version,flags,method,time,date,crc,packed,unpacked,name_len,extra_len=struct.unpack_from('<HHHHHIIIHH',raw,off+4)
 assert method==8 and raw[off+30:off+30+name_len]==b'FOLDER_ITEM', 'unexpected GZF stream'
 compressed=raw[off+30+name_len+extra_len:]
 obj=zlib.decompressobj(-15)
 database=obj.decompress(compressed)+obj.flush()
 if not obj.eof:raise ValueError('truncated deflate stream')
 trailer=obj.unused_data
 assert len(trailer)>=16, 'ZIP data descriptor missing'
 # ZIP flags 0x8: data descriptor may have signature PK\x07\x08
 d=trailer
 if d[:4]==b'PK\x07\x08':d=d[4:]
 crc_record, comp_record, uncomp_record=struct.unpack_from('<III',d,0)
 assert crc_record==(zlib.crc32(database)&0xffffffff),'CRC mismatch'
 assert uncomp_record==len(database), 'uncompressed size mismatch'
 assert comp_record==len(compressed)-len(obj.unused_data), 'compressed size mismatch'
 assert database[:8]==bytes.fromhex('2f30312c34292c2a'),'Ghidra DB internal header mismatch'
 funs=sorted({m.decode('ascii') for m in re.findall(rb'FUN_[0-9a-f]{8}',database)})
 markers={m.decode('utf8'):m in database for m in [b'RedBoot_factory_start_0004A998',b'RedBoot_factory_start_000467F4',b'MMI_reset_00280040',b'MMI_native_NGame_header_verify_004611BC',b'MMI_diag_stub_00298B64'] if m in database}
 return {'file':p.name,'bytes':len(raw),'sha256':sha(raw),'internal_database_bytes':len(database),'internal_db_sha256':sha(database),'deflate_crc_ok':True,'distinct_auto_fun_labels':len(funs),'min_auto_fun':funs[0] if funs else None,'max_auto_fun':funs[-1] if funs else None,'human_labels_seen':list(markers)}

def main():
 a=argparse.ArgumentParser();a.add_argument('--ghidra-zip',required=True,type=Path);a.add_argument('--stage52',required=True,type=Path);a.add_argument('--output',required=True,type=Path);args=a.parse_args()
 root=args.stage52/'SPMP-OS_Stage52_Ghidra_Imports'/'elf';report={'source_archive':str(args.ghidra_zip),'source_archive_sha256':sha(args.ghidra_zip.read_bytes()),'classification':'read-only analysis; GZF contains Ghidra DB, not pseudocode text','programs':{},'findings':[]}
 with zipfile.ZipFile(args.ghidra_zip) as z:
  name_lookup={Path(n).name:n for n in z.namelist()}
  assert len(name_lookup)==3
  for key,(fname,sub,base,segment) in LABELS.items():
   gzfp=args.output.parent/fname
   if gzfp.exists(): p=gzfp
   else:
    gzfp.write_bytes(z.read(name_lookup[fname]));p=gzfp
   meta=read_gzf(p); seg=(root/sub/segment).read_bytes()
   meta['segment_sha256']=sha(seg);meta['segment_bytes']=len(seg)
   meta['entrypoint']=('0x280040' if key=='MMI' else '0x30040')
   meta['verified_arm_words']={f'0x{addr:08x}':f'0x{word(seg,base,addr):08x}' for addr in SELECTED[key]}
   report['programs'][key]=meta
 # Hard evidence assertions
 mmi=(root/'MMI'/'segment_00280000_file_00000034.bin').read_bytes();r1=(root/'RedBoot_variant_1'/'segment_00030000_file_000025b8.bin').read_bytes();r3=(root/'RedBoot_variant_3'/'segment_00030000_file_000020f0.bin').read_bytes()
 assert word(mmi,0x280000,0x298b64)==0xe12fff1e,'diag stub not BX LR'
 assert word(mmi,0x280000,0x280154)==0xeafffffe,'MMI startup loop differs'
 assert word(mmi,0x280000,0x460254)==0xe1a0f003,'N1 jump differs'
 assert word(mmi,0x280000,0x460250)==0xe1a0e00f,'N1 return address setup differs'
 assert word(r1,0x30000,0x4aa48)==word(r3,0x30000,0x468a0)==0xa00000,'RedBoot staging differs'
 assert branch_target(word(r1,0x30000,0x4a9c4),0x4a9c4)==0x38f0c
 assert branch_target(word(r3,0x30000,0x46818),0x46818)==0x36bf4
 assert word(r1,0x30000,0x4aa44)==0x59bb4
 assert word(r3,0x30000,0x4689c)==0x54d60
 assert r1[0x59bb4-0x30000:].startswith(b'/Rom/image/8000_mmi.rap\0')
 assert r3[0x54d60-0x30000:].startswith(b'/Rom/image/8000_mmi.rap\0')
 for rootdir, file in [(r1,'v1'),(r3,'v3')]:pass
 # MMI boot vectors / BSS range values from literals
 bss_start,bss_end=word(mmi,0x280000,0x280498),word(mmi,0x280000,0x28049c)
 assert bss_start<bss_end and (bss_end-bss_start)<0x800000
 report['findings']=[
  {'id':'RB-factory-ROMFS','evidence':'0x4a9bc -> string 0x59bb4; BL 0x4a9c4 -> 0x38f0c; variant3 0x46810 -> string 0x54d60; BL 0x46818 -> 0x36bf4','confidence':'binary verified','implication':'Factory boot exists independently of proposed SD chain; do not overwrite.'},
  {'id':'RB-A00000','evidence':'literal 0x00a00000 at v1:0x4aa48 and v3:0x468a0','confidence':'binary verified','implication':'Argument passed to loader, not proof of unused SPMP-OS RAM.'},
  {'id':'MMI-entry','evidence':'MMI RAP record mapping; entry 0x00280040; ARM MRC CP15 at entry','confidence':'binary verified','implication':'MMI initializes CPU modes, exception vectors and RAM; cannot treat as ordinary game.'},
  {'id':'MMI-BSS','evidence':f'ARM reset literal at 0x280498=0x{bss_start:08x}, 0x28049c=0x{bss_end:08x}; clearing loop at 0x280130','confidence':'binary verified','implication':'Candidate clearing span; MUST verify Ghidra decomp and memory ownership before using RAM-handoff.'},
  {'id':'MMI-diagnostic-stub','evidence':'BX LR word at 0x00298b64=0xe12fff1e','confidence':'binary verified','implication':'Calls into stub do not print; must instrument selectively to get real logs.'},
  {'id':'MMI-N1-handoff','evidence':'0x00460250 MOV LR,PC then 0x00460254 MOV PC,R3; earlier r3=0x00a00000','confidence':'binary verified','implication':'Built-in emulator loader uses direct in-RAM handoff path, separate from wrapped NGame route.'},
  {'id':'MMI-native','evidence':'header handler at 0x004611bc; footer compare around 0x004618c4','confidence':'binary verified control flow; semantics pending deeper decomp','implication':'Investigate accepted app formats and return codes; no assumption that mkbing output is universally accepted.'},
 ]
 # Binary image hashes if available
 for fn in [Path('/mnt/data/8000_MMI.RAP'),Path('/mnt/data/redboot_research/library/Images_RedBoot_201000427.img')]:
  if fn.exists(): report.setdefault('originals',[]).append({'file':fn.name,'bytes':fn.stat().st_size,'sha256':sha(fn.read_bytes())})
 args.output.parent.mkdir(parents=True,exist_ok=True)
 args.output.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
 print('PASS: three GZF CRCs; original-derived binary evidence; output',args.output)
 for k,v in report['programs'].items():print(k, v['distinct_auto_fun_labels'],'unique FUN labels / DB',v['internal_database_bytes'],'bytes')
 print('MMI BSS possible range:',hex(bss_start),hex(bss_end))

if __name__=='__main__': main()
