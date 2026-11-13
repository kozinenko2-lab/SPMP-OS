/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004ce8d0; original_symbol=ecos_var_pool_status */

void ecos_var_pool_status(int param_1,uint param_2,undefined4 *param_3)

{
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar1;
  uint extraout_r1_02;
  int *extraout_r2;
  int *piVar2;
  int *extraout_r2_00;
  int *extraout_r2_01;
  int *extraout_r2_02;
  int iVar3;
  int iVar4;
  
  piVar2 = DAT_004cea18;
  uVar1 = param_2 << 0x10;
  iVar3 = *DAT_004cea18;
  *DAT_004cea18 = iVar3 + 1;
  if (param_1 == 0) {
    FUN_002815bc(DAT_004cea1c,uVar1,piVar2,iVar3 + 1);
    uVar1 = extraout_r1;
    piVar2 = extraout_r2;
  }
  if ((param_2 & 0x80) != 0) {
    *(bool *)(param_3 + 7) = *(int *)(param_1 + 0x24) != 0;
  }
  *param_3 = *(undefined4 *)(param_1 + 0xc);
  if ((param_2 & 2) != 0) {
    piVar2 = *(int **)(param_1 + 0x14);
    param_3[1] = *(int *)(param_1 + 0x18) - (int)piVar2;
  }
  if ((param_2 & 8) != 0) {
    piVar2 = *(int **)(param_1 + 0x20);
    param_3[3] = (*(int *)(param_1 + 0x18) - *(int *)(param_1 + 0x14)) - (int)piVar2;
  }
  param_3[4] = *(undefined4 *)(param_1 + 0x20);
  if ((param_2 & 0x40) != 0) {
    iVar3 = param_1;
    iVar4 = 0;
    while( true ) {
      if (**(int **)(iVar3 + 4) != iVar3) {
        FUN_002815bc(DAT_004cea20,uVar1,piVar2,**(int **)(iVar3 + 4));
        uVar1 = extraout_r1_00;
        piVar2 = extraout_r2_00;
      }
      iVar3 = *(int *)(iVar3 + 4);
      if (*(int *)(iVar3 + 8) == 0) break;
      if (iVar4 < *(int *)(iVar3 + 8)) {
        iVar4 = *(int *)(iVar3 + 8);
      }
    }
    if (iVar3 != param_1) {
      FUN_002815bc(DAT_004cea24,uVar1,piVar2,0);
      uVar1 = extraout_r1_01;
      piVar2 = extraout_r2_01;
    }
    param_3[6] = iVar4 + -0xc;
  }
  param_3[8] = *(undefined4 *)(param_1 + 0xc);
  param_3[9] = *(undefined4 *)(param_1 + 0x10);
  iVar3 = *DAT_004cea18 + -1;
  if (iVar3 == 0) {
    FUN_0033bf08(0,uVar1,piVar2);
  }
  else {
    if (iVar3 == 0) {
      FUN_002815bc(DAT_004cea28,uVar1,piVar2,*DAT_004cea18);
      uVar1 = extraout_r1_02;
      piVar2 = extraout_r2_02;
    }
    if (*DAT_004cea18 == 0) {
      FUN_002815bc(DAT_004cea2c,uVar1,piVar2,0);
    }
    *DAT_004cea18 = iVar3;
  }
  return;
}

