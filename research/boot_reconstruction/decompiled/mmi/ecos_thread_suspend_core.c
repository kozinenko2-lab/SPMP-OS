/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033a720; original_symbol=ecos_thread_suspend_core */

void ecos_thread_suspend_core(uint param_1,uint param_2)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  int *piVar2;
  int *extraout_r2;
  int *extraout_r2_00;
  int *extraout_r2_01;
  uint uVar3;
  
  piVar2 = (int *)(*DAT_0033a7bc + 1);
  *DAT_0033a7bc = (int)piVar2;
  uVar3 = *(int *)(param_1 + 0x3c) + 1;
  *(uint *)(param_1 + 0x3c) = uVar3;
  if (499 < uVar3) {
    FUN_002815bc(DAT_0033a7c0,param_2,piVar2,uVar3);
    param_2 = extraout_r1;
    piVar2 = extraout_r2;
  }
  if (*(int *)(param_1 + 0x38) == 0) {
    FUN_0033bacc(DAT_0033a7c4,param_1,piVar2);
    param_2 = extraout_r1_00;
    piVar2 = extraout_r2_00;
  }
  *(uint *)(param_1 + 0x38) = *(uint *)(param_1 + 0x38) | 4;
  iVar1 = *DAT_0033a7bc + -1;
  if (iVar1 == 0) {
    FUN_0033bf08(0,param_2,piVar2);
  }
  else {
    if (iVar1 == 0) {
      FUN_002815bc(DAT_0033a7c8,param_2,piVar2,*DAT_0033a7bc);
      param_2 = extraout_r1_01;
      piVar2 = extraout_r2_01;
    }
    if (*DAT_0033a7bc == 0) {
      FUN_002815bc(DAT_0033a7cc,param_2,piVar2,0);
    }
    *DAT_0033a7bc = iVar1;
  }
  return;
}

