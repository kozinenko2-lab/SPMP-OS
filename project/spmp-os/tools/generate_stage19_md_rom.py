#!/usr/bin/env python3
"""Open test cartridge for Motorola 68000 guest execution; not a commercial ROM."""
import struct
from pathlib import Path
p=Path(__file__).resolve().parents[1]/'samples'/'spmp_md_68000_test.md'
p.parent.mkdir(parents=True,exist_ok=True)
b=bytearray(256*1024)
struct.pack_into('>II',b,0,0x00ff0000,0x00000200)
b[0x100:0x10c]=b'SEGA GENESIS'
b[0x120:0x120+20]=b'SPMP TEST 68K CPU   '
b[0x180:0x182]=b'GM'
struct.pack_into('>II',b,0x1a0,0,len(b)-1)
b[0x1f0:0x1f3]=b'JUE'
# MOVE.B #$5A,$00FF0000; NOP; BRA -2
b[0x200:0x20c]=bytes.fromhex('13fc 005a 00ff 0000 4e71 60fe')
p.write_bytes(b)
assert len(b)==256*1024
print('Generated',p,'bytes',len(b))

# SMD copier format: 512-byte header and odd/even halves of each 16 KiB block.
smd=bytearray(512)
for off in range(0,len(b),16384):
    block=b[off:off+16384]
    smd.extend(block[1::2])
    smd.extend(block[0::2])
sp=p.with_suffix('.smd')
sp.write_bytes(smd)
assert len(smd)==512+len(b)
print('Generated',sp,'bytes',len(smd))
