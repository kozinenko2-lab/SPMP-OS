/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0003e3f0; original_symbol=rb_mmp_copy_segments */

uint rb_mmp_copy_segments(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iStack_30;
  int local_2c;
  undefined4 *local_28;
  
  if (param_1 == 0) {
LAB_0003e47c:
    uVar3 = 0xffffffff;
  }
  else if (param_2 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = 0;
    iVar4 = 0;
    do {
      FUN_0003e3c0(&iStack_30,(undefined4 *)((uVar3 & 0xfffffffc) + param_1),0x18);
      iVar1 = FUN_0003e3e4(&iStack_30,DAT_0003e488);
      if (iVar1 != 0) goto LAB_0003e47c;
      uVar2 = local_2c - 0x18;
      FUN_0003e3c0(local_28,(undefined4 *)((uVar3 + 0x18 & 0xfffffffc) + param_1),uVar2);
      uVar3 = uVar3 + 0x18 + uVar2;
      iVar4 = iVar4 + 1;
    } while (iVar4 != param_2);
  }
  return uVar3;
}

