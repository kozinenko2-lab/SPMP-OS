/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462a74; original_symbol=native_fs_open */

undefined4 native_fs_open(uint *param_1,uint param_2,uint *param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r3;
  undefined1 auStack_244 [32];
  undefined4 auStack_224 [128];
  
  FUN_00298d34(auStack_224,0,0x200);
  puVar1 = DAT_00462b3c;
  uVar2 = FUN_00346104((uint *)*DAT_00462b3c);
  FUN_00298c6c(auStack_224,(undefined4 *)*puVar1,uVar2);
  iVar3 = FUN_00346104((uint *)*puVar1);
  uVar2 = FUN_00346104(param_1);
  FUN_00298c6c((undefined4 *)((int)auStack_224 + iVar3),param_1,uVar2);
  MMI_diag_stub_00298B64();
  uVar2 = ecos_open((int)auStack_224,param_2,extraout_r2);
  if (((int)uVar2 < 0) ||
     (iVar3 = FUN_002eb7b0(uVar2,(uint)auStack_244,extraout_r2_00,extraout_r3), iVar3 < 0)) {
    MMI_diag_stub_00298B64();
    uVar4 = 2;
  }
  else {
    *param_3 = uVar2;
    uVar4 = 0;
  }
  return uVar4;
}

