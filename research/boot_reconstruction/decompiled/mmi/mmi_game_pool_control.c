/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e3898; original_symbol=mmi_game_pool_control */

void mmi_game_pool_control(int param_1,uint param_2)

{
  uint extraout_r1;
  uint uVar1;
  
  uVar1 = param_2;
  if (param_2 == 0xffff && param_1 == 1) {
    ecos_pool_create_wrapper(DAT_002e3900,DAT_002e38fc,DAT_002e3904 - DAT_002e38fc,8);
    uVar1 = extraout_r1;
  }
  if (param_2 == 0xffff && param_1 == 0) {
    ecos_pool_destroy_wrapper((int)DAT_002e3900,uVar1);
    return;
  }
  return;
}

