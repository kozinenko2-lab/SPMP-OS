/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00299588; original_symbol=mmi_general_alloc */

int * mmi_general_alloc(uint param_1)

{
  int *piVar1;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  
  if (param_1 == 0) {
    return (int *)0x0;
  }
  piVar1 = FUN_004cdab0((int *)*DAT_002995c8,param_1);
  if ((piVar1 != (int *)0x0) && (((uint)piVar1 & 3) != 0)) {
    FUN_002815bc(DAT_002995cc,extraout_r1,extraout_r2,extraout_r3);
  }
  return piVar1;
}

