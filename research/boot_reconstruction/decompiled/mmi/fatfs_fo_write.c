/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00283fc4; original_symbol=fatfs_fo_write */

int fatfs_fo_write(int param_1,undefined4 *param_2)

{
  int iVar1;
  uint extraout_r1;
  undefined8 uVar2;
  
  if (0x3f < *(int *)(param_1 + 0x14)) {
    MMI_diag_stub_00298B64();
    return 0x16;
  }
  iVar1 = FUN_002a9594(*(int *)(param_1 + 0x14),*(uint *)*param_2,((uint *)*param_2)[1]);
  if (iVar1 < 0) {
    FUN_002ab170(iVar1,extraout_r1);
    uVar2 = MMI_diag_stub_00298B64();
    iVar1 = FUN_002ab170((int)uVar2,(uint)((ulonglong)uVar2 >> 0x20));
  }
  else {
    param_2[3] = param_2[3] - iVar1;
    iVar1 = 0;
  }
  return iVar1;
}

