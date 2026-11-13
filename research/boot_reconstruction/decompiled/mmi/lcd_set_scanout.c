/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e83f0; original_symbol=lcd_set_scanout */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void lcd_set_scanout(undefined4 param_1)

{
  *(undefined4 *)(DAT_002e8424 + 0xc) = param_1;
  if (*DAT_002e8428 != 0) {
    _DAT_93000078 = 1;
  }
  _DAT_93000054 = param_1;
  return;
}

