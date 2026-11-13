/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003e1c4c; original_symbol=mmi_shell_message_dispatch */

undefined4 mmi_shell_message_dispatch(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  bool bVar4;
  
  if (*(int *)(param_2 + 8) == *DAT_003e1d50) {
    uVar1 = FUN_003bead4(*(int *)(param_2 + 8),*(uint *)(param_2 + 0xc),
                         *(undefined4 *)(param_2 + 0x10),*(int *)(param_2 + 0x14));
    return uVar1;
  }
  if (*(int *)(param_2 + 8) == param_1) {
    uVar1 = FUN_003bead4(param_1,*(uint *)(param_2 + 0xc),*(undefined4 *)(param_2 + 0x10),
                         *(int *)(param_2 + 0x14));
    return uVar1;
  }
  iVar2 = *(int *)(param_2 + 0xc);
  if ((((iVar2 == 0x551 || iVar2 == 0x55a) || (iVar2 == 0x553)) || (iVar2 == 0x558)) ||
     ((iVar2 == 0x554 || (iVar2 == 0x552)))) {
    *(int *)(param_2 + 8) = param_1;
  }
  uVar3 = *(uint *)(param_2 + 0xc);
  bVar4 = 0x43f < uVar3;
  if (uVar3 != 0x440) {
    bVar4 = uVar3 != 0x100;
  }
  if (((bVar4 && (uVar3 != 0x440 && uVar3 != 0x101)) && (uVar3 != 0x10)) && (uVar3 != 0xf)) {
    uVar1 = FUN_003bead4(*(int *)(param_2 + 8),uVar3,*(undefined4 *)(param_2 + 0x10),
                         *(int *)(param_2 + 0x14));
    return uVar1;
  }
  return 0;
}

