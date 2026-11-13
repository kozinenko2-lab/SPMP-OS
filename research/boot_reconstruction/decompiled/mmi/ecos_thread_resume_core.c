/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033a7d0; original_symbol=ecos_thread_resume_core */

void ecos_thread_resume_core(uint param_1,uint param_2)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  int *piVar2;
  int *extraout_r2;
  int *extraout_r2_00;
  int *extraout_r2_01;
  uint uVar3;
  
  piVar2 = (int *)(*DAT_0033a88c + 1);
  *DAT_0033a88c = (int)piVar2;
  if (*(int *)(param_1 + 0x3c) == 1) {
    *(undefined4 *)(param_1 + 0x3c) = 0;
    if ((*(uint *)(param_1 + 0x38) & 4) == 0) {
      FUN_002815bc(DAT_0033a890,param_2,piVar2,*(uint *)(param_1 + 0x38));
      param_2 = extraout_r1;
      piVar2 = extraout_r2;
    }
    uVar3 = *(uint *)(param_1 + 0x38) & 0xfffffffb;
    *(uint *)(param_1 + 0x38) = uVar3;
    if (uVar3 == 0) {
      FUN_0033b950(DAT_0033a894,param_1,piVar2,0);
      param_2 = extraout_r1_00;
      piVar2 = extraout_r2_00;
    }
  }
  else if (*(int *)(param_1 + 0x3c) != 0) {
    *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + -1;
  }
  iVar1 = *DAT_0033a88c + -1;
  if (iVar1 == 0) {
    FUN_0033bf08(0,param_2,piVar2);
  }
  else {
    if (iVar1 == 0) {
      FUN_002815bc(DAT_0033a898,param_2,piVar2,*DAT_0033a88c);
      param_2 = extraout_r1_01;
      piVar2 = extraout_r2_01;
    }
    if (*DAT_0033a88c == 0) {
      FUN_002815bc(DAT_0033a89c,param_2,piVar2,0);
    }
    *DAT_0033a88c = iVar1;
  }
  return;
}

