/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e842c; original_symbol=lcd_secondary_set_scanout */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void lcd_secondary_set_scanout(undefined4 param_1)

{
  *(undefined4 *)(DAT_002e8460 + 0x18) = param_1;
  if (*DAT_002e8464 != 0) {
    _DAT_93000078 = 1;
  }
  _DAT_93000084 = param_1;
  return;
}

