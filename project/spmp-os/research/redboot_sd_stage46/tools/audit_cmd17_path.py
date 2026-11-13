#!/usr/bin/env python3
"""Evidence tied to original RAP SHA, without writing flash or reading MMIO.

We do NOT claim that CMD17 is recovered.  We confirm two CMD18 setup
instructions, and three loops whose ARM back-branches lack independent bounds.
"""
import hashlib, json, struct, argparse
from pathlib import Path
SHA='5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
BASE=0x280000; OFF=0x4c
W={
0x2b9b78:0xe1a0c00d,
0x2b9b90:0xe59f707c,
0x2b9b9c:0xe5873000,
0x2b9bac:0xe5831000,
0x2b9bbc:0xe5830000,
0x2b9bc4:0xebffff9b,
0x2bb004:0xe3a00d56,
0x2bb008:0xe2800012,
0x2bb014:0xebfffad7,
0x2bb760:0xe3a00d56,
0x2bb764:0xe2800012,
0x2bb774:0xebfff8ff,
0x2bb250:0xe5923000,
0x2bb254:0xe3130008,
0x2bb258:0x0afffffc,
0x2bb274:0xe5923000,
0x2bb278:0xe3130001,
0x2bb27c:0x1afffffc,
0x2bb910:0xe5923000,
0x2bb914:0xe3130008,
0x2bb918:0x0afffffc,
0x2bb950:0xe2844001,
0x2bb954:0xe1540002,
0x2bb958:0x8a000030,
0x2bb95c:0xe5913000,
0x2bb960:0xe3130001,
0x2bb964:0x1afffff9,
0x2bba90:0x92b0b014,
0x2bb330:0x92b0b014,
}
def get(b,pc):
    start=OFF+pc-BASE
    if start<0 or start+4>len(b):raise ValueError('outside first ARM segment')
    return struct.unpack_from('<I',b,start)[0]
def target(pc,w):
    if (w>>24)!=0xea and (w>>24)!=0xeb and (w>>24)!=0x0a and (w>>24)!=0x1a:
        raise ValueError(f'not recognized branch {pc:#x}')
    n=(w&0xffffff)<<2
    if n&0x2000000:n-=0x4000000
    return (pc+8+n)&0xffffffff
def audit(data):
    if hashlib.sha256(data).hexdigest()!=SHA:raise ValueError('unexpected RAP SHA')
    if data[:4]!=b'MMPi' or data[0x34:0x38]!=b'MMPs':raise ValueError('unexpected layout')
    for a,w in W.items():
        found=get(data,a)
        if found!=w:raise ValueError(f'opcode mismatch at {a:#x}: got {found:#x}')
    for a,b in [(0x2bb258,0x2bb250),(0x2bb27c,0x2bb274),(0x2bb918,0x2bb910),(0x2bb964,0x2bb950),
                (0x2bb014,0x2b9b78),(0x2bb774,0x2b9b78)]:
        if target(a,get(data,a))!=b:raise ValueError(f'branch target changed {a:#x}')
    cmds=[(0x2bb014,5504+18),(0x2bb774,5504+18)]
    # These contain the CMD18 numeric index in low six bits.  Whether the
    # remaining command flags are correct for an SD host is not established.
    assert all((v&0x3f)==18 for _,v in cmds)
    return {
        'source_sha256': SHA,
        'verified_words':len(W),
        'cmd17_status':'NOT proven: no complete CMD17 transaction sequence recovered; do not invoke register addresses',
        'command_calls':[{'bl':hex(a),'command_word':hex(v),'low_6_bits':v&63,'confidence':'command word reconstruction from adjacent MOV+ADD'} for a,v in cmds],
        'poll_loops':[
            {'pc':'0x002BB250','branch':'0x002BB258','condition':'status & 0x8 becomes nonzero','local_bound':'none in this three-instruction loop'},
            {'pc':'0x002BB274','branch':'0x002BB27C','condition':'status & 0x1 becomes zero','local_bound':'none in this three-instruction loop'},
            {'pc':'0x002BB910','branch':'0x002BB918','condition':'status & 0x8 becomes nonzero','local_bound':'none in this three-instruction loop'},
            {'pc':'0x002BB950','branch':'0x002BB964','condition':'status & 0x1 becomes zero','local_bound':'counter checked at 0x002BB954/0x002BB958'}
        ],
        'cautions':['No verified early RedBoot SD/MMC backend','No confirmed card-power,pinmux,clock,DMA/cache-coherency setup',
                    'No OEM FAT log patch','No protection against asynchronous hang until watchdog is proven'],
        'mmio_access':False,
        'exact_machine_words':[{'pc':hex(a),'opcode':hex(w)} for a,w in sorted(W.items())]
    }
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rap',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();r=audit(a.rap.read_bytes());a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(r,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('Verified',len(W),'ARM machine words; CMD17 remains unproven')
