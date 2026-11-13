/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00046220; original_symbol=rb_go */

void rb_go(uint *param_1,uint param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  uint uVar5;
  uint *puVar6;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 extraout_r2_04;
  undefined4 extraout_r2_05;
  undefined4 extraout_r2_06;
  undefined4 extraout_r2_07;
  uint extraout_r2_08;
  undefined4 extraout_r2_09;
  uint uVar7;
  uint extraout_r3;
  undefined4 *puVar8;
  uint *puVar9;
  undefined1 uVar10;
  undefined4 in_cr5;
  undefined4 in_cr8;
  undefined8 uVar11;
  uint auStack_b8 [2];
  byte abStack_b0 [24];
  undefined1 auStack_98 [24];
  undefined1 auStack_80 [24];
  undefined1 auStack_68 [24];
  undefined1 auStack_50 [24];
  int local_38;
  int local_34;
  int local_30;
  uint local_2c;
  int local_28;
  int local_24;
  int local_20;
  uint local_1c;
  
  puVar8 = DAT_00046718;
  local_28 = 0;
  local_2c = 0;
  local_38 = 0;
  *DAT_00046718 = 0xaa;
  puVar8[1] = 0x55;
  FUN_0004563c(DAT_0004671c,param_2,puVar8,0x55);
  *DAT_00046720 = 0;
  local_1c = *DAT_00046724;
  FUN_00047f68(abStack_b0,0x77,1,0,&local_24,&local_20,DAT_00046728);
  FUN_00047f68(auStack_98,99,0,2,&local_28,0,DAT_0004672c);
  FUN_00047f68(auStack_80,0x6e,0,2,&local_38,0,DAT_00046730);
  FUN_00047f68(auStack_68,0x72,1,0,&local_2c,&local_30,DAT_00046734);
  FUN_00047f68(auStack_50,100,0,2,&local_34,0,DAT_00046738);
  iVar3 = FUN_000480cc(param_1,param_2,1,abStack_b0,&DAT_00000005,&local_1c,(uint *)0x0,DAT_0004673c
                      );
  if (iVar3 == 0) {
    if ((*(uint *)*DAT_00046740 & 1) != 0) {
      FUN_0004563c(DAT_00046744,extraout_r1,extraout_r2,*(uint *)*DAT_00046740);
    }
  }
  else if (local_1c == 0xffffffff) {
    FUN_0004563c(DAT_00046748,extraout_r1,extraout_r2,0xffffffff);
  }
  else {
    FUN_0004563c(DAT_0004674c,local_2c,local_30,local_1c);
    uVar1 = DAT_00046758;
    uVar4 = DAT_00046750;
    puVar8 = DAT_00046718;
    if ((local_2c == 0) || (local_30 == 0)) {
      *DAT_00046718 = DAT_00046750;
      FUN_0004563c(DAT_00046754,extraout_r1_00,uVar4,puVar8);
      uVar5 = extraout_r1_01;
      uVar4 = extraout_r2_00;
    }
    else if (local_2c == 1) {
      *DAT_00046718 = DAT_00046758;
      FUN_0004563c(DAT_0004675c,extraout_r1_00,uVar1,puVar8);
      uVar5 = extraout_r1_02;
      uVar4 = extraout_r2_01;
    }
    else {
      FUN_0004563c(DAT_00046760,extraout_r1_00,local_2c,local_30);
      uVar5 = extraout_r1_03;
      uVar4 = extraout_r2_02;
    }
    uVar1 = DAT_00046758;
    puVar8 = DAT_00046718;
    if (local_34 != 0) {
      DAT_00046718[1] = DAT_00046758;
      FUN_0004563c(DAT_00046764,uVar5,uVar1,puVar8);
      uVar5 = extraout_r1_04;
      uVar4 = extraout_r2_03;
    }
    if (local_20 != 0) {
      iVar3 = local_24 * 1000;
      FUN_0004563c(DAT_00046768,local_1c,local_24,local_24 * 0x7d);
      uVar5 = extraout_r1_05;
      uVar4 = extraout_r2_04;
      for (; 9 < iVar3; iVar3 = iVar3 + -10) {
        uVar11 = FUN_00047e88(auStack_b8,8,10);
        uVar5 = (uint)((ulonglong)uVar11 >> 0x20);
        if ((int)uVar11 == -2) {
          return;
        }
        uVar4 = extraout_r2_05;
      }
    }
    puVar9 = (uint *)*DAT_00046740;
    if ((*puVar9 & 1) != 0) {
      FUN_0004563c(DAT_0004676c,uVar5,uVar4,*puVar9);
    }
    uVar4 = (**(code **)(DAT_00046770 + 0x34))(0xffffffff);
    iVar3 = 0;
    do {
      iVar2 = DAT_00046770;
      (**(code **)(DAT_00046770 + 0x34))(iVar3);
      (*(code *)(*(undefined4 **)(iVar2 + 0x14))[5])(**(undefined4 **)(iVar2 + 0x14),4);
      iVar3 = iVar3 + 1;
    } while (iVar3 != 2);
    (**(code **)(iVar2 + 0x34))(uVar4);
    puVar8 = *(undefined4 **)(iVar2 + 0x14);
    uVar11 = (*(code *)puVar8[5])(*puVar8,9);
    uVar5 = (uint)((ulonglong)uVar11 >> 0x20);
    uVar4 = extraout_r2_06;
    if (local_38 != 0) {
      FUN_0003fe40((int)uVar11,uVar5,extraout_r2_06);
      uVar5 = extraout_r1_06;
      uVar4 = extraout_r2_07;
    }
    uVar10 = (*puVar9 & 1) == 0;
    if (!(bool)uVar10) {
      FUN_0004563c(DAT_00046774,uVar5,uVar4,*puVar9);
    }
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (!(bool)uVar10);
    coproc_moveto_Data_Synchronization(0);
    if (local_28 == 0) {
      uVar5 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar5 & 0xffffefff);
      coproc_moveto_Invalidate_Entire_Instruction(0);
      uVar5 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar5 & 0xfffffffb);
      coproc_moveto_Invalidate_Entire_Data_cache(0);
      coproc_movefrom_Data_cache_operations(3);
      coproc_moveto_Data_Synchronization(0);
    }
    coproc_moveto_Invalidate_Entire_Instruction(0);
    coprocessor_moveto(0xf,0,0,0,in_cr8,in_cr5);
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (local_28 != 0);
    coproc_moveto_Data_Synchronization(0);
    coproc_moveto_Invalidate_Entire_Data_cache(0);
    coproc_moveto_Invalidate_unified_TLB_unlocked(0);
    if ((*puVar9 & 1) != 0) {
      FUN_0004563c(DAT_00046778,*DAT_0004677c,local_1c,DAT_0004677c);
    }
    uVar5 = *DAT_0004677c & 0xfffffff0;
    puVar6 = (uint *)(uVar5 - 0x50);
    uVar7 = 0;
    do {
      puVar6[uVar7] = uVar7;
      uVar7 = uVar7 + 1;
    } while (uVar7 != 0xb);
    *puVar6 = local_1c;
    *(uint *)(uVar5 - 0x1c) = uVar5;
    uVar4 = DAT_00046780;
    *(undefined4 *)(uVar5 - 0x18) = DAT_00046780;
    *(undefined4 *)(uVar5 - 0x14) = uVar4;
    *(undefined4 *)(uVar5 - 0x10) = 0x13;
    *DAT_0004677c = (uint)puVar6;
    uVar7 = *puVar9;
    if ((uVar7 & 1) != 0) {
      FUN_0004563c(DAT_00046784,(uint)puVar6,uVar5,uVar7);
      uVar5 = extraout_r2_08;
      uVar7 = extraout_r3;
    }
    FUN_0004ab00(DAT_0004677c,DAT_00046788,uVar5,uVar7);
    if (local_28 == 0) {
      uVar5 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar5 | 0x1003);
      uVar5 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar5 | 7);
    }
    coproc_moveto_Invalidate_Entire_Instruction(0);
    coprocessor_moveto(0xf,0,0,0,in_cr8,in_cr5);
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (local_28 != 0);
    coproc_moveto_Data_Synchronization(0);
    coproc_moveto_Invalidate_Entire_Data_cache(0);
    coproc_moveto_Invalidate_unified_TLB_unlocked(0);
    (*(code *)puVar8[5])(*puVar8,10);
    FUN_0004563c(DAT_0004678c,*DAT_00046790,extraout_r2_09,DAT_00046790);
  }
  return;
}

