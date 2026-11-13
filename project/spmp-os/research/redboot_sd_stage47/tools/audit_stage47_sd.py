#!/usr/bin/env python3
"""Read-only static audit of SD command callsites from ORIGINAL 8000_MMI.RAP.
No attempt is made to flash, emulate MMIO or infer CMD17 from CMD18.
"""
from __future__ import annotations
import argparse, hashlib, json, struct
from pathlib import Path
SHA='5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
BASE=0x280000; OFFSET=0x4c
# All opcodes copied from original RAP and rechecked independently.
W={
 0x2baff4:0xe5933000,0x2baff8:0xe3530000,0x2baffc:0x03a03c02,
 0x2bb000:0x13a03001,0x2bb004:0xe3a00d56,0x2bb008:0xe2800012,
 0x2bb00c:0xe0010693,0x2bb014:0xebfffad7,
 0x2bb754:0xe3530000,0x2bb758:0x03a03c02,0x2bb75c:0x13a03001,
 0x2bb760:0xe3a00d56,0x2bb764:0xe2800012,
 0x2bb768:0xe51b103c,0x2bb76c:0xe0010193,0x2bb774:0xebfff8ff,
 0x2bbb28:0xe3a00d5e,0x2bbb2c:0xe2800019,0x2bbb38:0xebfff80e,
 0x2bbb50:0xe3a00d5e,0x2bbb54:0xe2800019,0x2bbb60:0xebfff804,
 0x2bc24c:0xe3a00d5e,0x2bc250:0xe2800019,0x2bc25c:0xebfff645,
 0x2bc27c:0xe3a00d5e,0x2bc280:0xe2800019,0x2bc28c:0xebfff639,
 0x2bae50:0xe3a00a07,0x2bae54:0xe280008c,0x2bae60:0xebfffb44,
 0x2bb290:0xe3a00a07,0x2bb294:0xe280008c,0x2bb2a0:0xebfffa34,
 0x2bbe3c:0xe3a00a07,0x2bbe40:0xe280008c,0x2bbe4c:0xebfff749,
}
# MOV + ADD + BL immediate to command sender. The bits above low six are
# not fully documented: DON'T feed these into unverified hardware.
CALLS={
 'CMD18': [(0x2bb004,0x2bb008,0x2bb014,5504,18),
           (0x2bb760,0x2bb764,0x2bb774,5504,18)],
 'CMD25': [(0x2bbb28,0x2bbb2c,0x2bbb38,6016,25),
           (0x2bbb50,0x2bbb54,0x2bbb60,6016,25),
           (0x2bc24c,0x2bc250,0x2bc25c,6016,25),
           (0x2bc27c,0x2bc280,0x2bc28c,6016,25)],
 'CMD12': [(0x2bae50,0x2bae54,0x2bae60,28672,140),
           (0x2bb290,0x2bb294,0x2bb2a0,28672,140),
           (0x2bbe3c,0x2bbe40,0x2bbe4c,28672,140)],
}

def word(data:bytes,pc:int)->int:
    o=OFFSET+pc-BASE
    if o<0 or o+4>len(data): raise ValueError(f'address outside RAP {pc:#x}')
    return struct.unpack_from('<I',data,o)[0]

def branch_target(pc:int,opcode:int)->int:
    if (opcode&0x0f000000)!=0x0b000000:raise ValueError(f'not ARM BL: {pc:#x}')
    v=opcode&0xffffff
    if v&0x800000:v-=1<<24
    return (pc+8+4*v)&0xffffffff

def check(data:bytes)->dict:
    if hashlib.sha256(data).hexdigest()!=SHA:raise ValueError('RAP SHA mismatch')
    if data[:4]!=b'MMPi' or data[0x34:0x38]!=b'MMPs':raise ValueError('RAP layout mismatch')
    for pc,expected in W.items():
        if word(data,pc)!=expected:raise ValueError(f'ARM opcode mismatch at {pc:#x}')
    output=[]
    for name, entries in CALLS.items():
        for mov,add,bl,imm,delta in entries:
            # All currently tracked MOV and ADD encodings end up in r0.
            # Instruction words are pinned by W and these constants.
            cmdword=imm+delta
            if cmdword&63!=int(name[3:]):raise ValueError('bad command index')
            target=branch_target(bl,word(data,bl))
            if target!=0x2b9b78:raise ValueError('command dispatch target wrong')
            output.append({'command':name,'cmdword_hex':hex(cmdword),
                           'mov_pc':hex(mov),'add_pc':hex(add),
                           'call_pc':hex(bl),'dispatch_pc':hex(target),
                           'confidence':'pinned original instructions and BL target'})
    # The apparently unrecognized ARM instructions are MUL r1,r3,r6 and MUL r1,r3,r1.
    if word(data,0x2bb00c)!=0xe0010693 or word(data,0x2bb76c)!=0xe0010193:
        raise ValueError('address multiplication changed')
    return {
      'source_sha256':SHA,'verified_arm_words':len(W),
      'verified_dispatch_calls':len(output), 'calls':output,
      'address_scaling':[
       {'context':'CMD18 first path','selector_pc':'0x2baff8','equal_zero_word_pc':'0x2baffc',
        'not_equal_word_pc':'0x2bb000','mul_pc':'0x2bb00c','factors':[512,1],
        'interpretation':'likely SDSC byte vs SDHC block addressing; condition variable type not proven'},
       {'context':'CMD18 second path','equal_zero_word_pc':'0x2bb758',
        'not_equal_word_pc':'0x2bb75c','mul_pc':'0x2bb76c','factors':[512,1],
        'interpretation':'same dual scaling'}],
      'cmd17_status':'UNPROVEN: not reconstructed from this audited command region',
      'safety':{'mmio_access':False,'can_flash':False,'card_initialization_proven':False,
                'early_sd_backend_proven':False,'validated_fallback_on_board':False}
    }

if __name__=='__main__':
    ap=argparse.ArgumentParser()
    ap.add_argument('rap',type=Path)
    ap.add_argument('--out',type=Path,required=True)
    a=ap.parse_args()
    d=check(a.rap.read_bytes())
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(d,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Stage47 audit OK:',d['verified_arm_words'],'words,',len(d['calls']),'calls')
