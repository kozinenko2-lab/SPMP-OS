#!/usr/bin/env python3
"""Read-only exact-image evidence for LCD, PCM, memory and typed probe surface."""
import argparse,csv,hashlib,json
from pathlib import Path
from audit_boot import Image,H
from audit_stage3 import extract_native_api
from audit_stage4 import CONTRACTS as ABI
from audit_stage5 import CONTRACTS as RESOURCES
EXTRA=[(0x38,'native_init_graph','mmi_init_graph_fn'),(0x54,'native_set_display_screen','mmi_set_display_origin_fn'),
       (0xa8,'native_enable_double_buffer','mmi_enable_double_buffer_fn'),(0xc4,'native_fill_rect','mmi_fill_rect_fn')]

def produce(rap):
    blob=rap.read_bytes()
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Wrong RAP SHA-256')
    im=Image(blob);checks=[]
    words={0x2e83a4:0xe1d300b6,0x2e83b4:0xe1d300b4,0x2e83c4:0xe593000c,0x2e83f4:0xe583000c,
        0x2e8400:0xe5830000,0x2e841c:0x15832000,0x2a400c:0xe5940000,0x2a4014:0xe5845000,
        0x2e47c4:0xe203303b,0x2e47c8:0xe3530010,0x2e47e4:0x9afffff5,
        0x2dd5bc:0xe1d410f0,0x2dd5c0:0xe1d420f2,0x2dd61c:0xe3a00000,
        0x2a1504:0xe2411004,0x2a1530:0xe2433004,0x2a1550:0xe78c3002,0x2a1560:0xe3130601,
        0x2c56cc:0xe1a00084,0x2c5718:0xe18350b0,0x2c5720:0xe1c120b2,0x2c5784:0xe5832000,
        0x294a64:0xe15c0001,0x294a68:0xc59f3038,
        0x2a43e4:0x2a315c,0x2a43f0:0x2a3020,
        0x55bc44:0x34c384,0x55bc48:0x34c38c,0x34c384:0xe3a00d05,0x34c38c:0xe3a000f0,
        0x514198:0x42000,0x5141b8:0x588b60,
        0x42f92c:0xe3a00601,0x42f930:0xe2800010,
        0x430a6c:0xe2833003,0x430a70:0xe3c33003,0x430a78:0xe3a03601,
        0x430a84:0xe2800001,0x430a8c:0xe3a02014}
    for address,word in words.items():
        assert im.word(address)==word,hex(address);checks.append({'address':hex(address),'word':hex(word)})
    calls=[(0x2a3fe0,0x2e83c0),(0x2a4010,0x2e83f0),(0x2a4020,0x2a3e98),
        (0x2a3ec4,0x339f50),(0x2a3ed4,0x339f64),(0x2a3170,0x339f3c),
        (0x2a4300,0x339c10),(0x2a4310,0x339cd8),(0x2dd5f0,0x313f40),(0x2dd5f4,0x2e479c),
        (0x2a1598,0x2a12b8),(0x2a2fac,0x339c10),(0x2a2fbc,0x339cd8),
        (0x2953f0,0x294f74),(0x2b9834,0x2e3724),(0x2b9890,0x2e3724),
        (0x42f934,0x2e3724),(0x42f970,0x2e3784),(0x430a90,0x2d6d60)]
    for address,target in calls:
        assert im.branch(address)==target,hex(address);checks.append({'address':hex(address),'direct_call':hex(target)})
    driver=[im.word(a) for a in range(0x557658,0x557688,4)]
    assert driver==[0x2a4460,0x2a4538,0x2e83a0,0x2e83b0,0x2a4268,0x2e83f0,0x2a3fd4,0x2a3e5c,0x2a4248,0x2e83c0,0,0]
    slots=extract_native_api(im);contracts=[]
    for row in [tuple(c[:3]) for c in ABI]+RESOURCES+EXTRA:
        offset,label,typedef=row
        contracts.append({'slot':offset//4,'offset':hex(offset),'target':slots[offset//4]['target'],
                          'export':label,'typedef':typedef,'header':'mmi_native_resources_abi.h' if row in RESOURCES else 'mmi_native_abi.h'})
    contracts.sort(key=lambda row:row['slot'])
    assert len({r['slot'] for r in contracts})==28
    base=Path(__file__).resolve().parents[1];plan=json.loads((base/'ghidra_targets.json').read_text())
    names={int(t['address'],16):t['label'] for p in plan['programs'] if p['id']=='mmi' for t in p['targets']}
    for row in contracts:
        assert names[int(row['target'],16)]==row['export']
        assert row['typedef'] in (base/'src'/row['header']).read_text()
    return {'source_sha256':H['8000_MMI.RAP'],'checks':checks,'contracts':contracts,
        'display_table':{'base':'0x557658','words':[hex(a) for a in driver]},
        'lcd_state':{'base':'0x60640c','flags_offset':0,'height_u16_offset':4,'width_u16_offset':6,'scanout_offset':12,'secondary_scanout_offset':24},
        'lcd_irq':{'vector':23,'isr':'0x2a3020','dsr':'0x2a315c','event_object':'0x5dce1c','event_bits':1},
        'game_thread_stack':{'pointer_global':'0x6769b0','allocation_bytes':'0x100010','thread_stack_bytes':'0x100000',
            'stack_base_expression':'(allocation_pointer + 3) & ~3','launch_request':'0xd01','request_bytes':20,
            'scope':'shell launcher path; stack allocation is cached, address depends on pool history'},
        'pcm_queue':{'base':'0x588b60','capacity':'0x42000','within_mmi_bss':True},
        'pcm_dma':{'irq_vector_expression':'24 + allocated_channel','allocator_channel_indices':[0,1],
                   'first_base':'0x93010008','first_end':'0x93010018','second_base':'0x9301004c','second_end':'0x9301005c','channel_stride':4,
                   'fifo':'0x93012004','engine_control':'0x93012000'},
        'limits':['Register roles follow original instruction dataflow; no peripheral timings or physical board certification.',
                  'Raw DMA arithmetic fixtures include channels 2/3; allocator only establishes indices 0/1.',
                  'Typed adapter has no numeric-slot resolver or device entry. No launchable image generated.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=produce(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    with a.output.with_suffix('.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=['slot','offset','target','export','typedef','header'],lineterminator='\n');w.writeheader();w.writerows(result['contracts'])
    print('PASS:',len(result['checks']),'ARM/data checks; 28 typed native slots; LCD and PCM register dataflow')
