/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00038f0c; original_symbol=rb_buffered_load */

int rb_buffered_load(uint param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  undefined4 *puVar3;
  int iVar4;
  
  iVar4 = 0;
  FUN_00049e4c(DAT_00038fa0,(uint)param_2,param_3,param_4);
  iVar1 = FUN_000435b0(param_1,1);
  puVar3 = param_2;
  if (iVar1 < 0) {
    FUN_00049e4c(DAT_00038fa4,param_1,extraout_r2,extraout_r3);
    iVar1 = -1;
  }
  else {
    do {
      iVar2 = FUN_00043e10(iVar1,puVar3,0x1000);
      puVar3 = puVar3 + 0x400;
    } while (iVar2 != 0);
    FUN_00043e40(iVar1);
    iVar1 = 0;
  }
  if (-1 < iVar1) {
    iVar4 = rb_mmp_parse(param_2,1);
  }
  return iVar4;
}

