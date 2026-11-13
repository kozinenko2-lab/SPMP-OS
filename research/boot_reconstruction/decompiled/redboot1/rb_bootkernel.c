/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0004a994; original_symbol=rb_bootkernel */

bool rb_bootkernel(void)

{
  uint uVar1;
  uint in_r1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar2;
  undefined4 in_r2;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 uVar3;
  undefined4 in_r3;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 *extraout_r3_01;
  undefined4 *puVar4;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined1 *puStack_40;
  undefined1 auStack_3c [32];
  
  FUN_00049e4c(DAT_0004aa3c,in_r1,in_r2,in_r3);
  puVar4 = DAT_0004aa40;
  *DAT_0004aa40 = 1;
  uVar1 = rb_buffered_load(DAT_0004aa44,DAT_0004aa48,extraout_r2,puVar4);
  if (uVar1 == 0) {
    FUN_00049e4c(DAT_0004aa5c,extraout_r1,extraout_r2_00,extraout_r3);
    puVar4 = DAT_0004aa40;
    uVar3 = 0;
    *DAT_0004aa40 = 0;
    uVar2 = extraout_r1_01;
  }
  else {
    uStack_48 = DAT_0004aa4c;
    uStack_44 = DAT_0004aa50;
    puStack_40 = auStack_3c;
    FUN_00049cd4(puStack_40,DAT_0004aa54,uVar1,DAT_0004aa50);
    FUN_00049e4c(DAT_0004aa58,uVar1,extraout_r2_01,extraout_r3_00);
    rb_go((uint *)&DAT_00000003,(uint)&uStack_48,extraout_r2_02);
    uVar2 = extraout_r1_00;
    uVar3 = extraout_r2_03;
    puVar4 = extraout_r3_01;
  }
  FUN_00049e4c(DAT_0004aa60,uVar2,uVar3,puVar4);
  return uVar1 != 0;
}

