/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002949f0; original_symbol=pcm_queue_reserve_try */

int pcm_queue_reserve_try(int param_1)

{
  int iVar1;
  
  iVar1 = *DAT_00294a94 - *DAT_00294a98;
  if (iVar1 < 0) {
    iVar1 = (*DAT_00294a9c - *DAT_00294a98) + *DAT_00294a94;
  }
  if (iVar1 < *DAT_00294aa0) {
    return 0;
  }
  if (param_1 == 0) {
    if (iVar1 < 0x2001) {
      iVar1 = 0;
    }
    else {
      iVar1 = *DAT_00294a98 + DAT_00294aa4;
    }
    return iVar1;
  }
  if (param_1 < 1) {
    MMI_diag_stub_00298B64();
    return -1;
  }
  if (param_1 < iVar1) {
    iVar1 = *DAT_00294aa8 + *DAT_00294a98;
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}

