#!/usr/bin/env python3
"""Synthetic Sega CD media test - no copyrighted BIOS or disc content."""
from pathlib import Path
import struct
base=Path(__file__).resolve().parents[1];samples=base/'samples';samples.mkdir(exist_ok=True)
sectors=400
iso=bytearray(2048*sectors)
iso[:14]=b'SEGADISCSYSTEM'
iso[0x100:0x104]=b'SPMP'
(samples/'spmp_test_cd.iso').write_bytes(iso)
binimg=bytearray(sectors*2352)
for i in range(sectors):
    binimg[i*2352+16:i*2352+16+2048]=iso[i*2048:(i+1)*2048]
(samples/'spmp_test_cd.bin').write_bytes(binimg)
(samples/'spmp_test_cd.cue').write_text('FILE "spmp_test_cd.bin" BINARY\n  TRACK 01 MODE1/2352\n    INDEX 01 00:00:00\n')
# Synthetic dummy BIOS ONLY for boot-path exercise, NOT a working Sega CD BIOS.
# Valid-ish M68K vector/loop lets test initialization and error handling.
bios=bytearray(128*1024)
struct.pack_into('>II',bios,0,0x00ff0000,0x00000200)
bios[0x100:0x110]=b'SEGA GENESIS    '
bios[0x124:0x128]=b'BOOT'
bios[0x200:0x202]=b'`\xfe' # BRA -2, deliberately no BIOS implementation
biosdir=base/'build/stage22_test_bios';biosdir.mkdir(parents=True,exist_ok=True)
(biosdir/'us_scd2_9306.bin').write_bytes(bios)
print('generated',len(iso),'ISO bytes',len(binimg),'BIN bytes','synthetic test BIOS')
