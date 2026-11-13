#!/usr/bin/env python3
"""Read-only, exact-build audit of eCos/GUI thread creation in OEM MMI RAP.

    Does NOT execute firmware or alter RAP, Ghidra databases, NAND, RedBoot.
    Thread names are decoded from literal loads immediately before verified BLs.
    No blind function-pointer enumeration or speculative MMIO writes.
"""
import argparse
import csv
import hashlib
import json
import re
import struct
from pathlib import Path

KNOWN_SHA256 = '5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
SEGMENT_BASE = 0x00280000
SEGMENT_OFFSET = 0x4c
SEGMENT_LEN = 3059088
CREATE_ADDR = 0x00339aac
KEY_BOOT = (0x28013c, 0x280140, 0x280144, 0x280150, 0x297cf4, 0x297cf8, 0x297cfc, 0x297d00)

class Firmware:
    def __init__(self, data: bytes, verify=True):
        self.original_sha256 = hashlib.sha256(data).hexdigest()
        if verify and self.original_sha256 != KNOWN_SHA256:
            raise ValueError('Unexpected OEM RAP SHA256: no static addresses may be trusted')
        self.segment = data[SEGMENT_OFFSET:SEGMENT_OFFSET + SEGMENT_LEN]
        if len(self.segment) != SEGMENT_LEN or data[0:4] != b'MMPi' or data[0x34:0x38] != b'MMPs':
            raise ValueError('Unexpected OEM MMP/RAP format/segment')

    def u32(self, addr):
        idx = addr - SEGMENT_BASE
        if addr % 4 or idx < 0 or idx + 4 > len(self.segment):
            return None
        return struct.unpack_from('<I', self.segment, idx)[0]

    def cstring(self, addr, limit=100):
        idx = addr - SEGMENT_BASE
        if idx < 0 or idx >= len(self.segment):
            return None
        s = self.segment[idx:idx+limit]
        term = s.find(b'\x00')
        if term < 3 or term >= limit:
            return None
        s = s[:term]
        if any(ch < 32 or ch > 126 for ch in s):
            return None
        return s.decode('ascii')

    def bl(self, addr):
        word = self.u32(addr)
        if word is None or ((word >> 24) & 15) != 0xB or ((word >> 28) & 15) == 15:
            return None
        off = word & 0xffffff
        if off & 0x800000:
            off -= 0x1000000
        return (addr + 8 + (off << 2)) & 0xffffffff

    def pc_literal(self, addr, register=None):
        w = self.u32(addr)
        if w is None or (w >> 26) & 3 != 1 or (w >> 25) & 1 != 0:
            return None
        if ((w>>24)&1)!=1 or ((w>>22)&1)!=0 or ((w>>21)&1)!=0 or ((w>>20)&1)!=1:
            return None
        if ((w>>16)&15)!=15:
            return None
        rd=(w>>12)&15
        if register is not None and rd!=register:
            return None
        literal_address=addr+8+(w&0xfff)*(1 if (w>>23)&1 else -1)
        value=self.u32(literal_address)
        if value is None:
            return None
        return {'instruction':hex(addr),'literal_address':hex(literal_address),'value':hex(value),'register':rd}

    def preceding_literal(self, callsite, reg, max_back=32):
        for addr in range(callsite - 4, callsite-max_back-1,-4):
            v=self.pc_literal(addr,reg)
            if v is not None:
                return v
        return None

    def call_sites(self, target):
        return [SEGMENT_BASE+i for i in range(0,len(self.segment)-3,4) if self.bl(SEGMENT_BASE+i)==target]


def produce_records(fw):
    records=[]
    for site in fw.call_sites(CREATE_ADDR):
        r1=fw.preceding_literal(site,1)
        r3=fw.preceding_literal(site,3)
        name=fw.cstring(int(r3['value'],16)) if r3 else None
        entry=int(r1['value'],16) if r1 else None
        if entry is not None and not (SEGMENT_BASE<=entry<SEGMENT_BASE+SEGMENT_LEN):
            entry=None
        records.append({
            'callsite':f'0x{site:08X}',
            'entrypoint':f'0x{entry:08X}' if entry is not None else '',
            'thread_name':name or '',
            'name_status':'verified_lit_ascii' if name else 'runtime_or_unresolved',
            'entry_load':r1['instruction'] if r1 else '',
            'name_load':r3['instruction'] if r3 else '',
            'name_literal_address':r3['literal_address'] if r3 else '',
            'name_address':r3['value'] if r3 else ''
        })
    return records


