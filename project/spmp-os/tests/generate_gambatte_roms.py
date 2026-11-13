"""Generate entirely original GB/GBC MBC1+RAM smoke ROMs (no game copyrighted data).
Nintendo logo header bytes are fixed technical boot validation data, not a game.
"""
from pathlib import Path
OUT=Path(__file__).resolve().parents[1]/'samples'
LOGO=bytes.fromhex('CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D 00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99 BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E')
def make(name,gbc,value):
 b=bytearray([0]*0x8000)
 b[0x100:0x104]=bytes([0x00,0xC3,0x50,0x01])
 b[0x104:0x134]=LOGO
 title=('SPMP '+name.upper()).encode('ascii')[:15]
 b[0x134:0x134+len(title)]=title
 b[0x143]=0x80 if gbc else 0x00
 b[0x146]=0x00
 b[0x147]=0x03 # MBC1 + RAM + BATTERY
 b[0x148]=0x00 # 32KB ROM
 b[0x149]=0x02 # 8KB RAM
 b[0x14a]=0x01
 b[0x14b]=0x33
 b[0x14d]=(-sum(b[0x134:0x14d])-25)&255
 # Enable cartridge RAM, store a byte, then draw a checker pattern by
 # programming tile 0 while LCD is off. PPU must render repeated tile 0.
 program=bytes([
  0x3e,0x0a,0xea,0x00,0x00,  # RAM enable at 0x0000
  0x3e,value,0xea,0x00,0xa0, # store unique SRAM byte at 0xA000
  0x3e,0x00,0xe0,0x40,       # LCDC off while writing VRAM
  0x3e,0xe4,0xe0,0x47,       # DMG palette colours 0..3
  0x21,0x00,0x80,            # HL = tile 0 at VRAM 0x8000
  0x06,0x08,                 # B = 8 tile rows
  0x3e,0xaa,0x22,            # pixel low bits, LD (HL+),A
  0x3e,0x55,0x22,            # pixel high bits, LD (HL+),A
  0x05,0x20,0xf7,            # DEC B; JR NZ -9
  0x3e,0x91,0xe0,0x40,       # LCDC on; BG enable, signed tile index off
  0x18,0xfe                 # idle
 ])
 b[0x150:0x150+len(program)]=program
 b[0x14e]=(sum(b[:0x14e])+sum(b[0x150:]))>>8 & 255
 b[0x14f]=(sum(b[:0x14e])+sum(b[0x150:])) & 255
 (OUT/f'spmp_{name}.{"gbc" if gbc else "gb"}').write_bytes(b)
if __name__=='__main__':
 OUT.mkdir(exist_ok=True)
 make('battery',False,0x42)
 make('color',True,0x57)
 print('Generated original GB and GBC smoke ROMs')
