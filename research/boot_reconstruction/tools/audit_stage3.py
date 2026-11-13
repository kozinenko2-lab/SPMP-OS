#!/usr/bin/env python3
"""Exact-build read-only audit of GUI callback, native API and NGame layout."""
import argparse,csv,hashlib,json
from pathlib import Path
from audit_boot import audit,Image,H
import ngame_inspect as ng

def extract_native_api(im):
    assert im.literal(0x2d32bc)==0x5efb74
    assert im.word(0x2d32c8)==0xe3a01055
    default=im.literal(0x2d32cc);assert default==0x2d2db4
    slots=[{'slot':i,'offset':hex(i*4),'target':hex(default),'store':None} for i in range(85)]
    for at in range(0x2d32ec,0x2d353c,4):
        word=im.word(at)
        if word & 0xfffff000 != 0xe5843000:continue
        previous=im.word(at-4)
        if previous==0xe3a03000:target=0
        else:
            assert previous & 0x0f7ff000==0x051f3000,hex(at)
            target=im.literal(at-4)
        offset=word&0xfff;assert offset%4==0 and offset//4<85
        slots[offset//4].update(target=hex(target),store=hex(at))
    for s in slots:
        address=int(s['target'],16)
        if address==0:s['kind']='null'
        elif im.word(address)==0xe3e00001 and im.word(address+4)==0xe12fff1e:s['kind']='stub_returns_minus_2'
        elif im.word(address)==0xe12fff1e:s['kind']='bare_return_no_result_assignment'
        elif im.word(address)==0xe3a00000 and im.word(address+4)==0xe12fff1e:s['kind']='stub_returns_zero'
        else:s['kind']='function_signature_unresolved'
    assert slots[0x11c//4]['target']=='0x339be8' and slots[0x150//4]['target']=='0x0'
    return slots

def produce(root):
    audit(root);blob=(root/'firmware/originals/8000_MMI.RAP').read_bytes();im=Image(blob);checks=[]
    for at,target in [(0x2807dc,0x3be5a0),(0x280760,0x447398),(0x280774,0x3e48a8),
                      (0x280784,0x3bf4c4),(0x28078c,0x3bf500),(0x2807a0,0x3bf470),
                      (0x461854,0x4611bc),(0x461894,0x461130),(0x4618cc,0x345f54),
                      (0x46085c,0x4617e4),(0x460880,0x2d32b0),(0x460884,0x4628b8)]:
        assert im.branch(at)==target,hex(at);checks.append({'address':hex(at),'direct_call':hex(target)})
    for at,expected in {0x3be5a4:0xe5830000,0x3be678:0xe3a03005,0x3be680:0xe1a0f00c,
        0x2807a8:0x1afffff4,0x3bf4b8:0xe2500012,
        0x460120:0xe1a0c00d,0x46018c:0xe59f10d0,0x46025c:0xe89da830,
        0x4617e4:0xe1a0c00d,0x4618c4:0xe1a00004,0x4618ec:0xe89da870,
        0x46122c:0xe3a03080,0x461234:0xe5953078,0x461144:0xe3510b01,
        0x461164:0xe3a03008,0x46116c:0xe2433007,
        0x4618b4:0xe2444010,0x4608e8:0xe1a0f005,0x46087c:0xe3a0560a,
        0x463238:0xe1a03253,0x46323c:0xe2033001,
        0x4ae52c:0xe590b000,0x4ae544:0xe89b7fff,0x4ae548:0xe1b0f00e,
        0x33c148:0x0063e0f8}.items():
        assert im.word(at)==expected,hex(at);checks.append({'address':hex(at),'word':hex(expected)})
    assert im.literal(0x2807d8)==0x2805f8
    assert im.literal(0x3be5a0)==0x560c18
    assert im.literal(0x4608e0)==0x5efb74
    assert im.string(im.literal(0x461180)).encode()==ng.KEY
    assert im.string(im.literal(0x4618c8)).encode()+b'\0'==ng.FOOTER
    table_checks=[]
    for name,address in [('IP',0x50ba2c),('FP',0x50ba6c),('EXPAND',0x50baac),('PERMUTE',0x50badc),('PC1',0x50bafc),('PC2',0x50bb34),('ROTATE',0x50bb64),('SBOX',0x50bb74)]:
        expected=bytes(getattr(ng,name));s=next(s for s in im.s if s['address']<=address<s['address']+s['size']);off=s['offset']+address-s['address']
        assert blob[off:off+len(expected)]==expected
        table_checks.append({'name':name,'address':hex(address),'bytes':len(expected),'sha256':hashlib.sha256(expected).hexdigest()})
    return {'source_sha256':H['8000_MMI.RAP'],'checks':checks,'cipher_tables':table_checks,
        'native_api_base':'0x5efb74','native_api_slots':extract_native_api(im),
        'gui_callback':{'slot':'0x560c18','registered_entry':'0x2805f8','entry_registers':[0,0,0,5]},
        'native_bin_entry':{'pc':'0xa00000','r0':'0x5efb74','other_argument_registers':'not assigned at entry callsite'},
        'limits':['Exact build; no console execution.','Slot addresses are not verified callable signatures.']}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('root',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    r=produce(a.root.resolve());a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2)+'\n')
    with a.output.with_suffix('.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=['slot','offset','target','store','kind']);w.writeheader();w.writerows(r['native_api_slots'])
    print('PASS:',len(r['checks']),'ARM checks, 8 cipher tables, 85 native API slots')