def analyze(fw):
    records=produce_records(fw)
    boot={f'0x{x:08X}':f'0x{fw.bl(x):08X}' if fw.bl(x) else None for x in KEY_BOOT}
    # Additional hard checks keep stage-specific conclusions tethered to the OEM code.
    checks={
        'reset_branch_to_0x297CE8':fw.bl(0x280150)==0x297ce8,
        'bootstrap_calls_Demo_GUI_constructor':fw.bl(0x297cfc)==0x2807c8,
        'Demo_GUI_thread_create':fw.bl(0x280810)==CREATE_ADDR,
        'Demo_GUI_entry_literal':fw.u32(0x280834)==0x3be5b0,
        'Demo_GUI_name_literal':fw.u32(0x280838)==0x4cfb44,
        'Demo_GUI_name':fw.cstring(0x4cfb44)=='Demo GUI',
        'initial_thread_priority_25':fw.u32(0x280800)==0xe3a00019,
        'initial_thread_stack_bytes_0x4000':fw.u32(0x2807e8)==0xe3a03901,
        'boot_noop_0x297d04':fw.u32(0x297d04)==0xe12fff1e,
        'boot_noop_0x297d08':fw.u32(0x297d08)==0xe12fff1e,
        'sys_init_entry_0x447540':any(r['thread_name']=='Systen initial Thread' and r['entrypoint']=='0x00447540' for r in records),
        'game_entry_0x460ab0':any(r['thread_name']=='Game thread' and r['entrypoint']=='0x00460AB0' for r in records)
    }
    if not all(checks.values()):
        raise AssertionError('OEM code has changed or unexpected decoding: '+str(checks))
    return {'source_sha256':fw.original_sha256,'static_call_sites':len(records),'resolved_names':sum(bool(x['thread_name']) for x in records),
        'unique_names':len(set(x['thread_name'] for x in records if x['thread_name'])),
        'confirmed_checks':checks,'boot_direct_calls':boot,'thread_create_target':f'0x{CREATE_ADDR:08X}','threads':records,
        'limitations':['This is a static, exact-binary ARM callsite/literal analysis, NOT a runtime trace.',
        'Thread names are recovered only when the argument is a nearby PC-relative string literal.',
        'Entry literal loads cannot prove all later register modifications; inspect Ghidra before instrumenting.',
        'Function 0x00339AAC is a wrapper that forwards to 0x0033A2DC; it is not a direct MMIO operation.',
        'Ghidra labels in Stage 53 are discovery hints, not confirmed semantic symbols.']}


def output_files(result, outdir):
    outdir.mkdir(parents=True,exist_ok=True)
    (outdir/'thread_creation_35.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    with (outdir/'thread_creation_35.csv').open('w',newline='',encoding='utf-8') as file:
        w=csv.DictWriter(file,fieldnames=['callsite','thread_name','entrypoint','name_status','entry_load','name_load','name_literal_address','name_address'])
        w.writeheader();w.writerows(result['threads'])
    with (outdir/'ghidra_priority_entries.csv').open('w',newline='',encoding='utf-8') as file:
        w=csv.writer(file)
        w.writerow(['entrypoint','suggested_function_name','source_callsite','oem_thread_name','evidence_level'])
        for r in result['threads']:
            if r['thread_name'] and r['entrypoint']:
                sanitized=re.sub('[^A-Za-z0-9_]','_',r['thread_name'])
                w.writerow([r['entrypoint'],'MMI_thread_'+sanitized[:60],r['callsite'],r['thread_name'],'LDR literals at verified BL'])
    (outdir/'stage54_summary.json').write_text(json.dumps({k:v for k,v in result.items() if k!='threads'},indent=2,ensure_ascii=False)+'\n',encoding='utf-8')


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--rap',type=Path,required=True,help='Original, unmodified 8000_MMI.RAP')
    ap.add_argument('--output',type=Path,required=True)
    a=ap.parse_args()
    fw=Firmware(a.rap.read_bytes())
    result=analyze(fw)
    output_files(result,a.output)
    print(f"Confirmed: {result['static_call_sites']} direct BL callsites, {result['resolved_names']} static names, {len(result['confirmed_checks'])} hard code checks")

if __name__=='__main__':main()
