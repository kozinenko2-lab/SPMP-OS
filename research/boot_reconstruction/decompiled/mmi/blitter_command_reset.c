/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e5c60; original_symbol=blitter_command_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void blitter_command_reset(undefined4 param_1,uint param_2)

{
  FUN_002e5ba0(param_1,param_2);
  *DAT_002e5c90 = param_1;
  _DAT_92007028 = param_1;
  return;
}

