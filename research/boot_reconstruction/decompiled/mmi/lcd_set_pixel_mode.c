/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4460; original_symbol=lcd_set_pixel_mode */

void lcd_set_pixel_mode(undefined4 param_1)

{
  uint uVar1;
  
  uVar1 = lcd_get_flags();
  uVar1 = uVar1 & 0xffffff00;
  switch(param_1) {
  case 1:
    uVar1 = uVar1 | 2;
    break;
  default:
    uVar1 = uVar1 | 1;
    break;
  case 3:
    uVar1 = uVar1 | 4;
    break;
  case 4:
    uVar1 = uVar1 | 0x10;
    break;
  case 5:
    uVar1 = uVar1 | 0x20;
    break;
  case 7:
    uVar1 = uVar1 | 0x40;
    break;
  case 8:
    uVar1 = uVar1 | 0x80;
  }
  lcd_set_flags(uVar1);
  lcd_apply_pixel_mode();
  lcd_select_main_span_ops();
  return;
}

