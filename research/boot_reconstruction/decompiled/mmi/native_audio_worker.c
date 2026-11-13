/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004625b8; original_symbol=native_audio_worker */

void native_audio_worker(void)

{
  bool bVar1;
  char *pcVar2;
  undefined4 *puVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  uint *puVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  byte *pbVar13;
  short *psVar14;
  int iVar15;
  
  piVar9 = mmi_general_alloc(0x400);
  *DAT_00462874 = (int)piVar9;
  if (piVar9 == (int *)0x0) {
    MMI_diag_stub_00298B64();
    return;
  }
  *DAT_0046287c = '\x03';
  *DAT_00462880 = '\r';
  FUN_00298d34((undefined4 *)*DAT_00462874,0,0x400);
  puVar3 = DAT_00462884;
  *DAT_00462884 = 0;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = 0;
  puVar3[4] = 0;
  puVar3[5] = 0;
  puVar3[6] = 0;
  puVar3[7] = 0;
  puVar3[8] = 0;
  puVar3[9] = 0;
  puVar3[10] = 0;
  puVar3[0xb] = 0;
  puVar3[0xc] = 0;
  puVar3 = DAT_00462888;
  *DAT_00462888 = 0;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = 0;
  puVar3 = DAT_0046288c;
  *DAT_0046288c = 0;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = 0;
  puVar3 = DAT_00462894;
  piVar4 = DAT_00462890;
  piVar9 = DAT_00462874;
LAB_0046269c:
  piVar6 = DAT_0046289c;
  piVar5 = DAT_00462898;
  pcVar2 = DAT_00462880;
  bVar1 = true;
  iVar10 = 0;
  iVar11 = *piVar4;
  if (((iVar11 != 0) && (*DAT_00462880 == '\v')) && (iVar15 = *DAT_00462898, iVar15 != 0)) {
    pbVar13 = (byte *)*DAT_0046289c + 1;
    iVar10 = (int)((*(byte *)*DAT_0046289c + 0x80) * 0x1000000) >> 0x10;
    *DAT_0046289c = (int)pbVar13;
    piVar7 = DAT_004628a0;
    bVar1 = false;
    if ((byte *)(iVar11 + *DAT_004628a0) <= pbVar13) {
      iVar15 = iVar15 + -1;
      *piVar5 = iVar15;
      if (iVar15 == 0) {
        *piVar4 = 0;
        *piVar6 = 0;
        *piVar7 = 0;
        *pcVar2 = '\r';
      }
      else {
        *DAT_0046289c = *piVar4;
      }
    }
  }
  piVar5 = DAT_004628a8;
  pcVar2 = DAT_0046287c;
  if (((1 < *DAT_004628a4) && (*DAT_0046287c == '\x01')) && (iVar11 = *DAT_004628a8, 0 < iVar11)) {
    psVar14 = (short *)*puVar3;
    bVar1 = false;
    iVar10 = iVar10 + *psVar14;
    uVar12 = *DAT_004628a4 - 2;
    *DAT_004628a4 = uVar12;
    *puVar3 = psVar14 + 1;
    if (uVar12 == 0) {
      iVar11 = iVar11 + -1;
      *piVar5 = iVar11;
      if (iVar11 == 0) {
        *pcVar2 = '\x03';
      }
      else {
        *DAT_004628a4 = *DAT_004628ac;
        *puVar3 = *DAT_004628b0;
      }
    }
  }
  puVar8 = DAT_004628b4;
  if (!bVar1) {
    if (0x7ffe < iVar10) {
      iVar10 = 0x7fff;
    }
    if (iVar10 < -0x8000) {
      iVar10 = -0x8000;
    }
    *(short *)(*piVar9 + *DAT_004628b4) = (short)iVar10;
    *puVar8 = *puVar8 + 2;
  }
  puVar8 = DAT_004628b4;
  if (*DAT_004628b4 < 0x400) goto LAB_00462820;
  mmi_pcm_submit((undefined4 *)*piVar9,*DAT_004628b4);
  *puVar8 = 0;
  goto LAB_0046285c;
LAB_00462820:
  if (bVar1) {
    if (*DAT_004628b4 != 0) {
      FUN_00298d34((undefined4 *)*piVar9,0,0x400 - *DAT_004628b4);
      mmi_pcm_submit((undefined4 *)*piVar9,0x400);
      *puVar8 = 0;
    }
LAB_0046285c:
    if (bVar1) {
      ecos_thread_delay_wrapper(1,0);
    }
  }
  goto LAB_0046269c;
}

