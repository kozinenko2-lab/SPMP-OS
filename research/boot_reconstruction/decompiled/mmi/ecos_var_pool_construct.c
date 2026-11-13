/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004cdff0; original_symbol=ecos_var_pool_construct */

void ecos_var_pool_construct(undefined4 *param_1,uint param_2,uint param_3,uint param_4)

{
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint uVar1;
  undefined4 extraout_r2;
  uint extraout_r2_00;
  uint uVar2;
  int iVar3;
  undefined4 extraout_r3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  
  uVar1 = param_2;
  uVar2 = param_3;
  if (param_4 == 0) {
    FUN_002815bc(DAT_004ce14c,param_2,param_3,0);
    FUN_002815bc(DAT_004ce150,extraout_r1,extraout_r2,extraout_r3);
    uVar1 = extraout_r1_00;
    uVar2 = extraout_r2_00;
  }
  if ((param_4 - 1 & param_4) != 0) {
    FUN_002815bc(DAT_004ce154,uVar1,uVar2,param_4 - 1);
    uVar1 = extraout_r1_01;
  }
  if (param_3 < 0xc) {
    param_1[5] = 0;
  }
  else {
    param_1[3] = param_2;
    param_1[4] = param_3;
    param_1[7] = param_4;
    if ((int)param_4 < 0xc) {
      do {
        param_4 = param_4 * 2;
      } while ((int)param_4 < 0xc);
      param_1[7] = param_4;
    }
    uVar2 = param_1[7];
    if ((uVar2 - 1 & uVar2) != 0) {
      FUN_002815bc(DAT_004ce158,uVar1,uVar2,uVar2 - 1);
    }
    uVar1 = -param_1[7];
    uVar2 = (param_2 + param_1[7] + 0xb & uVar1) - 0xc;
    param_1[5] = uVar2;
    uVar4 = (param_2 + param_3 + 0xc & uVar1) - 0xc;
    param_1[6] = uVar4;
    if (uVar4 <= uVar2) {
      FUN_002815bc(DAT_004ce15c,uVar1,uVar2,uVar4);
      uVar1 = extraout_r1_02;
    }
    if (param_2 + param_3 < (uint)param_1[6]) {
      FUN_002815bc(DAT_004ce160,uVar1,param_2 + param_3,param_1[6]);
      uVar1 = extraout_r1_03;
    }
    if ((param_1[6] + 0xc & param_1[7] - 1) != 0) {
      FUN_002815bc(DAT_004ce164,uVar1,param_1[6] + 0xc,param_1[7] - 1);
      uVar1 = extraout_r1_04;
    }
    puVar6 = (undefined4 *)param_1[5];
    if (((uint)(puVar6 + 3) & param_1[7] - 1) != 0) {
      FUN_002815bc(DAT_004ce168,uVar1,puVar6 + 3,param_1[7] - 1);
    }
    param_1[1] = puVar6;
    *param_1 = puVar6;
    param_1[2] = 0;
    puVar6[1] = param_1;
    *puVar6 = param_1;
    iVar5 = param_1[6];
    iVar3 = param_1[5];
    puVar6[2] = iVar5 - iVar3;
    param_1[8] = iVar5 - iVar3;
  }
  (*DAT_004ce16c)(param_1 + 9);
  return;
}

