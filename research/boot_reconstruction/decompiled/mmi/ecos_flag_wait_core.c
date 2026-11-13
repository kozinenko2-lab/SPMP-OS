/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033c9ec; original_symbol=ecos_flag_wait_core */

int * ecos_flag_wait_core(uint *param_1,uint param_2,uint param_3,undefined4 param_4)

{
  int *piVar1;
  int *piVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  undefined4 extraout_r1_02;
  uint uVar3;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r2;
  int *extraout_r2_00;
  int *extraout_r2_01;
  int *piVar4;
  int iVar5;
  int *extraout_r2_02;
  uint *puVar6;
  uint *extraout_r2_03;
  uint *extraout_r2_04;
  uint *extraout_r2_05;
  uint *extraout_r2_06;
  undefined4 extraout_r3;
  int iVar7;
  uint uVar8;
  longlong lVar9;
  uint local_34;
  uint local_30;
  int *local_2c;
  uint local_28;
  
  uVar8 = param_2;
  uVar3 = param_3;
  if (param_1 == (uint *)0x0) {
    FUN_002815bc(DAT_0033cbd4,param_2,param_3,param_4);
    uVar8 = extraout_r1;
    uVar3 = extraout_r2;
    param_4 = extraout_r3;
  }
  if (3 < (param_3 & 0xff)) {
    FUN_002815bc(DAT_0033cbd8,uVar8,uVar3,param_4);
  }
  piVar1 = DAT_0033cbdc;
  iVar7 = *DAT_0033cbdc;
  *DAT_0033cbdc = iVar7 + 1;
  piVar2 = ecos_flag_poll_core(param_1,param_2,param_3 & 0xff,iVar7 + 1);
  if (piVar2 != (int *)0x0) {
    iVar7 = *piVar1 + -1;
    if (iVar7 == 0) {
      FUN_0033bf08(0,extraout_r1_00,extraout_r2_00);
    }
    else {
      uVar8 = extraout_r1_00;
      piVar4 = extraout_r2_00;
      if (iVar7 == 0) {
        FUN_002815bc(DAT_0033cbe0,extraout_r1_00,extraout_r2_00,*piVar1);
        uVar8 = extraout_r1_01;
        piVar4 = extraout_r2_01;
      }
      if (*DAT_0033cbdc == 0) {
        FUN_002815bc(DAT_0033cbe4,uVar8,piVar4,0);
      }
      *DAT_0033cbdc = iVar7;
    }
    return piVar2;
  }
  iVar7 = *DAT_0033cbe8;
  local_34 = 0;
  local_2c = (int *)0x0;
  local_30 = param_2;
  if ((param_3 & 2) == 0) {
    local_30 = 0;
    local_34 = param_2;
  }
  local_28 = param_3 & 1;
  *(uint **)(iVar7 + 0x44) = &local_34;
  piVar1 = DAT_0033cbe8;
  uVar8 = 1;
  lVar9 = (ulonglong)extraout_r1_00 << 0x20;
  do {
    *(undefined4 *)(*piVar1 + 0x78) = 1;
    iVar5 = *piVar1;
    *(undefined4 *)(iVar5 + 0x7c) = 0;
    FUN_0033a5b8((int)lVar9,(uint)((ulonglong)lVar9 >> 0x20),iVar5);
    FUN_0033be2c((int *)(param_1 + 1),iVar7);
    lVar9 = FUN_0033bf08(*DAT_0033cbdc,extraout_r1_02,extraout_r2_02);
    puVar6 = *(uint **)(*piVar1 + 0x44);
    if (&local_34 != puVar6) {
      lVar9 = FUN_002815bc(DAT_0033cbec,(uint)((ulonglong)lVar9 >> 0x20),puVar6,&local_34);
      puVar6 = extraout_r2_03;
    }
    iVar5 = *(int *)(iVar7 + 0x7c);
    if (3 < iVar5) {
      if (iVar5 < 6) {
        uVar8 = 0;
      }
      else if (iVar5 == 6) {
        lVar9 = ecos_thread_exit_core();
        puVar6 = extraout_r2_04;
      }
    }
    uVar3 = (uint)((ulonglong)lVar9 >> 0x20);
  } while ((uVar8 != 0) && (local_2c == (int *)0x0));
  iVar7 = 1 - uVar8;
  if (1 < uVar8) {
    iVar7 = 0;
  }
  if (local_2c == (int *)0x0) {
    if (iVar7 != 0) goto LAB_0033cb88;
  }
  else if (iVar7 == 0) goto LAB_0033cb88;
  FUN_002815bc(DAT_0033cbf0,uVar3,puVar6,local_2c);
  uVar3 = extraout_r1_03;
  puVar6 = extraout_r2_05;
LAB_0033cb88:
  iVar7 = *DAT_0033cbdc + -1;
  if (iVar7 == 0) {
    FUN_0033bf08(0,uVar3,(int *)puVar6);
  }
  else {
    if (iVar7 == 0) {
      FUN_002815bc(DAT_0033cbe0,uVar3,puVar6,*DAT_0033cbdc);
      uVar3 = extraout_r1_04;
      puVar6 = extraout_r2_06;
    }
    if (*DAT_0033cbdc == 0) {
      FUN_002815bc(DAT_0033cbe4,uVar3,puVar6,0);
    }
    *DAT_0033cbdc = iVar7;
  }
  return local_2c;
}

