/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a5138; original_symbol=lcd_render_clear_core */

void lcd_render_clear_core(int param_1,int param_2,undefined4 *param_3)

{
  uint uVar1;
  
  uVar1 = lcd_get_flags();
  if ((uVar1 & 1) != 0) {
    FUN_002a4de0(param_1,param_2,param_3);
    return;
  }
  if ((uVar1 & 2) != 0) {
    FUN_002a4e24(param_1,param_2,param_3);
    return;
  }
  if ((uVar1 & 4) != 0) {
    FUN_002a4e68(param_1,param_2,param_3);
    return;
  }
  if ((uVar1 & 0x10) != 0) {
    FUN_002a4eac(param_1,param_2,(uint)param_3);
    return;
  }
  if ((uVar1 & 0x20) != 0) {
    FUN_002a4f40(param_1,param_2,(uint)param_3);
    return;
  }
  if ((uVar1 & 0x40) != 0) {
    FUN_002a4fec(param_1,param_2,(uint)param_3);
    return;
  }
  if ((uVar1 & 0x80) == 0) {
    return;
  }
  FUN_002a5084(param_1,param_2,(uint)param_3);
  return;
}

