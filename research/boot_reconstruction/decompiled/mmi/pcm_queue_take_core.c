/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294f74; original_symbol=pcm_queue_take_core */

int pcm_queue_take_core(uint *param_1,uint param_2)

{
  int *piVar1;
  int *piVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  
  if (*DAT_002952a0 == 0) {
    if (*DAT_002952ac != 0) {
      *DAT_002952a8 = *DAT_002952b0;
    }
  }
  else {
    *DAT_002952a8 = *DAT_002952a4;
  }
  puVar3 = DAT_002952b4;
  uVar8 = *DAT_002952a8;
  uVar5 = uVar8 - *DAT_002952b4;
  if (*DAT_002952b8 == 0) {
    if ((*DAT_002952b4 != *DAT_002952bc) || (uVar8 == 0)) {
      return 0;
    }
    *DAT_002952b8 = 1;
    *puVar3 = 0;
    uVar5 = uVar8;
  }
  if ((int)uVar5 < 0) {
    uVar5 = (*DAT_002952c0 - *DAT_002952b4) + *DAT_002952a8;
  }
  if (uVar5 < 0x11) {
    *param_1 = 0;
    if (*DAT_002952a0 == 0) {
      return 0;
    }
    MMI_diag_stub_00298B64();
    if (param_2 == 0) {
      *param_1 = 0x10001000;
    }
    else {
      *param_1 = 0x10001000;
    }
    return *DAT_002952c8;
  }
  if ((int)param_2 < 1) {
    param_2 = 0x1000;
  }
  if ((int)param_2 < (int)uVar5) {
    *param_1 = param_2;
    puVar3 = DAT_002952b4;
    uVar8 = *DAT_002952b4;
    uVar7 = param_2 + uVar8;
    *DAT_002952b4 = uVar7;
    puVar4 = DAT_002952c0;
    piVar2 = DAT_002952ac;
    piVar1 = DAT_002952a0;
    if ((int)*DAT_002952c0 < (int)(param_2 + uVar7)) {
      iVar6 = uVar5 - param_2;
      if ((int)param_2 < iVar6) {
        *param_1 = (*DAT_002952c0 - uVar7) + *param_1;
        *puVar4 = *DAT_002952bc;
        *DAT_002952cc = 0;
        *puVar3 = 0;
      }
      else if (*DAT_002952a0 == 0) {
        if (*DAT_002952ac == 0) {
          *param_1 = 0;
          *DAT_002952b4 = uVar8;
          return 0;
        }
        *param_1 = iVar6 + *param_1 | 0x20000000;
        *DAT_002952c0 = *DAT_002952bc;
        *DAT_002952cc = 0;
        *DAT_002952b4 = 0;
        *piVar2 = 0;
        *DAT_002952b0 = 0x400000;
      }
      else {
        *param_1 = iVar6 + *param_1 | 0x10000000;
        *DAT_002952c0 = *DAT_002952bc;
        *DAT_002952cc = 0;
        *DAT_002952b4 = 0;
        *piVar1 = 0;
        *DAT_002952a4 = 0x400000;
      }
    }
  }
  else {
    *param_1 = uVar5 - 1 & 0xfffffff0;
    puVar3 = DAT_002952b4;
    uVar8 = *DAT_002952b4;
    uVar5 = (uVar5 + uVar8) - 1 & 0xfffffff0;
    *DAT_002952b4 = uVar5;
    piVar1 = DAT_002952a0;
    if (*DAT_002952a0 == 0) {
      if ((int)*DAT_002952c0 < (int)(param_2 + uVar5)) {
        *param_1 = 0;
        *puVar3 = uVar8;
        return 0;
      }
    }
    else if (*DAT_002952a0 == 1) {
      *param_1 = *param_1 | 0x10000000;
      *piVar1 = 0;
      *DAT_002952a4 = 0x400000;
    }
    piVar1 = DAT_002952ac;
    if (*DAT_002952ac == 1) {
      *param_1 = *param_1 | 0x20000000;
      *piVar1 = 0;
      *DAT_002952b0 = 0x400000;
    }
  }
  return uVar8 + *DAT_002952c8;
}

