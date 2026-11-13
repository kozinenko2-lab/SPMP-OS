#!/usr/bin/env python3
"""Read-only, SHA-pinned recovery of ARM SD dispatch command callsites.
Never executes firmware, MMIO, changes an image, or emits a flash patch.
"""
from __future__ import annotations
import argparse, hashlib, json, struct
from collections import Counter
from pathlib import Path

OEM_RAP_SHA256 = '5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
ROM_BASE, ROM_OFFSET = 0x280000, 0x4c
SENDER = 0x2b9b78
SCAN_LO, SCAN_HI = 0x2b8000, 0x2bd500
# Independently classified from ARM MOV/ADD and low-six command field.
# ACMD6 and ACMD41 require CMD55, but this file does not assert that a
# particular card responds successfully on real hardware.
EXPECTED = {
 0x2ba4e8:('CMD0',0), 0x2ba52c:('CMD8',8),
 0x2ba578:('CMD55',55), 0x2ba5a0:('ACMD41',41), 0x2ba5b8:('ACMD41',41),
 0x2ba668:('CMD3',3), 0x2ba6b4:('CMD13',13), 0x2ba818:('CMD13',13),
 0x2ba850:('CMD7',7), 0x2ba948:('CMD55',55), 0x2ba95c:('ACMD6',6),
 0x2ba9a0:('CMD13',13), 0x2bac60:('CMD6',6),
 0x2bae60:('CMD12',12), 0x2bb014:('CMD18',18),
 0x2bb21c:('CMD13',13), 0x2bb2a0:('CMD12',12), 0x2bb2c8:('CMD12',12),
 0x2bb774:('CMD18',18), 0x2bb8d4:('CMD13',13),
 0x2bb988:('CMD12',12), 0x2bbb38:('CMD25',25),
 0x2bbb60:('CMD25',25), 0x2bbdc8:('CMD13',13),
 0x2bbe4c:('CMD12',12), 0x2bbe74:('CMD12',12),
 0x2bc25c:('CMD25',25), 0x2bc28c:('CMD25',25),
 0x2bc3e8:('CMD13',13), 0x2bc500:('CMD12',12),
}

def word(raw:bytes,pc:int)->int:
    i=ROM_OFFSET+pc-ROM_BASE
    if i<0 or i+4>len(raw):raise ValueError(f'address {pc:08x} out of RAP')
    return struct.unpack_from('<I',raw,i)[0]

def bl_target(pc:int,w:int)->int|None:
    if (w&0x0f000000)!=0x0b000000:return None
    o=w&0xffffff
    if o&0x800000:o-=1<<24
    return (pc+8+o*4)&0xffffffff

def imm_operand(w:int)->int:
    i=w&0xff; rot=((w>>8)&0xf)*2
    return ((i>>rot)|(i<<(32-rot)))&0xffffffff if rot else i

def command_value(raw:bytes,pc:int)->tuple[int,list[dict]]:
    # Short local symbolic propagation. Reset r0 after unrelated BL.
    regs={0:None,5:None}
    operations=[]
    for p in range(pc-64,pc,4):
        w=word(raw,p)
        if bl_target(p,w) is not None:
            regs[0]=None
            continue
        if ((w>>25)&7)!=1:continue   # data-processing immediate
        op=(w>>21)&0xf; rd=(w>>12)&0xf; rn=(w>>16)&0xf
        if rd not in regs:continue
        val=None
        if op==13:val=imm_operand(w) # MOV immediate
        elif op==4 and regs.get(rn) is not None:val=(regs[rn]+imm_operand(w))&0xffffffff
        elif op==2 and regs.get(rn) is not None:val=(regs[rn]-imm_operand(w))&0xffffffff
        if val is None:regs[rd]=None;continue
        regs[rd]=val
        operations.append({'pc':hex(p),'opcode':hex(w),'register':f'r{rd}',
                           'operation':{13:'mov',4:'add',2:'sub'}.get(op,'other'),
                           'value':hex(val),'condition':(w>>28)&15})
    if regs[0] is None:raise ValueError(f'cannot recover command register at {pc:#x}')
    return regs[0],operations

def analyze(raw:bytes)->dict:
    if hashlib.sha256(raw).hexdigest()!=OEM_RAP_SHA256:raise ValueError('original RAP SHA-256 mismatch')
    if raw[:4]!=b'MMPi' or raw[0x34:0x38]!=b'MMPs':raise ValueError('RAP segment header mismatch')
    direct=[]
    for pc in range(SCAN_LO,SCAN_HI,4):
        w=word(raw,pc)
        if bl_target(pc,w)==SENDER:
            direct.append(pc)
    if set(direct)!=set(EXPECTED):
        extra=set(direct)-set(EXPECTED);missing=set(EXPECTED)-set(direct)
        raise ValueError(f'dispatch callsite set changed: extra={sorted(map(hex,extra))}, missing={sorted(map(hex,missing))}')
    calls=[]
    for pc in direct:
        name,num=EXPECTED[pc];cmd,ops=command_value(raw,pc)
        if (cmd&63)!=num:raise ValueError(f'command mismatch at {pc:#x}: {cmd:#x} vs {name}')
        if not ops or not any(x['register']=='r0' for x in ops):raise ValueError(f'no r0 constructor at {pc:#x}')
        calls.append({'pc':hex(pc),'dispatch':hex(SENDER),'dispatch_word':hex(word(raw,pc)),
                      'classification':name,'command_index':num,'oem_command_word':hex(cmd),
                      'constructors':ops,'confidence':'verified original SHA, direct BL and local r0 immediate construction'})
    counts=Counter(x['classification'] for x in calls)
    return {'source_sha256':OEM_RAP_SHA256,'verified_dispatch_calls':len(direct),
      'scan_range':[hex(SCAN_LO),hex(SCAN_HI)],'command_counts':dict(sorted(counts.items())),
      'calls':calls,'cmd17_direct_calls_in_scanned_range':sum(x['command_index']==17 for x in calls),
      'limitations':['CMD17 may exist through another dispatcher or indirect call',
        'OEM command upper bits are not decoded into hardware flags',
        'no SD card, DMA, pinmux or register ownership was tested on device',
        'no executable redboot/MMI patches or device-level logging are included']}

if __name__=='__main__':
    ap=argparse.ArgumentParser(); ap.add_argument('rap',type=Path);ap.add_argument('--out',type=Path)
    a=ap.parse_args();res=analyze(a.rap.read_bytes())
    if a.out:
        a.out.parent.mkdir(parents=True,exist_ok=True)
        a.out.write_text(json.dumps(res,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Stage48 OK:',res['verified_dispatch_calls'],'SD commands',res['command_counts'])
