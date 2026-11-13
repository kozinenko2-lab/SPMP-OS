#!/usr/bin/env python3
"""Read-only, exact-version static evidence for OEM SD_P430 status polling.
Only the supplied RAP is read. No hardware register access, no binary patching.
"""
from pathlib import Path
import argparse, hashlib, json, struct
SHA="5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e"
BASE=0x280000
OFF=0x4c
# Exact instructions in the OEM ARM segment, confirmed by llvm-objdump.
CHECK={
  0x2b9a38:0xe1a0c00d, 0x2b9a48:0xe59f3108,
  0x2b9a4c:0xe5933000, 0x2b9a50:0xe1100003,
  0x2b9a5c:0xe3a01cc3, 0x2b9a60:0xe2811050,
  0x2b9a68:0xe5943000, 0x2b9a6c:0xe3530000,
  0x2b9a74:0xe2822001, 0x2b9a78:0xe1520001,
  0x2b9a7c:0x8a000002, 0x2b9a80:0xe59c3000,
  0x2b9a84:0xe1100003, 0x2b9a88:0x0afffff6,
  0x2b9ac8:0xe59f2088, 0x2b9ad0:0xe3130e63,
  0x2b9ad4:0x03a00000, 0x2b9ad8:0x089da810,
  0x2b9b28:0xe3a00e42, 0x2b9b2c:0xe89da810,
  0x2b9b58:0x92b0b014, 0x2b9b5c:0x00555f18,
  0x2b9b90:0xe59f707c, 0x2b9b94:0xe3a03cff,
  0x2b9b98:0xe28330ff, 0x2b9b9c:0xe5873000,
  0x2b9ba0:0xe3a032c9, 0x2b9ba4:0xe283362b,
  0x2b9ba8:0xe2833a0b, 0x2b9bac:0xe5831000,
  0x2b9bb0:0xe3a03289, 0x2b9bb4:0xe283362b,
  0x2b9bb8:0xe2833a0b, 0x2b9bbc:0xe5830000,
  0x2b9bc0:0xe3a00004, 0x2b9c70:0xe3a00040,
  0x2ba490:0xe3a00004,
  0x2b9bc4:0xebffff9b, 0x2b9c74:0xebffff6f,
  0x2b9c98:0xebffff66, 0x2ba494:0xebfffd67,
}
def word(data,addr):
    start=OFF+addr-BASE
    if start<0 or start+4>len(data):raise ValueError("outside the embedded ARM image")
    return struct.unpack_from('<I',data,start)[0]
def branch_target(addr,opcode):
    if opcode>>24&0xf!=0xb:raise ValueError(f"Not BL at {addr:#x}")
    d=(opcode&0xffffff)<<2
    if d & 0x2000000:d-=0x4000000
    return (addr+8+d)&0xffffffff
def audit(data):
    if hashlib.sha256(data).hexdigest()!=SHA:
        raise ValueError('Unsupported RAP; SHA-256 differs from stock reference')
    if data[:4]!=b'MMPi' or data[0x34:0x38]!=b'MMPs':
        raise ValueError('Wrong segment layout')
    for addr,opcode in CHECK.items():
        found=word(data,addr)
        if found!=opcode:
            raise ValueError(f'Machine instruction mismatch {addr:#x}: {found:#x}')
    callsites=[]
    for c,mask,conf in [
       (0x2b9bc4,4,'immediate mov'),
       (0x2b9c74,0x40,'immediate mov'),
       (0x2b9c98,4,'add #4 after successful poll returned 0'),
       (0x2ba494,4,'immediate mov')]:
        if branch_target(c,word(data,c))!=0x2b9a38:
            raise ValueError('Poll helper call no longer points to expected routine')
        callsites.append(dict(bl=f'0x{c:08X}',mask=f'0x{mask:X}',evidence=conf))
    # Do not turn this into a writable MMIO register specification.
    return {
      'rap_sha256':SHA,'firmware':'Defender stock 8000_MMI.RAP',
      'status_poll_function':'0x002B9A38',
      'poll_status_source':{'literal_pc':'0x002B9B58','value':'0x92B0B014','observed':'32-bit LDR, TST input mask; read only in this function','status':'candidate register with confirmed instruction load; hardware ownership untested'},
      'oem_counter':{'count_immediate_1':49920,'count_immediate_2':80,'threshold':50000,'loop_initial_count':0,'branch_condition':'unsigned higher than 50000','maximum_loop_iterations':50001,'not_wallclock_time':True},
      'error_bit_tests':{'aggregate_mask':'0x630','individual_masks':['0x10','0x20','0x200','0x400']},
      'oem_return_values':{'no_reported_error_bits':'0','nonzero_status_error_mask':'0x420 (1056)','software_abort_gate':'0x405 (1029)'},
      'known_gap':'After budget exhausted, OEM code may return 0 if 0x630 bits are clear, even when the desired completion bit was not observed. Never equate OEM return-zero with observed completion in a new logger.',
      'software_gate':{'literal_pc':'0x002B9B5C','global_address':'0x00555F18','effect':'nonzero global short-circuits poll and is zeroed; not a hardware register'},
      'verified_callers':callsites,
      'command_setup':{
        'function':'0x002B9B78',
        'observed':[
          {'pc':'0x002B9B9C','effect':'STR 0xFFFF to 0x92B0B014 (status/ack-related address, semantics unverified)'},
          {'pc':'0x002B9BAC','effect':'STR caller r1 to address constructed as 0x92B0B00C'},
          {'pc':'0x002B9BBC','effect':'STR caller r0 to address constructed as 0x92B0B008'},
          {'pc':'0x002B9BC4','effect':'BL status poll with mask=0x04'},
          {'pc':'0x002B9BF4','effect':'LDR from 0x92B0B010 on nonerror path'}
        ],
        'critical_warning':'Static analysis only; no MMIO writes permitted by Stage45 firmware or probe code.'
      },
      'opcodes':[{'pc':f'0x{a:08X}','word':f'0x{w:08X}'} for a,w in sorted(CHECK.items())],
      'hardware_accessed_by_this_tool':False,
      'unsupported':['No SD read-sector implementation','No verified physical SD/MMC transaction protocol','No write to NAND or SD performed','No RedBoot/MMI patched']
    }
def main():
    p=argparse.ArgumentParser()
    p.add_argument('rap',type=Path)
    p.add_argument('--out',type=Path,required=True)
    args=p.parse_args()
    result=audit(args.rap.read_bytes())
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print('Stage45: verified',len(CHECK),'original words and',len(result['verified_callers']),'call sites')
if __name__=='__main__':main()
