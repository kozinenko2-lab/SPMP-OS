/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00036bf4; original_symbol=rb_buffered_load */

int rb_buffered_load(uint param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  undefined4 *puVar3;
  
  FUN_0004563c(DAT_00036c68,(uint)param_2,param_3,param_4);
  iVar1 = FUN_0003f860(param_1,1);
  if (iVar1 < 0) {
    FUN_0004563c(DAT_00036c6c,param_1,extraout_r2,extraout_r3);
    return 0;
  }
  puVar3 = param_2;
  do {
    iVar2 = FUN_0003fbc0(iVar1,puVar3,0x1000);
    puVar3 = puVar3 + 0x400;
  } while (iVar2 != 0);
  FUN_0003fbe4(iVar1);
  iVar1 = rb_mmp_parse(param_2,1);
  return iVar1;
}

