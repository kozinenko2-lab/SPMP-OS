/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a6504; original_symbol=ecos_stat_path */

undefined4 ecos_stat_path(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int *piVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar5;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined8 uVar6;
  int local_24;
  int local_20;
  undefined4 local_1c;

  local_24 = *DAT_002a65e8;
  local_1c = *DAT_002a65ec;
  local_20 = param_1;
  iVar1 = FUN_00281f48(&local_1c,&local_20,&local_24);
  if (iVar1 == 0) {
    uVar5 = extraout_r1;
    uVar3 = extraout_r2;
    if (local_24 == 0) {
      FUN_002815bc(DAT_002a65f0,extraout_r1,extraout_r2,0);
      uVar5 = extraout_r1_00;
      uVar3 = extraout_r2_00;
    }
    if (*(int *)(local_24 + 0x14) == 0) {
      FUN_002815bc(DAT_002a65f4,uVar5,uVar3,0);
      uVar3 = extraout_r2_01;
    }
    FUN_0028228c(local_24,*(uint *)(*(int *)(local_24 + 0x14) + 8),uVar3,*(int *)(local_24 + 0x14));
    iVar1 = (**(code **)(*(int *)(local_24 + 0x14) + 0x34))(local_24,local_1c,local_20,param_2);
    uVar6 = FUN_00282378(local_24,*(uint *)(*(int *)(local_24 + 0x14) + 8),extraout_r2_02,
                         *(int *)(local_24 + 0x14));
    uVar3 = 0;
    if (iVar1 != 0) {
      uVar3 = 0xffffffff;
      piVar4 = (int *)ecos_errno_location((int)uVar6,(uint)((ulonglong)uVar6 >> 0x20),extraout_r2_03
                                         );
      *piVar4 = iVar1;
    }
  }
  else {
    puVar2 = (undefined4 *)ecos_errno_location(iVar1,extraout_r1,extraout_r2);
    *puVar2 = 2;
    uVar3 = 0xffffffff;
  }
  return uVar3;
}
