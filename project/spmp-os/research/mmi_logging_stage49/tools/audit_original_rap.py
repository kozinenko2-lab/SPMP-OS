#!/usr/bin/env python3
"""Read-only, exact-hash audit of user's original 8000_MMI.RAP.
No modified firmware image or patch is generated.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

SHA = '5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
PAYLOAD_FILE = 0x4c
BASE = 0x280000
SEGMENT_SIZE = 3059088

def analyse(data):
    sha = hashlib.sha256(data).hexdigest()
    if sha != SHA:
        raise ValueError(f'Expected exact original RAP SHA-256 {SHA}; got {sha}. Refusing analysis.')
    if data[:4] != b'MMPi' or struct.unpack_from('<I', data, 4)[0] != len(data):
        raise ValueError('Bad RAP envelope')
    if data[0x34:0x38] != b'MMPs' or struct.unpack_from('<I', data, 0x3c)[0] != BASE:
        raise ValueError('First ARM payload segment differs from known model')
    if struct.unpack_from('<I', data, 0x28)[0] != 0x280040:
        raise ValueError('Reset entry differs')
    def offset(va):
        off=PAYLOAD_FILE + va - BASE
        if not PAYLOAD_FILE <= off <= PAYLOAD_FILE + SEGMENT_SIZE - 4:
            raise ValueError(f'VM address outside first segment: {va:#x}')
        return off
    def u32(va):return struct.unpack_from('<I',data,offset(va))[0]
    checks={
        0x298B64:0xE12FFF1E,  # bx lr -- a NO-OP logging stub
        0x298B68:0xE92D000F,  # push {r0-r3}, adjacent actual formatter
        0x298B6C:0xE92DD830,
        0x284100:0xE3A00000,  # OEM FAT fsync returns zero without work
        0x284104:0xE12FFF1E,
        0x33E4D4:0xE59F0108,  # load /dev/ttydiag path
        0x33E4E4:0xEB000163,  # opening tty device path
        0x2814DC:0xE1A0C00D,
    }
    for va,expected in checks.items():
        actual=u32(va)
        if actual!=expected:
            raise ValueError(f'Opcode mismatch at {va:#x}: {actual:#x}, expected {expected:#x}')
    p=lambda name: data.find(name)
    paths={}
    for name in (b'/dev/ttydiag',b'/dev/haldiag',b'/fat20a0',b'/hda1'):
        off=p(name)
        if off<0:raise ValueError(f'Path missing: {name!r}')
        paths[name.decode()]={'file_offset':f'0x{off:08X}','virtual':f'0x{BASE + off - PAYLOAD_FILE:08X}'}
    if u32(0x33E5E4)!=int(paths['/dev/ttydiag']['virtual'],16):
        raise ValueError('ttydiag pointer at 0x33E5E4 changed')
    if u32(0x56A788)!=int(paths['/dev/haldiag']['virtual'],16):
        raise ValueError('haldiag pointer at 0x56A788 changed')
    diag_bl=[]
    for off in range(PAYLOAD_FILE,PAYLOAD_FILE+SEGMENT_SIZE-3,4):
        w=struct.unpack_from('<I',data,off)[0]
        if (w&0xff000000)!=0xEB000000:continue
        imm=w&0xffffff
        if imm&0x800000:imm-=0x1000000
        site=BASE+off-PAYLOAD_FILE
        dest=site+8+imm*4
        if dest==0x298B64:diag_bl.append(site)
    if len(diag_bl)!=4559:
        raise ValueError(f'BL references changed (expected 4559; found {len(diag_bl)})')
    return {
        'original_sha256':sha,'original_size':len(data),'firmware_unchanged':True,
        'redboot_modified':False,'rap_modified':False,
        'first_segment_virtual_base':f'0x{BASE:08X}',
        'reset_entry':'0x00280040',
        'no_op_diag_stub':{'virtual_address':'0x00298B64','file_offset':f'0x{offset(0x298B64):08X}',
           'arm_instruction':'BX LR','direct_BL_call_sites':len(diag_bl),
           'first_20_sites':[f'0x{x:08X}' for x in diag_bl[:20]],
           'finding':'Calls to this stub return immediately: no diagnostics emitted by this path.'},
        'adjacent_formatter':{'address':'0x00298B68','initial_flag_0x00514244':u32(0x514244),
           'caution':'Adjacent formatter checks a runtime flag; enabling a BL is not enough to guarantee visible logs.'},
        'fat_fsync':{'address':'0x00284100','instructions':['MOV R0,#0','BX LR'],
           'caution':'No physical flush guarantee.'},
        'diagnostic_device_paths':paths,
        'safe_next_action':'Build/run separate native SD marker-gated probe with unchanged MMI, then integrate RAM buffer and hooks only in recoverable RAM test.',
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('rap',type=Path)
    p.add_argument('--out',type=Path)
    args=p.parse_args()
    result=analyse(args.rap.read_bytes())
    content=json.dumps(result,ensure_ascii=False,indent=2)+'\n'
    if args.out:
        args.out.write_text(content,encoding='utf-8')
        print(f'Wrote read-only evidence to {args.out}')
    else: print(content)

if __name__=='__main__':main()
