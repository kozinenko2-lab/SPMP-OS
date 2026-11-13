#!/usr/bin/env python3
"""Read-only exact-build audit of boot policy, customization and MMI event contracts."""
import argparse,hashlib,json
from pathlib import Path
from audit_boot import audit,Image,H

def produce(root):
    baseline=audit(root)
    raw=(root/'firmware/originals/Images_RedBoot_201000427.img').read_bytes()
    v=baseline['redboot_variants'][2];start=int(v['source_offset'],16)
    rb=Image(raw[start:start+v['bytes']]);mmi=Image((root/'firmware/originals/8000_MMI.RAP').read_bytes())
    checks=[]
    def word(im,program,addr,expected):
        actual=im.word(addr)
        if actual!=expected:raise ValueError(f'{program} instruction mismatch at {addr:#x}')
        segment=next(s for s in im.s if s['address']<=addr<s['address']+s['size'])
        offset=segment['offset']+addr-segment['address']
        checks.append({'program':program,'address':hex(addr),'source_offset':hex(offset),'word':f'{actual:08x}'})
    for addr,expected in {
        0x45e68:0xe3130010,0x45e7c:0xe3130001,0x45eb0:0xe3130002,
        0x45ed0:0xe3130004,0x45ef0:0xe3130101,0x45f10:0xe3130080,
        0x45f2c:0xe3130202,0x33f0c:0xe1a001a0,0x33f10:0xe2000001,
        0x46b70:0xe3540000,0x4681c:0xe2504000,
        0x40ab0:0xe3530001,0x40abc:0xe3530001,0x40ac8:0xe3530028,
        0x40ad4:0xe3530001,0x410bc:0xe0832007,
        0x41464:0xe3c334ff,0x4146c:0xe3100402,0x4147c:0xe061300c,
        0x41484:0xe1a03303,0x41488:0xe1822423,0x4149c:0xe0833002,
    }.items():word(rb,'redboot3',addr,expected)
    if rb.literal(0x40aa0)!=0x464c457f:raise ValueError('ELF magic mismatch')
    for addr,stage in [(0x46974,0),(0x469cc,1),(0x46ad0,2),(0x46d24,3)]:
        if rb.branch(addr)!=0x36844:raise ValueError('Customization callback mismatch')
        word(rb,'redboot3',addr-4,0xe3a00000|stage)
    reloc=[]
    for typ in range(1,30):
        target=rb.word(0x413e8+(typ-1)*4)
        expected=0x4145c if typ in (1,28,29) else 0x41498 if typ==2 else 0x414ac
        if target!=expected:raise ValueError('Relocation dispatch table mismatch')
        reloc.append({'type':typ,'target':hex(target),'supported':typ in (1,2,28,29)})
    symbols=[]
    expected_names=['diag_printf','GetCxRefClockFreq','__udivsi3','cyg_thread_delay','setLcmSWRotationType']
    if rb.word(0x1798c)!=5:raise ValueError('External symbol count mismatch')
    for i,name in enumerate(expected_names):
        at=0x17990+8*i;pointer=rb.word(at)
        if rb.string(pointer)!=name:raise ValueError('External symbol name mismatch')
        symbols.append({'name':name,'address':hex(rb.word(at+4)),'table_entry':hex(at)})
    for addr,expected in {
        0x447564:0xe3a01001,0x447568:0xe3a02003,
        0x2d2d2c:0xe3a01001,0x2d2d58:0xe3a01001,0x2d2d84:0xe3a01001,
        0x44750c:0xe2844001,0x4474e8:0xe3a0500a,0x44751c:0xe1540005,
        0x33ceb4:0xe3140002,0x33ceec:0xe3140001,
        0x33cef0:0x13a03000,0x33cef4:0x15863000,
    }.items():word(mmi,'mmi',addr,expected)
    for call in [0x2d2d30,0x2d2d5c,0x2d2d88]:
        if mmi.branch(call)!=0x339f3c:raise ValueError('Completion event producer mismatch')
    for load in [0x2d2d28,0x2d2d54,0x2d2d80,0x447560]:
        if mmi.literal(load)!=0x68e4c8:raise ValueError('Event address mismatch')
    for call,target in [(0x44756c,0x339f64),(0x44757c,0x2d1b28),
                        (0x44758c,0x4474d8),(0x4475a4,0x447458),(0x4475a8,0x339b04)]:
        if mmi.branch(call)!=target:raise ValueError('System init call mismatch')
    return {'source_hashes':H,'redboot3_sha256':v['sha256'],'instruction_checks':checks,
        'relocation_dispatch':reloc,'initial_module_import_symbols':symbols,
        'mmi_completion_event':{'address':'0x68e4c8','mask':1,'wait_mode':3,
          'producer':'0x2d2c28','set_on_two_error_returns':True,'set_on_normal_return':True},
        'mode_bits':{'clear_all_on_match':1,'or_instead_of_and':2},
        'limits':['Exact supplied builds only; no runtime trace.',
                  'Symbol table is the initial mapped table, not proof of later runtime contents.',
                  'C reconstructions are host models, not target-callable firmware.']}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('root',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=produce(a.root.resolve());a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',len(result['instruction_checks']),'ARM checks, 29 relocation table entries, 5 import symbols, MMI completion contract')
