/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00290c5c; original_symbol=pcm_dma_isr */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 pcm_dma_isr(void)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  
  _DAT_93010004 = 1 << (*DAT_002911d8 & 0xff);
  if ((*(uint *)(*DAT_002911dc + 4) & 8) != 0) {
    if (*DAT_00291224 == 0) {
      *DAT_00291224 = 1;
    }
    else {
      *DAT_00291224 = 0;
    }
    return 2;
  }
  if (*DAT_002911e0 == 0) {
    *DAT_002911ec = *DAT_002911ec + (*(uint *)(*DAT_002911f0 + 8) >> 8);
  }
  else {
    *DAT_002911e4 = *DAT_002911e4 + 1;
    if (*DAT_002911e8 != 1) {
      *DAT_002911ec = *DAT_002911ec + (*(uint *)(*DAT_002911f0 + 8) >> 8);
    }
  }
  if ((*(uint *)*DAT_002911f4 & 1) != 0) {
    MMI_diag_stub_00298B64();
  }
  if (0 < *DAT_002911e8) {
    *DAT_002911e8 = 0;
    if ((*DAT_002911fc & 4) != 0) {
      *DAT_00291200 = 1;
      if (((*(uint *)*DAT_002911f4 & 4) != 0) &&
         (MMI_diag_stub_00298B64(), (*(uint *)*DAT_002911f4 & 4) != 0)) {
        MMI_diag_stub_00298B64();
      }
      FUN_0029467c(DAT_00291210,*(int *)(*DAT_00291208 + 8));
      if (*DAT_00291214 < 1) {
        return 2;
      }
      *DAT_00291214 = *DAT_00291214 + -1;
      return 2;
    }
    *DAT_002911ec = *DAT_002911ec + (*(uint *)(*DAT_002911f0 + 8) >> 8);
    if ((*(uint *)*DAT_002911f4 & 4) != 0) {
      MMI_diag_stub_00298B64();
    }
  }
  if (*DAT_0029121c <= *DAT_002911e4) {
    if (*DAT_002911e0 != 0) {
      if (*(uint *)(*DAT_00291208 + 8) < 0x1001) {
        if (*DAT_00291200 == 0) {
          *DAT_002911e4 = 0;
          *DAT_002911e8 = 1;
          *DAT_00291208 = *(int *)(*DAT_002911dc + 0x10);
          *DAT_00291214 = *DAT_00291214 + 1;
        }
        else {
          *DAT_00291200 = 0;
          *DAT_002911e4 = 0;
        }
      }
      else {
        if ((*(uint *)*DAT_002911f4 & 4) != 0) {
          MMI_diag_stub_00298B64();
        }
        *DAT_002911e4 = *DAT_002911e4 + -1;
      }
      goto LAB_00290f58;
    }
    if (*DAT_0029121c <= *DAT_002911e4) goto LAB_00290f58;
  }
  if ((*DAT_002911e0 != 0) && (*DAT_00291200 == 0)) {
    *DAT_002911e8 = 1;
    *DAT_00291208 = *(int *)(*DAT_002911dc + 0x10);
  }
LAB_00290f58:
  piVar4 = DAT_002911f0;
  *(uint *)(*DAT_002911f0 + 4) = *(uint *)(*DAT_002911f0 + 4) & 0xfffffffb | 1;
  *piVar4 = *(int *)(*piVar4 + 0x10);
  iVar3 = *(int *)(*DAT_002911dc + 0x10);
  *DAT_002911dc = iVar3;
  *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) & 0xfffffffd | 4;
  uVar2 = FUN_002968d0();
  if (uVar2 == 1) {
    uVar2 = (_DAT_93010000 >> 8) >> (*DAT_002911d8 & 0xff) & 1;
    if ((*(uint *)*DAT_002911f4 & 1) != 0) {
      MMI_diag_stub_00298B64();
    }
    if ((uVar2 == 0) && (*DAT_00291224 == 0)) {
      return 2;
    }
    if ((uVar2 == 1) && (*DAT_00291224 == 1)) {
      return 2;
    }
  }
  else if ((*(uint *)*DAT_002911f4 & 1) != 0) {
    MMI_diag_stub_00298B64();
  }
  iVar3 = DAT_00291234;
  piVar4 = DAT_002911dc;
  puVar1 = DAT_002911d8;
  if (*DAT_00291224 == 0) {
    if (*DAT_00291230 - 1U < 3) {
      *(int *)(*DAT_002911d8 * 4 + -0x6cfefff8) = DAT_00291234;
      *(int *)(*puVar1 * 4 + -0x6cfeffe8) = iVar3 + 0x3fc;
    }
    else if (((undefined4 *)*DAT_002911dc)[2] != 0) {
      *(undefined4 *)(*DAT_002911d8 * 4 + -0x6cfefff8) = *(undefined4 *)*DAT_002911dc;
      piVar4 = (int *)*piVar4;
      *(int *)(*puVar1 * 4 + -0x6cfeffe8) = *piVar4 + piVar4[2] + -4;
    }
    *DAT_00291224 = 1;
  }
  else {
    if (*DAT_00291230 - 1U < 3) {
      *(int *)(*DAT_002911d8 * 4 + -0x6cfeffb4) = DAT_00291234;
      *(int *)(*puVar1 * 4 + -0x6cfeffa4) = iVar3 + 0x3fc;
    }
    else if (((undefined4 *)*DAT_002911dc)[2] != 0) {
      *(undefined4 *)(*DAT_002911d8 * 4 + -0x6cfeffb4) = *(undefined4 *)*DAT_002911dc;
      piVar4 = (int *)*piVar4;
      *(int *)(*puVar1 * 4 + -0x6cfeffa4) = *piVar4 + piVar4[2] + -4;
    }
    *DAT_00291224 = 0;
  }
  return 2;
}

