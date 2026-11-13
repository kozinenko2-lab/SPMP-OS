/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002eb454; original_symbol=ecos_io_vector_transfer */

int ecos_io_vector_transfer(uint param_1,uint *param_2,int param_3,uint param_4)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  int *piVar6;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  uint uVar7;
  undefined4 extraout_r3;
  code *pcVar8;
  int iVar9;
  undefined8 uVar10;
  uint *local_b8;
  int local_b4;
  int local_ac;
  undefined4 local_a8;
  uint local_a4;
  uint local_a0 [32];
  
  if (param_3 < 0x11) {
    iVar2 = 0;
    iVar9 = 0;
    puVar4 = param_2;
    iVar5 = param_3;
    uVar7 = param_4;
    if (0 < param_3) {
      do {
        iVar5 = iVar9 * 8;
        uVar3 = param_2[iVar9 * 2 + 1];
        iVar2 = iVar2 + uVar3;
        puVar4 = local_a0 + iVar9 * 2 + 0x20;
        uVar7 = param_2[iVar9 * 2];
        local_a0[iVar9 * 2] = uVar7;
        local_a0[iVar9 * 2 + 1] = uVar3;
        iVar9 = iVar9 + 1;
      } while (iVar9 < param_3);
    }
    puVar4 = (uint *)FUN_002a5d20(param_1,(uint)puVar4,iVar5,uVar7);
    if (puVar4 == (uint *)0x0) {
      puVar1 = (undefined4 *)FUN_00299a14(0,extraout_r1,extraout_r2);
      *puVar1 = 9;
      iVar2 = -1;
    }
    else if ((param_4 & *puVar4) == 0) {
      uVar10 = FUN_002a5d7c(puVar4,extraout_r1);
      puVar1 = (undefined4 *)
               FUN_00299a14((int)uVar10,(uint)((ulonglong)uVar10 >> 0x20),extraout_r2_00);
      *puVar1 = 9;
      iVar2 = -1;
    }
    else {
      local_b8 = local_a0;
      local_a8 = 0;
      if (param_4 == 1) {
        puVar1 = (undefined4 *)puVar4[3];
        pcVar8 = (code *)*puVar1;
      }
      else {
        puVar1 = (undefined4 *)puVar4[3];
        pcVar8 = (code *)puVar1[1];
      }
      local_a4 = (uint)(param_4 != 1);
      local_b4 = param_3;
      local_ac = iVar2;
      FUN_002a5dac(puVar4,puVar4[2],extraout_r2,puVar1);
      iVar5 = (*pcVar8)(puVar4,&local_b8);
      FUN_002a5e08(puVar4,puVar4[2],extraout_r2_01,extraout_r3);
      iVar2 = iVar2 - local_ac;
      uVar10 = FUN_002a5d7c(puVar4,extraout_r1_00);
      if (((iVar5 != 0) && (iVar2 == 0 || iVar5 != 0xb)) && (iVar2 = 0, iVar5 != 0)) {
        iVar2 = -1;
        piVar6 = (int *)FUN_00299a14((int)uVar10,(uint)((ulonglong)uVar10 >> 0x20),extraout_r2_02);
        *piVar6 = iVar5;
      }
    }
  }
  else {
    puVar1 = (undefined4 *)FUN_00299a14(param_1,(uint)param_2,param_3);
    *puVar1 = 0x16;
    iVar2 = -1;
  }
  return iVar2;
}

