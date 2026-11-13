/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033aa40; original_symbol=ecos_thread_exit_core */

void ecos_thread_exit_core(void)

{
  uint *puVar1;
  undefined4 extraout_r1;
  undefined4 extraout_r1_00;
  undefined4 uVar2;
  int *extraout_r2;
  int *extraout_r2_00;
  int *piVar3;
  uint uVar4;
  
  puVar1 = DAT_0033aaa0;
  uVar4 = *DAT_0033aaa0;
  *DAT_0033aaa4 = *DAT_0033aaa4 + 1;
  FUN_00339804((uint *)(*puVar1 + 0x4c));
  uVar2 = extraout_r1;
  piVar3 = extraout_r2;
  if (*(int *)(uVar4 + 0x38) != 0x10) {
    *(undefined4 *)(uVar4 + 0x38) = 0x10;
    FUN_0033bacc(DAT_0033aaa8,uVar4,extraout_r2);
    uVar2 = extraout_r1_00;
    piVar3 = extraout_r2_00;
  }
  FUN_0033bf08(*DAT_0033aaa4,uVar2,piVar3);
  return;
}

