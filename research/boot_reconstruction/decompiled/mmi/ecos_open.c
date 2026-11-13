/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a5ed4; original_symbol=ecos_open */

uint ecos_open(int param_1,uint param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint uVar5;
  uint extraout_r1_05;
  uint extraout_r1_06;
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
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  undefined4 uVar6;
  undefined8 uVar7;
  int local_34;
  int local_30;
  undefined4 local_2c;
  
  local_34 = *DAT_002a606c;
  local_2c = *DAT_002a6070;
  local_30 = param_1;
  if ((param_2 & 3) == 0) {
    puVar1 = (undefined4 *)FUN_00299a14(param_1,param_2,param_3);
    *puVar1 = 0x16;
    uVar2 = 0xffffffff;
  }
  else {
    uVar2 = FUN_002a5bf0(0,param_2,param_3,local_2c);
    if ((int)uVar2 < 0) {
      puVar1 = (undefined4 *)FUN_00299a14(uVar2,extraout_r1,extraout_r2);
      *puVar1 = 0x18;
      uVar2 = 0xffffffff;
    }
    else {
      puVar1 = (undefined4 *)FUN_002a5aa0(uVar2,extraout_r1);
      if (puVar1 == (undefined4 *)0x0) {
        iVar3 = FUN_002a5cc8(uVar2,extraout_r1_00,extraout_r2_00,extraout_r3);
        puVar1 = (undefined4 *)FUN_00299a14(iVar3,extraout_r1_01,extraout_r2_01);
        *puVar1 = 0x17;
        uVar2 = 0xffffffff;
      }
      else {
        iVar3 = FUN_00281f48(&local_2c,&local_30,&local_34);
        if (iVar3 == 0) {
          uVar5 = extraout_r1_02;
          uVar6 = extraout_r2_02;
          if (local_34 == 0) {
            FUN_002815bc(DAT_002a6074,extraout_r1_02,extraout_r2_02,0);
            uVar5 = extraout_r1_04;
            uVar6 = extraout_r2_04;
          }
          if (*(int *)(local_34 + 0x14) == 0) {
            FUN_002815bc(DAT_002a6078,uVar5,uVar6,0);
            uVar6 = extraout_r2_05;
          }
          FUN_0028228c(local_34,*(uint *)(*(int *)(local_34 + 0x14) + 8),uVar6,
                       *(int *)(local_34 + 0x14));
          iVar3 = (**(code **)(*(int *)(local_34 + 0x14) + 0x14))
                            (local_34,local_2c,local_30,param_2,puVar1);
          FUN_00282378(local_34,*(uint *)(*(int *)(local_34 + 0x14) + 8),extraout_r2_06,
                       *(int *)(local_34 + 0x14));
          if (iVar3 == 0) {
            puVar1[7] = local_34;
            uVar6 = *(undefined4 *)(*(int *)(local_34 + 0x14) + 8);
            puVar1[2] = uVar6;
            FUN_002a5c6c(uVar2,(uint)puVar1,extraout_r2_07,uVar6);
          }
          else {
            FUN_002a5cc8(uVar2,extraout_r1_05,extraout_r2_07,extraout_r3_01);
            uVar7 = FUN_002a5b0c(puVar1,extraout_r1_06);
            piVar4 = (int *)FUN_00299a14((int)uVar7,(uint)((ulonglong)uVar7 >> 0x20),extraout_r2_08)
            ;
            *piVar4 = iVar3;
            uVar2 = 0xffffffff;
          }
        }
        else {
          FUN_002a5cc8(uVar2,extraout_r1_02,extraout_r2_02,extraout_r3_00);
          uVar7 = FUN_002a5b0c(puVar1,extraout_r1_03);
          puVar1 = (undefined4 *)
                   FUN_00299a14((int)uVar7,(uint)((ulonglong)uVar7 >> 0x20),extraout_r2_03);
          *puVar1 = 2;
          uVar2 = 0xffffffff;
        }
      }
    }
  }
  return uVar2;
}

