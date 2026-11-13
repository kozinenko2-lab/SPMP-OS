/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033aaac; original_symbol=ecos_thread_kill_core */

void ecos_thread_kill_core(uint param_1,uint param_2)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar2;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  int *extraout_r2;
  int *extraout_r2_00;
  int *piVar3;
  int *extraout_r2_01;
  int *extraout_r2_02;
  
  if (*DAT_0033abe8 == param_1) {
    ecos_thread_exit_core();
    param_2 = extraout_r1;
  }
  *DAT_0033abec = *DAT_0033abec + 1;
  FUN_0033a8a0(param_1,param_2);
  FUN_00339804((uint *)(param_1 + 0x4c));
  uVar2 = extraout_r1_00;
  if (*(int *)(param_1 + 0x7c) != 6) {
    switch(*(undefined4 *)(param_1 + 0x78)) {
    case 0:
      if (*(int *)(param_1 + 0x38) == 0) {
        FUN_0033bacc(DAT_0033abf0,param_1,extraout_r2);
        uVar2 = extraout_r1_01;
      }
      *(undefined4 *)(param_1 + 0x38) = 0x10;
      break;
    case 1:
    case 2:
    case 3:
      *(undefined4 *)(param_1 + 0x78) = 0;
      *(undefined4 *)(param_1 + 0x7c) = 6;
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      iVar1 = *DAT_0033abec + -1;
      if (iVar1 == 0) {
        FUN_0033bf08(0,extraout_r1_00,extraout_r2);
      }
      else {
        piVar3 = extraout_r2;
        if (iVar1 == 0) {
          FUN_002815bc(DAT_0033abf4,extraout_r1_00,extraout_r2,*DAT_0033abec);
          uVar2 = extraout_r1_02;
          piVar3 = extraout_r2_00;
        }
        if (*DAT_0033abec == 0) {
          FUN_002815bc(DAT_0033abf8,uVar2,piVar3,0);
        }
        *DAT_0033abec = iVar1;
      }
      return;
    }
  }
  FUN_0033a664(param_1,uVar2);
  iVar1 = *DAT_0033abec + -1;
  if (iVar1 == 0) {
    FUN_0033bf08(0,extraout_r1_03,extraout_r2_01);
  }
  else {
    uVar2 = extraout_r1_03;
    piVar3 = extraout_r2_01;
    if (iVar1 == 0) {
      FUN_002815bc(DAT_0033abf4,extraout_r1_03,extraout_r2_01,*DAT_0033abec);
      uVar2 = extraout_r1_04;
      piVar3 = extraout_r2_02;
    }
    if (*DAT_0033abec == 0) {
      FUN_002815bc(DAT_0033abf8,uVar2,piVar3,0);
    }
    *DAT_0033abec = iVar1;
  }
  return;
}

