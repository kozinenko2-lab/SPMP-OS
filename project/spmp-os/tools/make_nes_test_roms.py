#!/usr/bin/env python3
"""Build original synthetic iNES1 ROMs for deterministic CPU/PPU tests.
These assets contain no copyrighted game code, BIOS or commercial graphics.
"""
from pathlib import Path
root=Path(__file__).resolve().parents[1]/'samples'
root.mkdir(exist_ok=True)

def make(prg_banks=1, chr_banks=0, mapper=0, battery=True):
    head=bytearray(b'NES\x1a'+bytes(12))
    head[4]=prg_banks;head[5]=chr_banks
    head[6]=((mapper&0xf)<<4)|(2 if battery else 0)
    head[7]=mapper&0xf0
    return head,bytearray([0xea]*(prg_banks*16384)),bytearray(chr_banks*8192)

def vectors(prg,start=0x8000):
    # Last 6 bytes of cartridge space for NMI, RESET and IRQ.
    for off in (-6,-4,-2):
        prg[off]=start&255;prg[off+1]=(start>>8)&255

def save(name,head,prg,chr_):
    dest=root/name
    dest.write_bytes(head+prg+chr_)
    print(dest.name,len(head)+len(prg)+len(chr_))

# Configures CHR RAM tile 0 to have a single foreground pixel in each row.
# $6000 SRAM receives $A5, and PPU palette 0/1 receives black/red.
p=[
 0x78,0xa2,0xff,0x9a,
 0xa9,0xa5,0x8d,0x00,0x60,
 0xa9,0x3f,0x8d,0x06,0x20,0xa9,0x00,0x8d,0x06,0x20,
 0xa9,0x0f,0x8d,0x07,0x20,0xa9,0x16,0x8d,0x07,0x20,
 0xa9,0x00,0x8d,0x06,0x20,0x8d,0x06,0x20,
 0xa2,0x08,0xa9,0x80,
  0x8d,0x07,0x20,0xca,0xd0,0xfa,
 0xa9,0x08,0x8d,0x01,0x20,
]
loop=0x8000+len(p)
p.extend([0x4c,loop&255,loop>>8])
head,prg,chr_=make()
prg[:len(p)]=bytes(p)
vectors(prg)
save('nes_cpu_ppu_smoke.nes',head,prg,chr_)

# mapper 2: execute from fixed last bank at $C000, switch low bank to
# first bank, read marker $42 at $8000, write it to SRAM.
head,prg,chr_=make(prg_banks=2,mapper=2)
prg[0]=0x42
p=[0xa9,0x00,0x8d,0x00,0x80,0xad,0x00,0x80,0x8d,0x01,0x60,0x4c,0x0b,0xc0]
prg[0x4000:0x4000+len(p)]=bytes(p)
vectors(prg,0xc000)
save('nes_uxrom_smoke.nes',head,prg,chr_)

# mapper 3: bank 0 has blank CHR; bank 1 tile row 0 displays bit7.
head,prg,chr_=make(prg_banks=1,chr_banks=2,mapper=3)
chr_[8192:8192+8]=b'\x80'*8
p=[0xa9,0x3f,0x8d,0x06,0x20,0xa9,0,0x8d,0x06,0x20,
   0xa9,0x0f,0x8d,0x07,0x20,0xa9,0x16,0x8d,0x07,0x20,
   0xa9,1,0x8d,0,0x80, 0xa9,8,0x8d,1,0x20]
loop=0x8000+len(p);p += [0x4c,loop&255,loop>>8]
prg[:len(p)]=bytes(p);vectors(prg)
save('nes_cnrom_smoke.nes',head,prg,chr_)
