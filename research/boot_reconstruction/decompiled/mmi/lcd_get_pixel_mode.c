/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4538; original_symbol=lcd_get_pixel_mode */

undefined4 lcd_get_pixel_mode(void)

{
  uint uVar1;
  
  uVar1 = lcd_get_flags();
  uVar1 = uVar1 & 0xff;
  if (uVar1 == 0x10) {
    return 4;
  }
  if (uVar1 < 0x11) {
    if (uVar1 == 2) {
      return 1;
    }
    if (uVar1 < 3) {
      if (uVar1 == 1) {
        return 0;
      }
    }
    else if (uVar1 == 4) {
      return 3;
    }
  }
  else {
    if (uVar1 == 0x40) {
      return 7;
    }
    if (uVar1 < 0x41) {
      if (uVar1 == 0x20) {
        return 5;
      }
    }
    else if (uVar1 == 0x80) {
      return 8;
    }
  }
  MMI_diag_stub_00298B64();
  return 0;
}

