/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033c854; original_symbol=ecos_flag_setbits_core */

void ecos_flag_setbits_core(uint *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint uVar3;
  uint extraout_r1_03;
  uint uVar4;
  uint extraout_r2;
  int *extraout_r2_00;
  int *piVar5;
  int *extraout_r2_01;
  uint uVar6;
  uint *puVar7;
  undefined8 uVar8;
  int local_24;
  
  uVar3 = param_2;
  if (param_1 == (uint *)0x0) {
    FUN_002815bc(DAT_0033c9d4,param_2,param_3,param_4);
    uVar3 = extraout_r1;
  }
  piVar5 = DAT_0033c9d8;
  *DAT_0033c9d8 = *DAT_0033c9d8 + 1;
  *param_1 = param_2 | *param_1;
  if (param_1[1] != 0) {
    FUN_004cde00(&local_24);
    do {
      uVar8 = FUN_0033be68((int *)(param_1 + 1));
      uVar3 = (uint)((ulonglong)uVar8 >> 0x20);
      uVar1 = (uint)uVar8;
      puVar7 = *(uint **)(uVar1 + 0x44);
      uVar4 = *puVar7;
      uVar6 = 1 - puVar7[1];
      if (1 < puVar7[1]) {
        uVar6 = 0;
      }
      if (uVar4 != 0) {
        uVar6 = uVar6 ^ 1;
      }
      if (uVar6 != 0) {
        FUN_002815bc(DAT_0033c9dc,uVar3,uVar4,uVar6);
        uVar3 = extraout_r1_00;
        uVar4 = extraout_r2;
      }
      if (puVar7[2] != 0) {
        FUN_002815bc(DAT_0033c9e0,uVar3,uVar4,puVar7[2]);
        uVar3 = extraout_r1_01;
      }
      uVar4 = *puVar7;
      if (((uVar4 == 0) || ((uVar4 & *param_1) != uVar4)) && ((puVar7[1] & *param_1) == 0)) {
        FUN_0033be2c(&local_24,uVar1);
      }
      else {
        *(undefined4 *)(uVar1 + 0x78) = 0;
        *(undefined4 *)(uVar1 + 0x7c) = 7;
        FUN_0033a664(uVar1,uVar3);
        puVar7[2] = *param_1;
        if (puVar7[3] != 0) {
          *param_1 = 0;
        }
      }
    } while (param_1[1] != 0);
    while (local_24 != 0) {
      iVar2 = FUN_0033be68(&local_24);
      FUN_0033be2c((int *)(param_1 + 1),iVar2);
    }
    FUN_004ceba4(&local_24);
    uVar3 = extraout_r1_02;
    piVar5 = extraout_r2_00;
  }
  iVar2 = *DAT_0033c9d8 + -1;
  if (iVar2 == 0) {
    FUN_0033bf08(0,uVar3,piVar5);
  }
  else {
    if (iVar2 == 0) {
      FUN_002815bc(DAT_0033c9e4,uVar3,piVar5,*DAT_0033c9d8);
      uVar3 = extraout_r1_03;
      piVar5 = extraout_r2_01;
    }
    if (*DAT_0033c9d8 == 0) {
      FUN_002815bc(DAT_0033c9e8,uVar3,piVar5,0);
    }
    *DAT_0033c9d8 = iVar2;
  }
  return;
}

