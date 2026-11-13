/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0004b304; original_symbol=rb_go */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void rb_go(uint *param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  uint uVar4;
  uint *puVar5;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 extraout_r2_04;
  undefined4 extraout_r2_05;
  undefined4 extraout_r2_06;
  undefined4 extraout_r2_07;
  undefined4 extraout_r2_08;
  uint extraout_r2_09;
  undefined4 extraout_r2_10;
  uint uVar6;
  uint extraout_r3;
  int iVar7;
  undefined4 *puVar8;
  undefined1 uVar9;
  undefined4 in_cr5;
  undefined4 in_cr8;
  undefined8 uVar10;
  uint local_c8;
  int local_c4;
  int local_c0;
  uint local_bc;
  int local_b8;
  int local_b4;
  int local_b0;
  int local_ac;
  uint auStack_a8 [2];
  byte abStack_a0 [24];
  undefined1 auStack_88 [24];
  undefined1 auStack_70 [24];
  undefined1 auStack_58 [24];
  undefined1 auStack_40 [24];
  
  local_b4 = 0;
  local_bc = 0;
  local_b8 = 0;
  _DAT_9d801000 = 0xaa;
  _DAT_9d801004 = 0x55;
  FUN_00049e4c(DAT_0004b83c,param_2,param_3,0x55);
  *DAT_0004b840 = 0;
  local_c8 = *DAT_0004b844;
  FUN_0004cce0(abStack_a0,0x77,1,0,&local_ac,&local_b0,DAT_0004b848);
  FUN_0004cce0(auStack_88,99,0,2,&local_b4,0,DAT_0004b84c);
  FUN_0004cce0(auStack_70,0x6e,0,2,&local_b8,0,DAT_0004b850);
  FUN_0004cce0(auStack_58,0x72,1,0,&local_bc,&local_c0,DAT_0004b854);
  FUN_0004cce0(auStack_40,100,0,2,&local_c4,0,DAT_0004b858);
  iVar2 = FUN_0004cd04(param_1,param_2,(uint *)&DAT_00000001,abStack_a0,5,(int *)&local_c8,0,
                       DAT_0004b85c);
  if (iVar2 == 0) {
    if ((*(uint *)*DAT_0004b860 & 1) != 0) {
      FUN_00049e4c(DAT_0004b864,extraout_r1,extraout_r2,*(uint *)*DAT_0004b860);
    }
  }
  else if (local_c8 == 0xffffffff) {
    FUN_00049e4c(DAT_0004b868,extraout_r1,extraout_r2,0xffffffff);
  }
  else {
    FUN_00049e4c(DAT_0004b86c,local_bc,local_c0,local_c8);
    if ((local_bc == 0) || (local_c0 == 0)) {
      _DAT_9d801000 = 0xaa55;
      FUN_00049e4c(DAT_0004b870,extraout_r1_00,extraout_r2_00,0xaa55);
      uVar4 = extraout_r1_01;
      uVar3 = extraout_r2_01;
    }
    else if (local_bc == 1) {
      _DAT_9d801000 = 0x55aa;
      FUN_00049e4c(DAT_0004b874,extraout_r1_00,extraout_r2_00,0x55aa);
      uVar4 = extraout_r1_02;
      uVar3 = extraout_r2_02;
    }
    else {
      FUN_00049e4c(DAT_0004b878,extraout_r1_00,extraout_r2_00,local_bc);
      uVar4 = extraout_r1_03;
      uVar3 = extraout_r2_03;
    }
    if (local_c4 != 0) {
      _DAT_9d801004 = 0x55aa;
      FUN_00049e4c(DAT_0004b87c,uVar4,uVar3,0x55aa);
      uVar4 = extraout_r1_04;
      uVar3 = extraout_r2_04;
    }
    if (local_b0 != 0) {
      iVar2 = local_ac * 1000;
      FUN_00049e4c(DAT_0004b880,local_c8,local_ac,local_ac * 0x7d);
      uVar4 = extraout_r1_05;
      uVar3 = extraout_r2_05;
      for (; 9 < iVar2; iVar2 = iVar2 + -10) {
        uVar10 = FUN_0004c738(auStack_a8,8,10);
        uVar4 = (uint)((ulonglong)uVar10 >> 0x20);
        if ((int)uVar10 == -2) {
          return;
        }
        uVar3 = extraout_r2_06;
      }
    }
    if ((*(uint *)*DAT_0004b860 & 1) != 0) {
      FUN_00049e4c(DAT_0004b884,uVar4,uVar3,*(uint *)*DAT_0004b860);
    }
    uVar3 = (**(code **)(DAT_0004b888 + 0x34))(0xffffffff);
    iVar2 = DAT_0004b888;
    iVar7 = 0;
    do {
      (**(code **)(iVar2 + 0x34))(iVar7);
      puVar8 = *(undefined4 **)(iVar2 + 0x14);
      (*(code *)puVar8[5])(*puVar8,4);
      iVar1 = DAT_0004b888;
      iVar7 = iVar7 + 1;
    } while (iVar7 < 2);
    (**(code **)(DAT_0004b888 + 0x34))(uVar3);
    puVar8 = *(undefined4 **)(iVar1 + 0x14);
    uVar10 = (*(code *)puVar8[5])(*puVar8,9);
    uVar4 = (uint)((ulonglong)uVar10 >> 0x20);
    uVar3 = extraout_r2_07;
    if (local_b8 != 0) {
      FUN_00044328((int)uVar10,uVar4,extraout_r2_07);
      uVar4 = extraout_r1_06;
      uVar3 = extraout_r2_08;
    }
    uVar9 = (*(uint *)*DAT_0004b860 & 1) == 0;
    if (!(bool)uVar9) {
      FUN_00049e4c(DAT_0004b88c,uVar4,uVar3,*(uint *)*DAT_0004b860);
    }
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (!(bool)uVar9);
    coproc_moveto_Data_Synchronization(0);
    if (local_b4 == 0) {
      uVar4 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar4 & 0xffffefff);
      coproc_moveto_Invalidate_Entire_Instruction(0);
      uVar4 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar4 & 0xfffffffb);
      coproc_moveto_Invalidate_Entire_Data_cache(0);
      coproc_movefrom_Data_cache_operations(3);
      coproc_moveto_Data_Synchronization(0);
    }
    coproc_moveto_Invalidate_Entire_Instruction(0);
    coprocessor_moveto(0xf,0,0,0,in_cr8,in_cr5);
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (local_b4 != 0);
    coproc_moveto_Data_Synchronization(0);
    coproc_moveto_Invalidate_Entire_Data_cache(0);
    coproc_moveto_Invalidate_unified_TLB_unlocked(0);
    if ((*(uint *)*DAT_0004b860 & 1) != 0) {
      FUN_00049e4c(DAT_0004b890,*DAT_0004b894,local_c8,DAT_0004b894);
    }
    uVar4 = *DAT_0004b894 & 0xfffffff0;
    puVar5 = (uint *)(uVar4 - 0x50);
    uVar6 = 0;
    do {
      puVar5[uVar6] = uVar6;
      uVar6 = uVar6 + 1;
    } while ((int)uVar6 < 0xb);
    *puVar5 = local_c8;
    *(uint *)(uVar4 - 0x1c) = uVar4;
    uVar3 = DAT_0004b898;
    *(undefined4 *)(uVar4 - 0x18) = DAT_0004b898;
    *(undefined4 *)(uVar4 - 0x14) = uVar3;
    *(undefined4 *)(uVar4 - 0x10) = 0x13;
    *DAT_0004b894 = (uint)puVar5;
    uVar6 = *(uint *)*DAT_0004b860;
    if ((uVar6 & 1) != 0) {
      FUN_00049e4c(DAT_0004b89c,(uint)puVar5,uVar4,uVar6);
      uVar4 = extraout_r2_09;
      uVar6 = extraout_r3;
    }
    FUN_0004f7ec(DAT_0004b894,DAT_0004b8a0,uVar4,uVar6);
    if (local_b4 == 0) {
      uVar4 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar4 | 0x1003);
      uVar4 = coproc_movefrom_Control();
      coproc_moveto_Control(uVar4 | 7);
    }
    coproc_moveto_Invalidate_Entire_Instruction(0);
    coprocessor_moveto(0xf,0,0,0,in_cr8,in_cr5);
    do {
      coproc_movefrom_Data_cache_operations(3);
    } while (local_b4 != 0);
    coproc_moveto_Data_Synchronization(0);
    coproc_moveto_Invalidate_Entire_Data_cache(0);
    coproc_moveto_Invalidate_unified_TLB_unlocked(0);
    (*(code *)puVar8[5])(*puVar8,10);
    FUN_00049e4c(DAT_0004b8a4,*DAT_0004b8a8,extraout_r2_10,DAT_0004b8a8);
  }
  return;
}

