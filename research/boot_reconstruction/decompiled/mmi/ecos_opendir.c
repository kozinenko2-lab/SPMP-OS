/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a69bc; original_symbol=ecos_opendir */

uint ecos_opendir(int param_1,uint param_2,undefined4 param_3)

{
  uint uVar1;
  undefined4 *puVar2;
  uint *puVar3;
  int iVar4;
  int *piVar5;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 extraout_r2_04;
  undefined4 extraout_r2_05;
  undefined4 uVar6;
  undefined4 extraout_r2_06;
  undefined4 extraout_r2_07;
  undefined4 extraout_r2_08;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  uint uVar7;
  undefined8 uVar8;
  uint local_28;
  int local_24;
  undefined4 local_20;

  local_28 = *DAT_002a6b40;
  local_20 = *DAT_002a6b44;
  local_24 = param_1;
  uVar1 = FUN_002a5bf0(1,param_2,param_3,local_20);
  if ((int)uVar1 < 0) {
    puVar2 = (undefined4 *)ecos_errno_location(uVar1,extraout_r1,extraout_r2);
    *puVar2 = 0x18;
    uVar1 = 0;
  }
  else {
    puVar3 = (uint *)FUN_002a5aa0(uVar1,extraout_r1);
    if (puVar3 == (uint *)0x0) {
      iVar4 = FUN_002a5cc8(uVar1,extraout_r1_00,extraout_r2_00,extraout_r3);
      puVar2 = (undefined4 *)ecos_errno_location(iVar4,extraout_r1_01,extraout_r2_01);
      *puVar2 = 0x17;
      uVar1 = 0;
    }
    else {
      iVar4 = FUN_00281f48(&local_20,&local_24,(int *)&local_28);
      if (iVar4 == 0) {
        uVar7 = extraout_r1_02;
        uVar6 = extraout_r2_02;
        if (local_28 == 0) {
          FUN_002815bc(DAT_002a6b48,extraout_r1_02,extraout_r2_02,0);
          uVar7 = extraout_r1_04;
          uVar6 = extraout_r2_04;
        }
        if (*(int *)(local_28 + 0x14) == 0) {
          FUN_002815bc(DAT_002a6b4c,uVar7,uVar6,0);
          uVar6 = extraout_r2_05;
        }
        FUN_0028228c(local_28,*(uint *)(*(int *)(local_28 + 0x14) + 8),uVar6,
                     *(int *)(local_28 + 0x14));
        iVar4 = (**(code **)(*(int *)(local_28 + 0x14) + 0x2c))(local_28,local_20,local_24,puVar3);
        FUN_00282378(local_28,*(uint *)(*(int *)(local_28 + 0x14) + 8),extraout_r2_06,
                     *(int *)(local_28 + 0x14));
        if (iVar4 == 0) {
          *puVar3 = *puVar3 | 0x20001;
          puVar3[7] = local_28;
          uVar7 = *(uint *)(*(int *)(local_28 + 0x14) + 8);
          puVar3[2] = uVar7;
          FUN_002a5c6c(uVar1,(uint)puVar3,extraout_r2_07,uVar7);
        }
        else {
          FUN_002a5cc8(uVar1,extraout_r1_05,extraout_r2_07,extraout_r3_01);
          uVar8 = FUN_002a5b0c(puVar3,extraout_r1_06);
          piVar5 = (int *)ecos_errno_location((int)uVar8,(uint)((ulonglong)uVar8 >> 0x20),
                                              extraout_r2_08);
          *piVar5 = iVar4;
          uVar1 = 0;
        }
      }
      else {
        FUN_002a5cc8(uVar1,extraout_r1_02,extraout_r2_02,extraout_r3_00);
        uVar8 = FUN_002a5b0c(puVar3,extraout_r1_03);
        puVar2 = (undefined4 *)
                 ecos_errno_location((int)uVar8,(uint)((ulonglong)uVar8 >> 0x20),extraout_r2_03);
        *puVar2 = 2;
        uVar1 = 0;
      }
    }
  }
  return uVar1;
}
