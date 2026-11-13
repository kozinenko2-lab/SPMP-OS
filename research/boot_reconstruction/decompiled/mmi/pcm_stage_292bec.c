/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00292bec; original_symbol=pcm_stage_292bec */

undefined4 pcm_stage_292bec(void)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_1c;
  
  local_1c = 0;
  uVar3 = 0;
  if (*DAT_00292d1c < 3) {
    iVar2 = (*(code *)*DAT_00292d24)(&local_1c,*DAT_00292d20);
    if ((*(uint *)*DAT_00292d28 & 1) != 0) {
      MMI_diag_stub_00298B64();
    }
    piVar1 = DAT_00292d30;
    if (iVar2 == 0) {
      if ((*(uint *)*DAT_00292d28 & 0x10) != 0) {
        MMI_diag_stub_00298B64();
      }
      uVar3 = 0xffffffff;
    }
    else {
      *(int *)*DAT_00292d30 = iVar2;
      *(undefined4 *)(*piVar1 + 8) = local_1c;
      *(uint *)(*piVar1 + 4) = *(uint *)(*piVar1 + 4) & 0xfffffffe | 2;
      *piVar1 = *(int *)(*piVar1 + 0x10);
      *DAT_00292d1c = *DAT_00292d1c + 1;
      if ((*(uint *)*DAT_00292d28 & 0x10) != 0) {
        MMI_diag_stub_00298B64();
      }
    }
  }
  if ((*(uint *)*DAT_00292d28 & 1) != 0) {
    MMI_diag_stub_00298B64();
  }
  return uVar3;
}

