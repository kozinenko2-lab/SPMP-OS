/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003be70c; original_symbol=mmi_gui_init_impl */

undefined4 mmi_gui_init_impl(void)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int *piVar5;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint uVar6;
  int iVar7;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  int iVar8;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  int iVar9;
  undefined8 uVar10;
  undefined4 auStack_78 [2];
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  
  iVar2 = DAT_003be98c;
  iVar7 = 0;
  do {
    *(undefined4 *)(iVar2 + iVar7 * 0x10) = 0;
    iVar8 = iVar2 + iVar7 * 0x10;
    *(undefined4 *)(iVar8 + 4) = 0;
    *(undefined4 *)(iVar8 + 8) = 0;
    *(undefined4 *)(iVar8 + 0xc) = 0xffffffff;
    iVar7 = iVar7 + 1;
  } while (iVar7 < 0x100);
  *DAT_003be990 = 0xffffffff;
  uVar10 = FUN_004be7ec(iVar2,0);
  uVar6 = DAT_003be998;
  if ((int)uVar10 == 0) {
    puVar1 = (undefined4 *)FUN_00299a14(0,(uint)((ulonglong)uVar10 >> 0x20),extraout_r2);
    *puVar1 = 4;
  }
  else {
    *(undefined4 *)(DAT_003be998 + 0x3c) = DAT_003be994;
    FUN_004be808((int)uVar10,uVar6);
  }
  uVar10 = FUN_003bea0c();
  *DAT_003be99c = (int)uVar10;
  uVar10 = FUN_003d6990();
  piVar4 = DAT_003be9a0;
  *DAT_003be9a0 = (int)uVar10;
  if ((int)uVar10 == -1) {
    FUN_003bc13c(DAT_003be9a4,(uint)((ulonglong)uVar10 >> 0x20),extraout_r2_00,piVar4);
    uVar3 = 0xffffffff;
  }
  else {
    iVar2 = FUN_003b9fbc();
    if (iVar2 == 0) {
      FUN_003bc13c(DAT_003be9a8,extraout_r1,extraout_r2_01,extraout_r3);
      FUN_003d69b4();
      uVar3 = 0xffffffff;
    }
    else {
      local_70 = 0xb;
      local_6c = DAT_003be9ac;
      local_68 = 0;
      local_64 = 0;
      local_60 = 0;
      local_5c = 0;
      local_58 = 0;
      iVar7 = FUN_003c1b74(1);
      local_54 = FUN_003c1fc4(iVar7,extraout_r1_00);
      local_50 = 0;
      local_4c = DAT_003be9b0;
      FUN_003bf5ac(auStack_78);
      piVar4 = FUN_00296064(DAT_003be9b4,extraout_r1_01);
      if (piVar4 == (int *)0x0) {
        FUN_003bc13c(DAT_003be9b8,extraout_r1_02,extraout_r2_02,extraout_r3_00);
        FUN_003b985c();
        FUN_003ba174(iVar2);
        FUN_003d69b4();
        uVar3 = 0xffffffff;
      }
      else {
        piVar4[0xc] = 0x12000000;
        piVar4[0xd] = 0;
        piVar4[0xe] = *DAT_003be9bc;
        piVar4[0xf] = 0;
        piVar4[0x11] = 0;
        piVar4[0x12] = 0;
        piVar4[0x13] = 0;
        FUN_003c02f8(piVar4,0,0,*(undefined4 *)(iVar2 + 8),*(undefined4 *)(iVar2 + 0xc));
        FUN_003c0148(piVar4);
        piVar4[0x14] = 0;
        piVar4[0x16] = 0;
        piVar4[0x17] = 0;
        iVar7 = *DAT_003be9c0;
        uVar6 = DAT_003be9c0[1];
        iVar8 = DAT_003be9c0[2];
        iVar9 = DAT_003be9c0[3];
        piVar4[0x18] = iVar7;
        piVar4[0x19] = uVar6;
        piVar4[0x1a] = iVar8;
        *(char *)(piVar4 + 0x1b) = (char)iVar9;
        piVar4[0x28] = 0;
        piVar5 = FUN_003bab58(iVar7,uVar6);
        piVar4[0x2b] = (int)piVar5;
        *DAT_003be9c4 = piVar4;
        piVar5 = DAT_003be9c8;
        *DAT_003be9c8 = (int)piVar4;
        *DAT_003be9cc = piVar4;
        *DAT_003be9d0 = piVar4;
        FUN_003bfdb4(piVar4,(int *)0x0,1);
        *DAT_003be9d4 = 0;
        *DAT_003be9d8 = 0xffffffff;
        *DAT_003be9dc = 0xffffffff;
        FUN_003c3d0c(*(int *)(iVar2 + 8) / 2,*(int *)(iVar2 + 0xc) / 2);
        FUN_003c09bc(*piVar5,DAT_003be9e0);
        FUN_003b98a4(0,0,*(int *)(iVar2 + 8) + -1,*(int *)(iVar2 + 0xc) + -1);
        FUN_003b9900(*(int *)(iVar2 + 8) / 2,*(int *)(iVar2 + 0xc) / 2);
        uVar3 = 0;
      }
    }
  }
  return uVar3;
}

