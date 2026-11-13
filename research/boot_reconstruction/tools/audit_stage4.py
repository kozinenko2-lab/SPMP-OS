#!/usr/bin/env python3
"""Read-only exact-build checks for selected NativeGE ABI contracts.
Prototypes are reconstructions, not bindings or complete firmware sources.
"""
import argparse,hashlib,json
from pathlib import Path
from audit_boot import Image,H
from audit_stage3 import extract_native_api

# offset, exported label, declaration name, verification scope
CONTRACTS=[
    (0x10,'native_paint','mmi_paint_fn','arm_and_mocked_backend_vm'),
    (0x14,'native_load_image','mmi_load_image_fn','arm_and_mocked_backend_vm'),
    (0x18,'native_free_image','mmi_free_image_fn','arm_and_mocked_backend_vm'),
    (0x3c,'native_set_rop','mmi_set_rop_fn','static_arm'),
    (0x40,'native_get_rop','mmi_get_rop_fn','static_arm'),
    (0x44,'native_set_fg_color','mmi_set_fg_fn','arm_and_mocked_backend_vm'),
    (0x48,'native_get_fg_color','mmi_get_fg_fn','arm_and_mocked_backend_vm'),
    (0x58,'native_get_display_rect','mmi_get_display_rect_fn','arm_and_mocked_backend_vm'),
    (0x90,'native_set_framebuffer','mmi_set_framebuffer_fn','arm_and_mocked_backend_vm'),
    (0x94,'native_get_framebuffer_dimensions','mmi_get_dimensions_fn','arm_and_mocked_backend_vm'),
    (0xa4,'native_query_image','mmi_query_image_fn','arm_and_mocked_backend_vm'),
    (0x100,'native_get_key_input','mmi_get_keys_fn','arm_and_mocked_backend_vm'),
    (0x13c,'native_fs_open','mmi_fs_open_fn','arm_and_mocked_backend_vm'),
    (0x140,'native_fs_read','mmi_fs_read_fn','arm_and_mocked_backend_vm'),
    (0x144,'native_fs_write','mmi_fs_write_fn','arm_and_mocked_backend_vm'),
    (0x148,'native_fs_close','mmi_fs_close_fn','arm_and_mocked_backend_vm'),
    (0x14c,'native_fs_seek_status32','mmi_fs_seek_status32_fn','arm_and_mocked_backend_vm'),
]

def produce(rap):
    blob=rap.read_bytes()
    if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Unexpected RAP SHA-256')
    im=Image(blob);checks=[]
    words={
        0x462a8c:0xe1a08002, # open out-fd in r2
        0x462a9c:0xe3a02c02, # 512-byte local path cleared
        0x462b1c:0xa5884000, # out-fd only on nonnegative stat return
        0x462b60:0xe2504000, # read rejects fd==0
        0x462b88:0xe5870000, # read out-count via saved r3
        0x462b8c:0xe1500005, # exact-count success
        0x462bc8:0xe3500020, # write unsigned fd gate <=32
        0x462bcc:0x9a000004,
        0x462bec:0xe5850000,
        0x462c24:0xe3500020, # close same unsigned gate
        0x462c28:0x9a000003,
        0x462c74:0xe2501000, # seek backend return tested signed
        0x462c78:0xa3a00000, # seek status, not position
        0x462c88:0xe3a00002,
        0x2eb6ac:0xe50b1018, # seek backend takes 32-bit offset
        0x2eb6e8:0xe24b1018,
        0x2eb714:0x051b0018,
        0x2ddb80:0xe1a0c002, # QueryImage third argument is output pointer
        0x2ddb84:0xe20000ff,
        0x2ddb88:0xe20110ff,
        0x2ddbf8:0xe1cc00b0, # 16-bit output
        0x2de144:0xe1c400b0, # GetFrameBuffer writes two dimensions
        0x2de154:0xe1c500b0,
        0x2de088:0xe2833003, # SetFrameBuffer gate 1023
        0x2de0cc:0xe1a05085, # driver width*height*2
        0x2dd270:0xe1a04084, # Paint byte count width*height*2
        0x2dd29c:0xe5830004, # destination context pointer updated
        0x2d2fe0:0xe5850004, # key output at +4
        0x2d2fec:0xe3a00001, # polling wrapper returns 1
        0x2d3128:0xe3140a01, # pause priority raw bit
        0x2d31dc:0xe3a04000, # volume clears remaining raw mask
        0x2e373c:0xe3510902, # game pool rejects sizes <32768
        0x299594:0xe2501000, # general heap only rejects zero early
    }
    for address,expected in words.items():
        assert im.word(address)==expected,(hex(address),hex(im.word(address)),hex(expected))
        checks.append({'address':hex(address),'word':hex(expected)})
    calls=[(0x462b04,0x2a5ed4),(0x462b14,0x2eb7b0),(0x462b80,0x2eb5e0),
        (0x462be4,0x2eb610),(0x462c3c,0x2eb640),(0x462c70,0x2eb69c),
        (0x2dd288,0x298c6c),(0x2dd28c,0x29e450),(0x2de104,0x298d34),
        (0x2ddeb4,0x312d20),(0x2d300c,0x2d2e6c),(0x2d3014,0x45ffbc),
        (0x2d3148,0x2d6d60),(0x2d314c,0x3c4c5c),(0x2d3160,0x2d8b88),
        (0x2d3168,0x46190c),(0x2d3174,0x339be8),(0x2d31d0,0x3e30cc),
        (0x2d31d8,0x2c4de8),(0x2d2fd0,0x350630),(0x2d2fdc,0x350758),
        (0x2e3758,0x32e5bc),(0x2995a8,0x4cdab0)]
    for address,target in calls:
        assert im.branch(address)==target,hex(address)
        checks.append({'address':hex(address),'direct_call':hex(target)})
    slots=extract_native_api(im)
    contracts=[{'slot':offset//4,'offset':hex(offset),'target':slots[offset//4]['target'],
        'export':label,'typedef':declaration,'scope':scope} for offset,label,declaration,scope in CONTRACTS]
    base=Path(__file__).resolve().parents[1]
    plan=json.loads((base/'ghidra_targets.json').read_text())
    targets={int(t['address'],16):t['label'] for t in next(p for p in plan['programs'] if p['id']=='mmi')['targets']}
    for c in contracts:assert targets[int(c['target'],16)]==c['export']
    api_targets={int(s['target'],16) for s in slots}
    assert not ({0x299588,0x2e3724,0x2995d0,0x2e3784}&api_targets)
    return {'source_sha256':H['8000_MMI.RAP'],'checks':checks,'contracts':contracts,
        'allocators_not_in_native_api':['0x299588','0x2995d0','0x2e3724','0x2e3784'],
        'limits':['Image-specific wrapper contracts; not callable fixed-address bindings.',
                  'Backend behavior, ownership on all paths and hardware operation are not certified.']}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--rap',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    result=produce(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS:',len(result['checks']),'ARM checks;',len(result['contracts']),'selected declarations match native slots')
