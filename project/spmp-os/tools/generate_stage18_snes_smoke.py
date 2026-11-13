#!/usr/bin/env python3
"""Create a public-domain, no-audio-dependency SNES LoROM smoke cartridge."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'samples/snes_stage18_smoke.sfc'
b=bytearray([0xff])*32768
b[0:3]=bytes([0x78,0x80,0xfe])  # SEI; BRA -2
title=b'SPMP-OS STAGE18 ROM'
b[0x7fc0:0x7fc0+len(title)]=title
b[0x7fd5]=0x20;b[0x7fd6]=0;b[0x7fd7]=5;b[0x7fd8]=0
b[0x7fd9]=1;b[0x7fda]=0x33
b[0x7fdc:0x7fe0]=bytes([0xff,0xff,0,0]);b[0x7ffc:0x7ffe]=bytes([0,0x80])
out.write_bytes(b)
print(f'generated {out.name} {len(b)} bytes')
