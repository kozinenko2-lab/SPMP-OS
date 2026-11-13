#!/usr/bin/env python3
"""Reproducible, read-only forensic checks for user's stock 8000_MMI.RAP.
No write to firmware, no calls to OEM addresses, no MMIO register definitions.
"""
import argparse, csv, hashlib, json, struct
from pathlib import Path
SHA = '5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
LOAD=0x280000; FILE_OFFSET=0x4c
TABLE_ADDRESS=0x513cdc
NAMES=('read','write','lseek','ioctl','select','fsync','close','fstat','getinfo','setinfo')
EXPECTED=(0x283f3c,0x283fc4,0x284054,0x2840e0,0x282474,
          0x284100,0x284108,0x284188,0x284288,0x2842a8)
COMMAND_MARKERS=(
 'MASK_CMD18 fail','MASK_CMD25 fail','issue MASK_CMD17 cmd fail',
 'issue MASK_CMD24 cmd fail','MASK_CMD12 fail retry now',
 'sd_mblk_read_dma stop MASK_CMD12 fail', 'SD_POLL_WAIT(MASK_S_CmdComplete) error',
 'SD card not in TRAN state','force reset sd card','SD card not insert'
)
def readword(b,ram):
    off=FILE_OFFSET+(ram-LOAD)
    if off<0 or off+4>len(b):raise ValueError('Address out of bounds')
    return struct.unpack_from('<I',b,off)[0]
def bl_target(pc,word):
    if word>>24 & 0xf not in (0xb,):raise ValueError('Not BL')
    disp=(word&0xffffff)<<2
    if disp & 0x2000000:disp-=0x4000000
    return (pc+8+disp)&0xffffffff
def analyze(b):
    got=hashlib.sha256(b).hexdigest()
    if got!=SHA: raise ValueError('SHA-256 mismatch: unknown image, no address attribution')
    if b[:4]!=b'MMPi' or b[0x34:0x38]!=b'MMPs' or readword(b,0x280000)!=0xe59ff018:
        raise ValueError('Not the expected firmware layout')
    tbl=[readword(b,TABLE_ADDRESS+i*4) for i in range(10)]
    if tbl!=list(EXPECTED):raise ValueError('Unexpected fileops pointer table')
    # Actual fsync implementation is MOV r0,#0; BX lr (not a cache flush).
    assert readword(b,tbl[5])==0xe3a00000
    assert readword(b,tbl[5]+4)==0xe12fff1e
    # write(file,uio): LDR r1,[r6]; BL lower-level function.
    assert readword(b,0x28408c)==0xe5961000
    assert bl_target(0x284090,readword(b,0x284090))==0x2a9834
    # read(file,uio): LDM r3,{r1,r2}; BL lower-level function.
    assert readword(b,0x283f70)==0xe8930006
    assert bl_target(0x283f74,readword(b,0x283f74))==0x2a92f8
    # SD polling path and exact counter construction from instructions.
    expected_poll={0x2b9a5c:0xe3a01cc3,0x2b9a60:0xe2811050,
                   0x2b9a74:0xe2822001,0x2b9a78:0xe1520001,
                   0x2b9a7c:0x8a000002,0x2b9a88:0x0afffff6}
    for a,v in expected_poll.items():
        if readword(b,a)!=v:raise ValueError('SD polling body changed')
    assert bl_target(0x2b9ab0,readword(b,0x2b9ab0))==0x298b64
    commands=[]
    for marker in COMMAND_MARKERS:
        pos=b.find(marker.encode('ascii'))
        commands.append(dict(marker=marker,found=pos>=0,
            rap_file_offset=f'0x{pos:08x}' if pos>=0 else None,
            runtime_string_address=f'0x{LOAD+pos-FILE_OFFSET:08x}' if pos>=0 else None,
            confidence='string_only_not_command_flow' if pos>=0 else 'absent'))
    return {
      'rap_sha256':got,
      'fat_fileops_table_ram':f'0x{TABLE_ADDRESS:08x}',
      'fat_fileops_table_rap_offset':f'0x{TABLE_ADDRESS-LOAD+FILE_OFFSET:08x}',
      'fat_fileops':[dict(index=i,operation=n,address=f'0x{v:08x}',
             evidence='32-bit pointer in fileops table') for i,(n,v) in enumerate(zip(NAMES,tbl))],
      'fsync_implementation':{'address':f'0x{tbl[5]:08x}',
             'opcodes':['0xe3a00000','0xe12fff1e'],
             'effect':'return zero; no flush calls in this function',
             'warning':'cannot assume media durability from OEM fsync'},
      'fat_read_lower_function':'0x002a92f8',
      'fat_write_lower_function':'0x002a9834',
      'sd_poll':{'function':'0x002b9a38', 'counter_setup':['mov r1,#49920','add r1,r1,#80'],
              'limit_comparison':50000,'unit':'poll iterations, NOT milliseconds',
              'loop_pc':'0x002b9a68','comparison_pc':'0x002b9a78',
              'back_edge_pc':'0x002b9a88', 'observed':'conditional status polling and error/report paths',
              'not_verified':'which SD command this polling sequence serves and the controller clock'},
      'sd_command_diagnostics':commands,
      'hardware_writes_performed':False,
      'limitations':['No tested SD read/write driver','No installed MMI hooks',
          'No verified reserved RAM spanning RedBoot and MMI',
          'No confirmed physical media flush / write barrier',
          'String evidence is not a verified call graph']
    }
def main():
    ap=argparse.ArgumentParser();ap.add_argument('rap',type=Path);ap.add_argument('--out',type=Path,required=True)
    ar=ap.parse_args();data=analyze(ar.rap.read_bytes());ar.out.mkdir(parents=True,exist_ok=True)
    (ar.out/'stage44_oem_fat_sd_evidence.json').write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
    with (ar.out/'fat_fileops.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=data['fat_fileops'][0].keys());w.writeheader();w.writerows(data['fat_fileops'])
    with (ar.out/'sd_command_markers.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=data['sd_command_diagnostics'][0].keys());w.writeheader();w.writerows(data['sd_command_diagnostics'])
    print('Stage44 static evidence verified; table=10, poll=50000, command strings=',
          sum(x['found'] for x in data['sd_command_diagnostics']))
if __name__=='__main__':main()
