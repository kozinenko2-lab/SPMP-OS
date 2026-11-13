/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002eb69c; original_symbol=ecos_seek32 */

uint ecos_seek32(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  uint *puVar1;
  undefined4 *puVar2;
  int iVar3;
  int *piVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined8 uVar5;
  uint local_1c;
  
  local_1c = param_2;
  puVar1 = (uint *)FUN_002a5d20(param_1,param_2,param_3,param_4);
  if (puVar1 == (uint *)0x0) {
    puVar2 = (undefined4 *)FUN_00299a14(0,extraout_r1,extraout_r2);
    *puVar2 = 9;
    return 0xffffffff;
  }
  FUN_002a5dac(puVar1,puVar1[2],extraout_r2,extraout_r3);
  iVar3 = (**(code **)(puVar1[3] + 8))(puVar1,&local_1c,param_3);
  FUN_002a5e08(puVar1,puVar1[2],extraout_r2_00,extraout_r3_00);
  uVar5 = FUN_002a5d7c(puVar1,extraout_r1_00);
  if (iVar3 == 0) {
    return local_1c;
  }
  piVar4 = (int *)FUN_00299a14((int)uVar5,(uint)((ulonglong)uVar5 >> 0x20),extraout_r2_01);
  *piVar4 = iVar3;
  return 0xffffffff;
}

