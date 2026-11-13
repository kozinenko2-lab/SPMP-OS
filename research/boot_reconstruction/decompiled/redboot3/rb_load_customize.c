/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00036a3c; original_symbol=rb_load_customize */

undefined4 rb_load_customize(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r3;
  
  FUN_0004563c(DAT_00036a9c,param_2,param_3,param_4);
  piVar1 = rb_module_open_candidate(DAT_00036aa0,0,extraout_r2);
  if (piVar1 == (int *)0x0) {
    FUN_0004563c(DAT_00036aa4,extraout_r1,extraout_r2_00,extraout_r3);
    return 0;
  }
  iVar2 = rb_module_find_symbol((int)piVar1,DAT_00036aa8);
  piVar1 = DAT_00036aac;
  *DAT_00036aac = iVar2;
  if (iVar2 == 0) {
    FUN_0004563c(DAT_00036ab0,extraout_r1_00,extraout_r2_01,piVar1);
    return 1;
  }
  return 1;
}

