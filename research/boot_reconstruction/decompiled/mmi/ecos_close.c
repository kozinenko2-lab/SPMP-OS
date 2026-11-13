/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002eb640; original_symbol=ecos_close */

undefined4 ecos_close(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  uint *puVar1;
  undefined4 *puVar2;
  int iVar3;
  int *piVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r3;
  undefined4 uVar5;
  
  puVar1 = (uint *)FUN_002a5d20(param_1,param_2,param_3,param_4);
  if (puVar1 == (uint *)0x0) {
    puVar2 = (undefined4 *)FUN_00299a14(0,extraout_r1,extraout_r2);
    *puVar2 = 9;
    return 0xffffffff;
  }
  FUN_002a5d7c(puVar1,extraout_r1);
  iVar3 = FUN_002a5cc8(param_1,extraout_r1_00,extraout_r2_00,extraout_r3);
  uVar5 = 0;
  if (iVar3 != 0) {
    uVar5 = 0xffffffff;
    piVar4 = (int *)FUN_00299a14(iVar3,extraout_r1_01,extraout_r2_01);
    *piVar4 = iVar3;
  }
  return uVar5;
}

