/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000369fc; original_symbol=rb_mount_romfs */

undefined4 rb_mount_romfs(void)

{
  int iVar1;
  undefined4 uVar2;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  code *UNRECOVERED_JUMPTABLE;
  
  iVar1 = FUN_00030704(DAT_00036a2c,DAT_00036a30,DAT_00036a34);
  if (iVar1 < 0) {
    FUN_0004563c(DAT_00036a38,extraout_r1,extraout_r2,extraout_r3);
                    /* WARNING: Could not recover jumptable at 0x00036a28. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar2 = (*UNRECOVERED_JUMPTABLE)(0);
    return uVar2;
  }
  return 1;
}

