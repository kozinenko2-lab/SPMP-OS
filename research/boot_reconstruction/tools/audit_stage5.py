#!/usr/bin/env python3
"""Exact-build static evidence for constructors, Game memory, resources and exit."""
import argparse,hashlib,json
from pathlib import Path
from audit_boot import Image,H
from audit_stage3 import extract_native_api
from native_lifecycle_models import fresh_pool_layout

CONTRACTS=[(0xd4,'native_resource_init','mmi_resource_init_fn'),
    (0xd8,'native_resource_get','mmi_resource_get_fn'),(0xdc,'native_resource_play','mmi_resource_play_fn'),
    (0xe0,'native_resource_pause','mmi_resource_pause_fn'),(0xe4,'native_resource_resume','mmi_resource_resume_fn'),
    (0xe8,'native_resource_stop','mmi_resource_stop_fn'),(0x130,'native_exit_control','mmi_game_exit_control_fn')]

def produce(rap):
    blob=rap.read_bytes()
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Wrong RAP SHA-256')
    im=Image(blob);checks=[]
    words={0x299abc:0xe414f004, # constructor table traversed backwards
        0x4ce050:0xe355000b,0x4ce05c:0xe1a03083, # promote 8-byte alignment above 12-byte header
        0x4ce2cc:0xe0035002,0x4ce344:0xe0653003,0x4ce3a4:0xe284400c,
        0x4ce9bc:0xe246300c,0x2e3884:0xc5843028, # largest free block minus header -> info+40
        0x460dc4:0xe350060a,0x460dc8:0x8a000007, # only base>A00000 warns
        0x460dd4:0xe241160a,0x460ddc:0xe5831000,
        0x4608ec:0xe3a00102, # normal return uses negative cleanup mode
        0x4619ec:0xe3540000,0x4619f8:0xabfb6056,0x461a20:0xba000004,
        0x461e20:0xe3a0460f,0x461e3c:0x1a00000a, # cached kind-5 returns fixed default
        0x461ee0:0xeafffffe,0x461ef0:0xe3a02601,
        0x46201c:0xe5832000, # invalid getRes disables global supported mask
        0x462088:0xe2844014,0x462138:0xe1853003,
        0x4621b0:0x03e0220f, # zero repeats -> 0x0fffffff
        0x4622fc:0x0a000001,0x462300:0xe3e00000,
        0x462520:0x089da810, # whole-function return if MIDI already stopped
        0x4626e0:0xe2822080,0x4627cc:0xa1a00003,0x4627dc:0xb1a00003,
        0x462838:0xe59a0000,0x462840:0xe2632b01, # partial flush zeros buffer base
        0x462ca8:0xe2804601,0x462cb4:0xe3a02a32,0x462cc0:0xe1a00004,
        0x339b28:0xe5903038,0x33aac4:0xe1530000,0x33aac8:0x0bffffdc}
    for address,word in words.items():
        assert im.word(address)==word,hex(address)
        checks.append({'address':hex(address),'word':hex(word)})
    calls=[(0x280144,0x299a98),(0x2e3920,0x2e3898),(0x2e38d8,0x32e594),
        (0x32e5a0,0x4cdff0),(0x32e5c8,0x4ce274),(0x32e5dc,0x4ce478),(0x32e600,0x4ce8d0),
        (0x460cc0,0x2e3724),(0x460cd0,0x2e3724),(0x460db0,0x2e37c0),
        (0x460db8,0x2e3724),(0x460ec4,0x460800),(0x460f8c,0x2e3784),
        (0x4608f0,0x4619ac),(0x4619e8,0x339b58),(0x461a04,0x4629cc),
        (0x461a2c,0x339b94),(0x339b64,0x33a720),(0x339ba0,0x33aaac),
        (0x461ef4,0x298d34),(0x461ef8,0x462c94),(0x462cbc,0x45ffcc),(0x462cc4,0x464f78),
        (0x4621c0,0x46438c),(0x4622cc,0x461510),(0x462810,0x2c567c),
        (0x462844,0x298d34),(0x462850,0x2c567c),(0x462a18,0x339b18)]
    for address,target in calls:
        assert im.branch(address)==target,hex(address)
        checks.append({'address':hex(address),'direct_call':hex(target)})
    literals={0x299aa4:0x56ad58,0x299aa8:0x56ad08,0x2e38c0:0x9fff00,
        0x2e38c4:0x606348,0x2e38cc:0x2000000,0x462018:0x5641fc,
        0x462cb0:0x50b9e8,0x33aabc:0x63e0f0}
    for address,value in literals.items():
        assert im.literal(address)==value,hex(address)
        checks.append({'address':hex(address),'literal':hex(value)})
    assert im.string(0x50b9e8)=='/Rom/image/game/tonelib.bin'
    constructors=[im.word(a) for a in range(0x56ad08,0x56ad5c,4)]
    assert len(constructors)==21 and constructors[0]==0x2e3908
    slots=extract_native_api(im)
    declarations=[{'offset':hex(offset),'target':slots[offset//4]['target'],'export':name,'typedef':declaration}
                  for offset,name,declaration in CONTRACTS]
    plan=json.loads((Path(__file__).resolve().parents[1]/'ghidra_targets.json').read_text())
    names={int(t['address'],16):t['label'] for p in plan['programs'] if p['id']=='mmi' for t in p['targets']}
    for c in declarations:assert names[int(c['target'],16)]==c['export']
    return {'source_sha256':H['8000_MMI.RAP'],'checks':checks,'contracts':declarations,
        'constructors':{'table_start':'0x56ad08','table_end_exclusive':'0x56ad5c','invoked_reverse':[hex(a) for a in reversed(constructors)]},
        'configured_pool':{'object':'0x606348','start':'0x9fff00','end_exclusive':'0x2000000','requested_alignment':8},
        'fresh_pool_model':fresh_pool_layout(),
        'audio_workspace':{'default_base':'0xf00000','cleared_bytes':'0x100000','tonelib_destination':'0x1000000','tonelib_capacity':'0x32000'},
        'limits':['Static exact-build evidence; configured range is not physical RAM certification.',
                  'Fresh-pool addresses change with allocation history; no reserved SPMP-OS region is established.',
                  'See separate RAM-only VM observations and mocked-backend limits.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=produce(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',len(result['checks']),'ARM checks; 21 constructors;',len(result['contracts']),'resource/exit declarations')
