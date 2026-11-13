/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e3784; original_symbol=mmi_game_pool_free */

void mmi_game_pool_free(int *param_1)

{
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  undefined8 uVar1;
  
  if (param_1 == (int *)0x0) {
    return;
  }
  uVar1 = ecos_pool_free_wrapper(DAT_002e37b8,param_1,0);
  if ((int)uVar1 == 0) {
    FUN_002815bc(DAT_002e37bc,(uint)((ulonglong)uVar1 >> 0x20),extraout_r2,extraout_r3);
    return;
  }
  return;
}

