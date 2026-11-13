/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e77c0; original_symbol=lcd_enable */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void lcd_enable(int param_1)

{
  uint *puVar1;
  
  puVar1 = DAT_002e785c;
  if (param_1 == 0) {
    if (*DAT_002e7858 != 0) {
      _DAT_93000000 = 0;
    }
    *DAT_002e7858 = 0;
  }
  else {
    if (*DAT_002e7858 == 0) {
      FUN_002e80ec((uint)*(ushort *)((int)DAT_002e785c + 10),(uint)(ushort)DAT_002e785c[2]);
      _DAT_93000000 = _DAT_93000000 | (*puVar1 & 0x300000) >> 0x14 | 0x10000;
    }
    *DAT_002e7858 = 1;
  }
  _DAT_93000078 = 1;
  return;
}

