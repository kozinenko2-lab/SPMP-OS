/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004610c4; original_symbol=native_clock_restore */

void native_clock_restore(void)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  
  piVar1 = DAT_00461128;
  if (*DAT_00461128 == 0) {
    return;
  }
  iVar2 = FUN_00296af4();
  uVar3 = FUN_00296bc8(iVar2 - 1);
  FUN_00296d38(uVar3);
  MMI_diag_stub_00298B64();
  *piVar1 = 0;
  return;
}

