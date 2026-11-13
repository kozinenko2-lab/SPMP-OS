/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002995d0; original_symbol=mmi_general_free */

void mmi_general_free(uint param_1)

{
  int iVar1;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  
  if (param_1 == 0) {
    return;
  }
  iVar1 = FUN_004cdc40((int *)*DAT_00299608,param_1);
  if (iVar1 == 0) {
    FUN_002815bc(DAT_0029960c,extraout_r1,extraout_r2,extraout_r3);
    return;
  }
  return;
}

