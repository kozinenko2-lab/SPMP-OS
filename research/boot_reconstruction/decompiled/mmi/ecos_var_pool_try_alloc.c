/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004ce274; original_symbol=ecos_var_pool_try_alloc */

int * ecos_var_pool_try_alloc(int *param_1,uint param_2)

{
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint uVar1;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  uint extraout_r1_07;
  uint extraout_r1_08;
  int *extraout_r2;
  int *extraout_r2_00;
  int *extraout_r2_01;
  int *piVar2;
  int *extraout_r2_02;
  int *extraout_r2_03;
  int *extraout_r2_04;
  int *extraout_r2_05;
  int *extraout_r2_06;
  int *extraout_r2_07;
  int *extraout_r2_08;
  int iVar3;
  uint extraout_r3;
  int *piVar4;
  uint extraout_r3_00;
  uint extraout_r3_01;
  int *piVar5;
  uint uVar6;
  
  piVar2 = DAT_004ce448;
  iVar3 = *DAT_004ce448;
  *DAT_004ce448 = iVar3 + 1;
  uVar1 = param_2;
  if (param_1 == (int *)0x0) {
    FUN_002815bc(DAT_004ce44c,param_2,piVar2,iVar3 + 1);
    uVar1 = extraout_r1;
    piVar2 = extraout_r2;
  }
  if (param_1[5] == 0) {
    piVar5 = (int *)0x0;
    uVar6 = 0;
  }
  else {
    piVar2 = (int *)-param_1[7];
    uVar6 = param_2 + param_1[7] + 0xb & (uint)piVar2;
    piVar5 = param_1;
    do {
      if (*(int **)piVar5[1] != piVar5) {
        FUN_002815bc(DAT_004ce450,uVar1,piVar2,*(int **)piVar5[1]);
        uVar1 = extraout_r1_00;
        piVar2 = extraout_r2_00;
      }
      piVar5 = (int *)piVar5[1];
      if (piVar5[2] == 0) {
        uVar6 = 0;
        if (piVar5 != param_1) {
          FUN_002815bc(DAT_004ce454,uVar1,piVar2,0);
          uVar1 = extraout_r1_01;
          piVar2 = extraout_r2_01;
          uVar6 = extraout_r3;
        }
        piVar5 = (int *)0x0;
        goto LAB_004ce3dc;
      }
    } while (piVar5[2] < (int)uVar6);
    if (piVar5[2] == uVar6) {
      iVar3 = *piVar5;
      *(int *)(iVar3 + 4) = piVar5[1];
      piVar2 = (int *)piVar5[1];
      *piVar2 = iVar3;
    }
    else {
      if (piVar5[2] <= (int)uVar6) {
        FUN_002815bc(DAT_004ce458,uVar1,piVar2,piVar5[2]);
        uVar1 = extraout_r1_02;
        piVar2 = extraout_r2_02;
      }
      iVar3 = piVar5[2] - uVar6;
      piVar5[2] = iVar3;
      if (iVar3 < 0xc) {
        FUN_002815bc(DAT_004ce45c,uVar1,piVar2,iVar3);
        uVar1 = extraout_r1_03;
        piVar2 = extraout_r2_03;
      }
      piVar5 = (int *)((int)piVar5 + piVar5[2]);
    }
    piVar4 = (int *)param_1[5];
    if ((piVar5 < piVar4) || (piVar4 = (int *)param_1[6], piVar4 < piVar5)) {
      FUN_002815bc(DAT_004ce460,uVar1,piVar2,piVar4);
      uVar1 = extraout_r1_04;
      piVar2 = extraout_r2_04;
    }
    piVar5[2] = uVar6;
    *piVar5 = 0xd530d53;
    piVar5[1] = 0xd530d53;
    param_1[8] = param_1[8] - uVar6;
    piVar5 = piVar5 + 3;
    uVar6 = param_1[7] - 1;
    if ((uVar6 & (uint)piVar5) != 0) {
      FUN_002815bc(DAT_004ce464,uVar1,piVar2,uVar6);
      uVar1 = extraout_r1_05;
      piVar2 = extraout_r2_05;
      uVar6 = extraout_r3_00;
    }
    if (piVar5 == (int *)0x0) {
      FUN_002e3720();
      uVar1 = extraout_r1_06;
      piVar2 = extraout_r2_06;
      uVar6 = extraout_r3_01;
    }
  }
LAB_004ce3dc:
  if (param_1 == (int *)0x0) {
    FUN_002815bc(DAT_004ce44c,uVar1,piVar2,uVar6);
    uVar1 = extraout_r1_07;
    piVar2 = extraout_r2_07;
  }
  iVar3 = *DAT_004ce448 + -1;
  if (iVar3 == 0) {
    FUN_0033bf08(0,uVar1,piVar2);
  }
  else {
    if (iVar3 == 0) {
      FUN_002815bc(DAT_004ce46c,uVar1,piVar2,*DAT_004ce448);
      uVar1 = extraout_r1_08;
      piVar2 = extraout_r2_08;
    }
    if (*DAT_004ce448 == 0) {
      FUN_002815bc(DAT_004ce470,uVar1,piVar2,0);
    }
    *DAT_004ce448 = iVar3;
  }
  if (piVar5 == (int *)0x0) {
    FUN_002e3720();
  }
  return piVar5;
}

