#!/usr/bin/env python3
"""Static ELF/MMPi inventory. Never executes firmware binaries."""
import argparse
from collections import Counter
import hashlib
import json
import pathlib
import re
import subprocess

LIBS=['LIBMP3.O','LIBAAC.O','LIBWMA.O','LIBFLAC.O','LIBAPE.O','LIBAC3.O','LIBAMRNB.O','LIBAMRWB.O']
MMP=['H264_CODEC.IMG','MPG4ASPDEC.IMG','MPG2DEC_CODEC.IMG',
     'MJPGDEC.IMG','DIVX3DEC.IMG','JPGDEC.IMG','JPGENC_CODEC.IMG']

def run(cmd):return subprocess.run(cmd,check=True,capture_output=True,text=True).stdout

def audit(root):
    out={'research_only':True,'stock_objects':{},'mmpi_images':{}}
    for filename in LIBS:
        p=next(root.rglob(filename),None)
        if p is None:continue
        symbols=[];unresolved=[]
        for l in run(['readelf','-sW',str(p)]).splitlines():
            m=re.match(r'\s*\d+:\s+[0-9a-fA-F]+\s+(\d+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(.+?)\s*$',l)
            if not m:continue
            size,typ,scope,vis,section,name=m.groups()
            if section=='UND' and name:unresolved.append(name)
            if typ=='FUNC' and scope=='GLOBAL' and section!='UND':
                symbols.append({'name':name,'size':int(size),'visibility':vis})
        reloc=Counter(re.findall(r'\bR_ARM_\w+\b',run(['readelf','-rW',str(p)])))
        out['stock_objects'][filename]={
            'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),
            'elf_class':'ELF32 ARM ET_REL little-endian',
            'global_function_symbols':len(symbols),
            'unresolved':sorted(set(unresolved)),
            'relocations':dict(reloc),
            'key_functions':[s for s in symbols if s['name'].startswith(filename[3:-2].lower()+'_decoder_') or 'working_ram' in s['name']][:28]
        }
    for filename in MMP:
        p=next(root.rglob(filename),None)
        if not p:continue
        b=p.read_bytes()
        out['mmpi_images'][filename]={
            'size':len(b),'mmpi':b[:4]==b'MMPi','mmpd':b[16:20]==b'MMPd',
            'declared_size':int.from_bytes(b[4:8],'little'),
            'record_size':int.from_bytes(b[20:24],'little'),
            'raw_slot_byte':b[36] if len(b)>36 else None,
            'sha256':hashlib.sha256(b).hexdigest()
        }
    return out

if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--folder',type=pathlib.Path,required=True)
    ap.add_argument('--output',type=pathlib.Path)
    args=ap.parse_args()
    result=json.dumps(audit(args.folder),indent=2,ensure_ascii=False)
    if args.output:args.output.write_text(result,encoding='utf8')
    else:print(result)
