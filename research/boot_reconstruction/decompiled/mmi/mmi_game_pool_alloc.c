/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e3724; original_symbol=mmi_game_pool_alloc */

int mmi_game_pool_alloc(uint param_1)

{
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  ulonglong uVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  if (param_1 < 0x8000) {
    MMI_diag_stub_00298B64();
    return 0;
  }
  uVar1 = ecos_pool_alloc_wrapper(DAT_002e377c,param_1);
  if (((int)uVar1 != 0) && ((uVar1 & 3) != 0)) {
    FUN_002815bc(DAT_002e3780,(uint)(uVar1 >> 0x20),extraout_r2,extraout_r3);
  }
  return (int)uVar1;
}

