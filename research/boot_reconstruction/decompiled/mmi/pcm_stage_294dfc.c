/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294dfc; original_symbol=pcm_stage_294dfc */

undefined4 pcm_stage_294dfc(void)

{
  longlong lVar1;
  uint uVar2;
  int iVar3;
  ushort *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar5 = 0;
  iVar7 = 0;
  iVar6 = 0;
  iVar8 = 0;
  puVar4 = (ushort *)*DAT_00294f14;
  iVar3 = *DAT_00294f18 - *DAT_00294f1c;
  if (iVar3 < 0) {
    iVar3 = (*DAT_00294f20 - *DAT_00294f1c) + *DAT_00294f18;
  }
  if (0x17fff < iVar3) {
    iVar3 = 0x18000;
  }
  lVar1 = (longlong)(int)DAT_00294f24;
  MMI_diag_stub_00298B64();
  uVar2 = DAT_00294f24;
  do {
    if ((short)*puVar4 < 0) {
      *puVar4 = ~(ushort)(((uint)((ulonglong)(iVar8 * ((uint)*puVar4 * -0x10000 >> 0x10)) *
                                  (ulonglong)uVar2 >> 0x20) & 0x1fffff) >> 5) + 1;
    }
    else {
      *puVar4 = (ushort)((ulonglong)(iVar8 * (uint)*puVar4) * (ulonglong)uVar2 >> 0x25);
    }
    iVar6 = iVar6 + 2;
    iVar7 = iVar7 + 2;
    iVar5 = iVar5 + 2;
    if (*DAT_00294f20 < iVar5) {
      iVar5 = 0;
    }
    puVar4 = (ushort *)(iVar5 + *DAT_00294f14);
    if ((int)(lVar1 * iVar3 >> 0x25) - (iVar3 >> 0x1f) < iVar6) {
      iVar8 = iVar8 + 1;
      iVar6 = 0;
    }
  } while (iVar7 < iVar3);
  MMI_diag_stub_00298B64();
  return 0;
}

