#!/usr/bin/env python3
"""Pack source-built research ELF as experimental kind-6 RAM NGame application.
Not a Flash/NAND image. No private game header, assets or original firmware bytes.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from startup_elf import StartupELF,ENTRY
from ngame_inspect import transform,inspect,FOOTER

def pack(elf_bytes,title="SPMP Probe"):
    title_bytes=title.encode("ascii")
    if not 1<=len(title_bytes)<=15 or any(b<32 or b>126 for b in title_bytes):raise ValueError("Title must be 1..15 printable ASCII bytes")
    image=StartupELF(elf_bytes)
    footer_start=image.address('__reserved_footer_start__');footer_end=image.address('__reserved_footer_end__')
    payload=bytearray(footer_end-ENTRY)
    for s in image.segments:
        begin=s['address']-ENTRY;end=begin+s['file_bytes']
        if end>image.address('__file_image_end__')-ENTRY:raise ValueError('File-backed bytes exceed linker content span')
        payload[begin:end]=elf_bytes[s['offset']:s['offset']+s['file_bytes']]
    payload[footer_start-ENTRY:footer_start-ENTRY+len(FOOTER)]=FOOTER
    plain=bytes(payload);prefix=min(len(payload),1024)
    if prefix%8:raise ValueError('Partial cipher block')
    payload[:prefix]=transform(payload[:prefix],mode=0)
    header=bytearray(128);header[:8]=b'NGame1.0';struct.pack_into('<I',header,8,128)
    # Own metadata. Fields outside the traced loader contract remain zero.
    header[0x2c:0x2c+len(title_bytes)]=title_bytes;header[0x6d]=5
    data=bytes(header)+bytes(payload)
    result=inspect(data)
    if result['status']!='structural_match':raise AssertionError('Packed NGame failed structural self-check')
    decoded=bytearray(data[128:]);decoded[:prefix]=transform(decoded[:prefix])
    if bytes(decoded)!=plain:raise AssertionError('NGame transform round-trip failed')
    report={'status':'EXPERIMENTAL_RAM_NGAME_VM_ONLY','source_elf_sha256':hashlib.sha256(elf_bytes).hexdigest(),
        'container_sha256':hashlib.sha256(data).hexdigest(),'payload_sha256':hashlib.sha256(plain).hexdigest(),
        'container_bytes':len(data),'payload_bytes':len(plain),'header_bytes':128,'tail_bytes':0,
        'lcm_index':5,'entry':hex(ENTRY),'runtime_end':hex(image.bss_end),'bss_bytes':image.bss_end-image.bss_start,
        'structural_check':result,
        'limits':['Own header; no private game header or assets copied.',
                  'LCM byte 5 is observed in user working game, not a verified universal panel descriptor.',
                  'Top-level loader structural acceptance does not certify frontend metadata, hardware or RTOS behavior.',
                  'RAM application, no Flash/NAND image or write path.']}
    return data,plain,report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('elf',type=Path);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--manifest',type=Path,required=True);p.add_argument('--title',default='SPMP Probe');a=p.parse_args()
    data,plain,report=pack(a.elf.read_bytes(),a.title);a.output.write_bytes(data);a.manifest.write_text(json.dumps(report,indent=2)+'\n')
    print('PASS: source-built NGame,',len(data),'bytes; structural/decode round-trip only')
