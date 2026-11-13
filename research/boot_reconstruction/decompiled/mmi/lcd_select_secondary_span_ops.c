/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4180; original_symbol=lcd_select_secondary_span_ops */

void lcd_select_secondary_span_ops(void)

{
  uint uVar1;
  
  uVar1 = lcd_get_flags();
  uVar1 = uVar1 & 0xc0000000;
  if (uVar1 == 0) {
    *DAT_002a4224 = DAT_002a4220;
    *DAT_002a422c = DAT_002a4228;
    return;
  }
  if (uVar1 == 0x40000000) {
    *DAT_002a4224 = DAT_002a4230;
    *DAT_002a422c = DAT_002a4234;
    return;
  }
  if (uVar1 == 0x80000000) {
    *DAT_002a4224 = DAT_002a4238;
    *DAT_002a422c = DAT_002a423c;
    return;
  }
  if (uVar1 == 0xc0000000) {
    *DAT_002a4224 = DAT_002a4240;
    *DAT_002a422c = DAT_002a4244;
    return;
  }
  return;
}

