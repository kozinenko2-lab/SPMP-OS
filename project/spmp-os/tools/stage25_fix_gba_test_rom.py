#!/usr/bin/env python3
"""Repair the SPMP-OS synthetic GBA ARM7 ROM: ORR must use r1, not r0."""
import pathlib,sys
p=pathlib.Path(sys.argv[1] if len(sys.argv)>1 else "tests/roms/stage24_gba_test.gba")
b=bytearray(p.read_bytes())
assert len(b)>=0xCC
if b[0xc8:0xcc] == bytes.fromhex("031080e3"):
    b[0xca]=0x81
elif b[0xc8:0xcc] != bytes.fromhex("031081e3"):
    raise SystemExit("Unexpected ROM opcode; refusing to modify")
p.write_bytes(b)
print("ARM7 DISPCNT setup fixed: ORR r1,r1,#3")
