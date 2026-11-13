/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033ce74; original_symbol=ecos_flag_poll_core */

int * ecos_flag_poll_core(uint *param_1,uint param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar2;
  uint extraout_r2;
  uint uVar3;
  int *piVar4;
  int *extraout_r2_00;
  undefined4 extraout_r3;
  int *piVar5;
  
  uVar2 = param_2;
  uVar3 = param_3;
  if (param_1 == (uint *)0x0) {
    FUN_002815bc(DAT_0033cf40,param_2,param_3,param_4);
    uVar2 = extraout_r1;
    uVar3 = extraout_r2;
    param_4 = extraout_r3;
  }
  if (3 < (param_3 & 0xff)) {
    FUN_002815bc(DAT_0033cf44,uVar2,uVar3,param_4);
    uVar2 = extraout_r1_00;
  }
  piVar4 = DAT_0033cf48;
  *DAT_0033cf48 = *DAT_0033cf48 + 1;
  piVar5 = (int *)0x0;
  if ((param_3 & 2) == 0) {
    if ((param_2 != 0) && (piVar4 = (int *)*param_1, (param_2 & (uint)piVar4) == param_2)) {
      piVar5 = piVar4;
    }
  }
  else if ((param_2 & *param_1) != 0) {
    piVar5 = (int *)*param_1;
  }
  if ((piVar5 != (int *)0x0) && ((param_3 & 1) != 0)) {
    *param_1 = 0;
  }
  iVar1 = *DAT_0033cf48 + -1;
  if (iVar1 == 0) {
    FUN_0033bf08(0,uVar2,piVar4);
  }
  else {
    if (iVar1 == 0) {
      FUN_002815bc(DAT_0033cf4c,uVar2,piVar4,*DAT_0033cf48);
      uVar2 = extraout_r1_01;
      piVar4 = extraout_r2_00;
    }
    if (*DAT_0033cf48 == 0) {
      FUN_002815bc(DAT_0033cf50,uVar2,piVar4,0);
    }
    *DAT_0033cf48 = iVar1;
  }
  return piVar5;
}

