#!/usr/bin/env python3
"""Check compiler output identity, without loading, linking or executing it."""
import struct,sys
from pathlib import Path
for name in sys.argv[1:]:
    data=Path(name).read_bytes()
    if len(data)<52 or data[:7]!=b'\x7fELF\x01\x01\x01':raise ValueError(name+': expected ELF32 little-endian v1')
    kind,machine,version=struct.unpack_from('<HHI',data,16)
    flags=struct.unpack_from('<I',data,36)[0]
    if (kind,machine,version)!=(1,40,1):raise ValueError(name+': expected ARM ET_REL')
    if flags>>24!=5 or flags&0x400:raise ValueError(name+': EABI5 without hard-float flag required')
    print('PASS:',Path(name).name,'ELF32 LE ARM ET_REL EABI5; flags='+hex(flags))
