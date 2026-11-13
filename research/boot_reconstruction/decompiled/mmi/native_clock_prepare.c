/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461004; original_symbol=native_clock_prepare */

void native_clock_prepare(void)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_002d8784();
  MMI_diag_stub_00298B64();
  iVar1 = FUN_00296ad0();
  if (((iVar1 == 4) && (iVar1 = FUN_00296e24(), iVar1 < 0xcdfe601)) &&
     (iVar1 = FUN_00296b08(), iVar1 == 0)) {
    iVar1 = FUN_00296af4();
    uVar2 = FUN_00296bc8(iVar1 + 1);
    FUN_00296d0c(uVar2);
    MMI_diag_stub_00298B64();
    *DAT_004610bc = 1;
    return;
  }
  MMI_diag_stub_00298B64();
  *DAT_004610bc = 0;
  return;
}

