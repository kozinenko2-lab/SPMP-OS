/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00288348; original_symbol=fat_sd_register */

undefined4 fat_sd_register(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  
  MMI_diag_stub_00298B64();
  if (*DAT_002883e8 != 0) {
    MMI_diag_stub_00298B64();
    return 1;
  }
  *DAT_002883e8 = 1;
  iVar1 = FUN_002831e4(0,DAT_002883f0);
  if (iVar1 != 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_002831f8(0,DAT_002883f4);
  if (iVar1 != 0) {
    return 0xffffffff;
  }
  FUN_002b84b0(0,DAT_002883f8);
  uVar3 = FUN_002b84e8(0,DAT_002883fc);
  iVar1 = FUN_002bab1c((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20));
  if (iVar1 == 0) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

