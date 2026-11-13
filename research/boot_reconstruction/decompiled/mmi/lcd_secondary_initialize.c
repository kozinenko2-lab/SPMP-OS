/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a43f4; original_symbol=lcd_secondary_initialize */

void lcd_secondary_initialize
               (undefined2 param_1,undefined2 param_2,uint param_3,uint param_4,uint param_5,
               uint param_6,uint param_7)

{
  uint uVar1;
  
  uVar1 = param_6 & 0x3fffffff;
  if (param_7 == 10) {
    uVar1 = uVar1 | 0xc0000000;
  }
  else if (param_7 < 0xb) {
    if (param_7 == 1) {
      uVar1 = uVar1 | 0x40000000;
    }
  }
  else if (param_7 == 0xb) {
    uVar1 = uVar1 | 0x80000000;
  }
  FUN_002e82b8(param_1,param_2,param_3,param_4,uVar1);
  if (param_5 == 0) {
    param_5 = param_4;
  }
  lcd_secondary_set_render_buffer(param_5);
  lcd_select_secondary_span_ops();
  return;
}

