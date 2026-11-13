/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000467f4; original_symbol=rb_bootkernel */

bool rb_bootkernel(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar2;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 uVar3;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 *extraout_r3_01;
  undefined4 *puVar4;
  undefined1 auStack_38 [32];
  undefined4 local_18;
  undefined4 local_14;
  undefined1 *local_10;
  
  FUN_0004563c(DAT_00046894,param_2,param_3,param_4);
  puVar4 = DAT_00046898;
  *DAT_00046898 = 1;
  uVar1 = rb_buffered_load(DAT_0004689c,DAT_000468a0,extraout_r2,puVar4);
  if (uVar1 == 0) {
    FUN_0004563c(DAT_000468b4,extraout_r1,extraout_r2_00,extraout_r3);
    puVar4 = DAT_00046898;
    uVar3 = 0;
    *DAT_00046898 = 0;
    uVar2 = extraout_r1_01;
  }
  else {
    local_18 = DAT_000468a4;
    local_14 = DAT_000468a8;
    local_10 = auStack_38;
    FUN_00045890(auStack_38,DAT_000468ac,uVar1,DAT_000468a8);
    FUN_0004563c(DAT_000468b0,uVar1,extraout_r2_01,extraout_r3_00);
    rb_go((uint *)&DAT_00000003,(uint)&local_18);
    uVar2 = extraout_r1_00;
    uVar3 = extraout_r2_02;
    puVar4 = extraout_r3_01;
  }
  FUN_0004563c(DAT_000468b8,uVar2,uVar3,puVar4);
  return uVar1 != 0;
}

