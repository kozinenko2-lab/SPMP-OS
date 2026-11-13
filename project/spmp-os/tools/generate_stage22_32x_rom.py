#!/usr/bin/env python3
from pathlib import Path
import struct
base=Path(__file__).resolve().parents[1]
b=bytearray(256*1024)
struct.pack_into('>II',b,0,0xff0000,0x200)
b[0x100:0x110]=b'SEGA 32X        '
b[0x120:0x120+18]=b'SPMP HOME BREW 32X'
b[0x180:0x182]=b'GM'
struct.pack_into('>II',b,0x1a0,0,len(b)-1)
# 68k: write RAM flag, write 32X ADEN bit, spin (we test SH2 init, not game compatibility)
b[0x200:0x200+20]=bytes.fromhex('13fc005a00ff000033fc000100a151004e7160fe')
p=base/'samples/spmp_32x_init_test.32x';p.write_bytes(b);print(p,len(b))
