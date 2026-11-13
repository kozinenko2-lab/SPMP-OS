#!/usr/bin/env python3
"""Compare reconstructed transform with original ARM in an isolated Unicorn VM.
Optional dependency: unicorn. No MMIO or syscalls are mapped. Never runs on device.
"""
import argparse,hashlib,json,random,struct
from pathlib import Path
from audit_boot import Image,H
from ngame_inspect import transform

def emulate(blob,data,key,mode):
    from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
    from unicorn.arm_const import UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3,UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC
    im=Image(blob);u=Uc(UC_ARCH_ARM,UC_MODE_ARM)
    u.mem_map(0x280000,0x480000)
    for s in im.s:
        if s['size'] and 0x280000<=s['address']<0x700000:
            u.mem_write(s['address'],blob[s['offset']:s['offset']+s['size']])
    u.mem_map(0x1000000,0x20000);u.mem_map(0x1100000,0x20000)
    u.mem_write(0x1001000,data);u.mem_write(0x1002000,key)
    u.mem_write(0x1110000,struct.pack('<II',len(key),mode))
    for reg,value in [(UC_ARM_REG_R0,0x1000000),(UC_ARM_REG_R1,0x1001000),
                      (UC_ARM_REG_R2,len(data)),(UC_ARM_REG_R3,0x1002000),
                      (UC_ARM_REG_SP,0x1110000),(UC_ARM_REG_LR,0x1200000)]:u.reg_write(reg,value)
    u.emu_start(0x462cd0,0x1200000,timeout=30000000,count=20000000)
    if u.reg_read(UC_ARM_REG_PC)!=0x1200000 or u.reg_read(UC_ARM_REG_R0)!=1:
        raise RuntimeError('Original ARM did not complete within VM limits')
    return bytes(u.mem_read(0x1000000,len(data)))

def compare(rap):
    blob=rap.read_bytes();sha=hashlib.sha256(blob).hexdigest()
    if sha!=H['8000_MMI.RAP']:raise ValueError('Exact original RAP required')
    rng=random.Random(0x8000)
    cases=[(bytes.fromhex('0123456789abcdef'),bytes.fromhex('133457799bbcdff1'))]
    cases.extend((bytes(rng.randrange(256) for _ in range(n)),bytes(rng.randrange(256) for _ in range(8))) for n in [8,16,24,64,128,1024])
    rows=[]
    for case,(data,key) in enumerate(cases):
        for mode in (0,1):
            original=emulate(blob,data,key,mode);recovered=transform(data,key,mode)
            if original!=recovered:raise AssertionError(f'ARM/host mismatch case {case} mode {mode}')
            row={'case':case,'bytes':len(data),'mode':mode,'key_hex':key.hex(),
                 'input_sha256':hashlib.sha256(data).hexdigest(),
                 'output_sha256':hashlib.sha256(original).hexdigest(),'exact_match':True}
            if len(data)==8:row.update(input_hex=data.hex(),output_hex=original.hex())
            rows.append(row);print('PASS: original ARM vs host, case',case,'mode',mode,'bytes',len(data),flush=True)
    # A second, isolated run verifies all 85 native API slots. Only the clock
    # helper is replaced inside VM RAM; no firmware file is modified.
    from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
    from unicorn.arm_const import UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC
    from audit_stage3 import extract_native_api
    im=Image(blob);u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.mem_map(0x280000,0x480000)
    for seg in im.s:
        if seg['size'] and 0x280000<=seg['address']<0x700000:
            u.mem_write(seg['address'],blob[seg['offset']:seg['offset']+seg['size']])
    u.mem_map(0x1100000,0x20000)
    u.mem_write(0x339d18,bytes.fromhex('0000a0e3 0010a0e3 1eff2fe1'))
    u.reg_write(UC_ARM_REG_SP,0x1110000);u.reg_write(UC_ARM_REG_LR,0x1200000)
    u.emu_start(0x2d32b0,0x1200000,timeout=30000000,count=100000)
    if u.reg_read(UC_ARM_REG_PC)!=0x1200000:raise RuntimeError('API initializer did not complete')
    original_slots=struct.unpack('<85I',u.mem_read(0x5efb74,85*4))
    expected_slots=[int(x['target'],16) for x in extract_native_api(im)]
    if list(original_slots)!=expected_slots:raise AssertionError('Native API reconstruction mismatch')
    print('PASS: all 85 native API slots match original ARM initializer',flush=True)
    api_verification={'entry':'0x2d32b0','base':'0x5efb74','slots_verified':85,
       'vm_clock_stub':'0x339d18 returns r0=r1=0; VM RAM only',
       'table_sha256':hashlib.sha256(struct.pack('<85I',*original_slots)).hexdigest()}
    return {'native_api_initializer':api_verification,'source_sha256':sha,'entry':'0x462cd0','isolated_vm':'Unicorn ARM; RAM-only mappings; original crypto helpers execute from RAP',
            'max_instructions_per_case':20000000,'timeout_microseconds_per_case':30000000,
            'cases':rows,'limits':['Cipher routine only, no GUI, RTOS, MMIO or console execution.',
            'Eight-byte key path verified; 16-byte-key triple path not reconstructed.']}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=compare(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
