/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004ce478; original_symbol=ecos_var_pool_free */

/* WARNING: Removing unreachable block (ram,0x004ce804) */
/* WARNING: Removing unreachable block (ram,0x004ce80c) */
/* WARNING: Removing unreachable block (ram,0x004ce818) */
/* WARNING: Removing unreachable block (ram,0x004ce820) */
/* WARNING: Removing unreachable block (ram,0x004ce824) */
/* WARNING: Removing unreachable block (ram,0x004ce844) */

undefined4 ecos_var_pool_free(int *param_1,int *param_2,int param_3)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  int *extraout_r1_07;
  int *extraout_r1_08;
  int *extraout_r1_09;
  int *extraout_r1_10;
  int *extraout_r2;
  uint *puVar4;
  undefined4 extraout_r2_00;
  int *extraout_r2_01;
  int *piVar5;
  int *extraout_r2_02;
  int *extraout_r2_03;
  int *extraout_r2_04;
  int *extraout_r2_05;
  int *extraout_r2_06;
  int *extraout_r2_07;
  int *extraout_r2_08;
  int iVar6;
  undefined4 *puVar7;
  int *piVar8;
  uint *puVar9;
  int *piVar10;
  uint uVar11;
  int *piVar12;
  undefined8 uVar13;
  int local_30;
  
  piVar12 = DAT_004ce89c;
  iVar6 = *DAT_004ce89c;
  *DAT_004ce89c = iVar6 + 1;
  if (param_1 == (int *)0x0) {
    FUN_002815bc(DAT_004ce8a0,(uint)param_2,piVar12,iVar6 + 1);
    piVar12 = extraout_r2;
  }
  piVar10 = param_2;
  if ((param_2 < (int *)param_1[5]) || ((int *)param_1[6] < param_2)) {
    uVar1 = 0;
  }
  else {
    puVar9 = (uint *)(param_2 + -3);
    piVar12 = (int *)param_2[-2];
    if ((piVar12 == (int *)param_2[-3]) && (piVar12 == (int *)0xd530d53)) {
      if (param_3 == 0) {
        uVar2 = param_2[-1];
      }
      else {
        piVar12 = (int *)-param_1[7];
        uVar2 = param_3 + param_1[7] + 0xbU & (uint)piVar12;
      }
      if (param_2[-1] == uVar2) {
        if ((int)uVar2 < 0xc) {
          FUN_002815bc(DAT_004ce8a4,(uint)param_2,piVar12,param_2[-1]);
        }
        param_1[8] = param_1[8] + param_2[-1];
        for (puVar4 = (uint *)param_1[1]; (puVar4 != (uint *)param_1 && (puVar4 <= puVar9));
            puVar4 = (uint *)puVar4[1]) {
        }
        param_2[-2] = (int)puVar4;
        *puVar9 = *puVar4;
        *puVar4 = (uint)puVar9;
        *(uint **)(*puVar9 + 4) = puVar9;
        if ((int *)param_2[-2] != param_1) {
          piVar12 = (int *)((int)puVar9 + param_2[-1]);
          if (piVar12 == (int *)param_2[-2]) {
            param_2[-1] = param_2[-1] + piVar12[2];
            puVar7 = (undefined4 *)piVar12[1];
            param_2[-2] = (int)puVar7;
            *puVar7 = puVar9;
          }
        }
        piVar12 = (int *)*puVar9;
        piVar10 = param_1;
        if ((piVar12 != param_1) &&
           (piVar10 = (int *)piVar12[2], (uint *)((int)piVar12 + (int)piVar10) == puVar9)) {
          piVar10 = *(int **)((int)piVar12 + (int)piVar10);
          piVar12 = (int *)param_2[-1];
          piVar10[2] = piVar10[2] + (int)piVar12;
          piVar10[1] = param_2[-2];
          *(int **)param_2[-2] = piVar10;
        }
        uVar1 = 1;
      }
      else {
        uVar1 = 0;
      }
    }
    else {
      uVar1 = 0;
    }
  }
  if (param_1[9] != 0) {
    (*DAT_004ce8a8)(&local_30);
    do {
      uVar13 = FUN_0033be68(param_1 + 9);
      uVar2 = (uint)((ulonglong)uVar13 >> 0x20);
      uVar3 = (uint)uVar13;
      piVar12 = *(int **)(uVar3 + 0x44);
      if (piVar12[1] != 0) {
        FUN_002815bc(DAT_004ce8ac,uVar2,extraout_r2_00,piVar12[1]);
        uVar2 = extraout_r1;
      }
      if (param_1[5] == 0) {
        piVar10 = (int *)0x0;
      }
      else {
        piVar5 = (int *)-param_1[7];
        uVar11 = *piVar12 + param_1[7] + 0xbU & (uint)piVar5;
        piVar10 = param_1;
        do {
          if (*(int **)piVar10[1] != piVar10) {
            FUN_002815bc(DAT_004ce8b0,uVar2,piVar5,*(int **)piVar10[1]);
            uVar2 = extraout_r1_00;
            piVar5 = extraout_r2_01;
          }
          piVar10 = (int *)piVar10[1];
          if (piVar10[2] == 0) {
            if (piVar10 != param_1) {
              FUN_002815bc(DAT_004ce8b4,uVar2,piVar5,0);
              uVar2 = extraout_r1_01;
            }
            piVar10 = (int *)0x0;
            goto LAB_004ce784;
          }
        } while (piVar10[2] < (int)uVar11);
        if (piVar10[2] == uVar11) {
          iVar6 = *piVar10;
          *(int *)(iVar6 + 4) = piVar10[1];
          piVar5 = (int *)piVar10[1];
          *piVar5 = iVar6;
        }
        else {
          if (piVar10[2] <= (int)uVar11) {
            FUN_002815bc(DAT_004ce8b8,uVar2,piVar5,piVar10[2]);
            uVar2 = extraout_r1_02;
            piVar5 = extraout_r2_02;
          }
          iVar6 = piVar10[2] - uVar11;
          piVar10[2] = iVar6;
          if (iVar6 < 0xc) {
            FUN_002815bc(DAT_004ce8a4,uVar2,piVar5,iVar6);
            uVar2 = extraout_r1_03;
            piVar5 = extraout_r2_03;
          }
          piVar10 = (int *)((int)piVar10 + piVar10[2]);
        }
        piVar8 = (int *)param_1[5];
        if ((piVar10 < piVar8) || (piVar8 = (int *)param_1[6], piVar8 < piVar10)) {
          FUN_002815bc(DAT_004ce8bc,uVar2,piVar5,piVar8);
          uVar2 = extraout_r1_04;
          piVar5 = extraout_r2_04;
        }
        piVar10[2] = uVar11;
        *piVar10 = 0xd530d53;
        piVar10[1] = 0xd530d53;
        param_1[8] = param_1[8] - uVar11;
        piVar10 = piVar10 + 3;
        if ((param_1[7] - 1U & (uint)piVar10) != 0) {
          FUN_002815bc(DAT_004ce8c0,uVar2,piVar5,param_1[7] - 1U);
          uVar2 = extraout_r1_05;
        }
        if (piVar10 == (int *)0x0) {
          FUN_002e3720();
          uVar2 = extraout_r1_06;
        }
      }
LAB_004ce784:
      if (piVar10 == (int *)0x0) {
        FUN_0033be2c(&local_30,uVar3);
        piVar10 = extraout_r1_08;
        piVar12 = extraout_r2_06;
      }
      else {
        *(undefined4 *)(uVar3 + 0x78) = 0;
        *(undefined4 *)(uVar3 + 0x7c) = 7;
        FUN_0033a664(uVar3,uVar2);
        piVar12[1] = (int)piVar10;
        piVar10 = extraout_r1_07;
        piVar12 = extraout_r2_05;
      }
    } while (param_1[9] != 0);
    while (local_30 != 0) {
      iVar6 = FUN_0033be68(&local_30);
      FUN_0033be2c(param_1 + 9,iVar6);
      piVar10 = extraout_r1_09;
      piVar12 = extraout_r2_07;
    }
    local_30 = 0;
  }
  iVar6 = *DAT_004ce89c + -1;
  if (iVar6 == 0) {
    FUN_0033bf08(0,piVar10,piVar12);
  }
  else {
    if (iVar6 == 0) {
      FUN_002815bc(DAT_004ce8c8,(uint)piVar10,piVar12,*DAT_004ce89c);
      piVar10 = extraout_r1_10;
      piVar12 = extraout_r2_08;
    }
    if (*DAT_004ce89c == 0) {
      FUN_002815bc(DAT_004ce8cc,(uint)piVar10,piVar12,0);
    }
    *DAT_004ce89c = iVar6;
  }
  return uVar1;
}

