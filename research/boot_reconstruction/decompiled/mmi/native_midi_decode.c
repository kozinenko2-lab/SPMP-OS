/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046438c; original_symbol=native_midi_decode */

ulonglong native_midi_decode(void)

{
  int iVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  ushort *puVar5;
  undefined2 *puVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  short sVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  undefined4 extraout_r1;
  ushort *puVar15;
  int iVar16;
  
  iVar7 = DAT_00464d68;
  puVar6 = DAT_00464d64;
  uVar11 = 0;
  *DAT_00464d64 = 1;
  do {
    *(undefined4 *)(iVar7 + uVar11 * 4) = 0;
    iVar1 = DAT_00464d54;
    uVar11 = uVar11 + 1 & 0xfffeffff;
  } while (uVar11 < 0x20);
  uVar11 = 0;
  do {
    iVar13 = uVar11 * 2;
    uVar11 = uVar11 + 1 & 0xfffeffff;
    *(undefined2 *)(iVar1 + iVar13) = 0;
    iVar13 = DAT_00464d58;
  } while (uVar11 < 0x20);
  uVar11 = 0;
  do {
    iVar1 = uVar11 * 2;
    uVar11 = uVar11 + 1 & 0xfffeffff;
    *(undefined2 *)(iVar13 + iVar1) = 0;
    puVar8 = DAT_00464d6c;
  } while (uVar11 < 0x20);
  uVar11 = 0;
  do {
    puVar8[uVar11] = 0;
    uVar11 = uVar11 + 1 & 0xfffeffff;
  } while (uVar11 < 0x20);
  *puVar8 = 0xe;
  FUN_004642a0(0);
  puVar5 = DAT_00464d60;
  uVar2 = *DAT_00464d60;
  FUN_004642a0(0);
  uVar3 = *puVar5;
  FUN_004642a0(0);
  uVar4 = *puVar5;
  FUN_004642a0(0);
  if ((*puVar5 & 0xff) + ((uVar4 & 0xff) + ((uVar3 & 0xff) + (uint)uVar2 * 0x100) * 0x100) * 0x100
      != DAT_00464d70) {
    return CONCAT44(DAT_00464d70,0xffffffff);
  }
  FUN_004642a0(0);
  uVar2 = *puVar5;
  FUN_004642a0(0);
  uVar3 = *puVar5;
  FUN_004642a0(0);
  uVar4 = *puVar5;
  FUN_004642a0(0);
  if ((*puVar5 & 0xff) + ((uVar4 & 0xff) + ((uVar3 & 0xff) + (uint)uVar2 * 0x100) * 0x100) * 0x100
      != 6) {
    return CONCAT44(*puVar5 & 0xff,0xfffffffe);
  }
  FUN_004642a0(0);
  uVar2 = *puVar5;
  FUN_004642a0(0);
  if ((ushort)((*puVar5 & 0xff) + uVar2 * 0x100) == 2) {
    return (ulonglong)CONCAT24(*puVar5,0xfffffffd) & 0xffffffffff;
  }
  FUN_004642a0(0);
  puVar6[1] = *puVar5 & 0xff;
  FUN_004642a0(0);
  uVar2 = *puVar5;
  iVar1 = ((uVar2 & 0xff) + (uint)(ushort)puVar6[1] * 0x100) * 0x10000;
  puVar6[1] = (short)((uint)iVar1 >> 0x10);
  if (0x20 < iVar1 >> 0x10) {
    return (ulonglong)CONCAT24(uVar2,0xfffffffc) & 0xffffffffff;
  }
  FUN_004642a0(0);
  puVar6[3] = *puVar5 & 0xff;
  FUN_004642a0(0);
  uVar9 = DAT_00464d74;
  iVar1 = ((*puVar5 & 0xff) + (uint)(ushort)puVar6[3] * 0x100) * 0x10000;
  uVar11 = iVar1 >> 0x10;
  puVar6[3] = (short)((uint)iVar1 >> 0x10);
  if ((int)uVar11 < 0) {
    if ((int)-uVar11 >> 8 == 0x18) {
      sVar10 = 0x2b;
    }
    else if ((int)-uVar11 >> 8 == 0x19) {
      sVar10 = 0x29;
    }
    else {
      sVar10 = 0x22;
    }
    uVar14 = 0;
    for (uVar12 = ((int)sVar10 * (int)(short)DAT_00464d78 * 0x40 + 0x200U >> 10) << 8;
        (uVar11 & 0xff) <= uVar12; uVar12 = uVar12 - (uVar11 & 0xff)) {
      uVar14 = uVar14 + 1;
    }
  }
  else {
    uVar14 = 0;
    for (uVar12 = DAT_00464d7c; uVar11 <= uVar12; uVar12 = uVar12 - uVar11) {
      uVar14 = uVar14 + 1;
    }
  }
  uVar11 = (uint)((ulonglong)uVar14 * (ulonglong)uVar9 >> 0x29);
  *(uint *)(puVar6 + 8) = uVar11;
  *(uint *)(puVar6 + 10) = uVar11;
  iVar1 = DAT_00464d80;
  *(undefined4 *)(puVar6 + 0xc) = 0;
  uVar11 = 0;
  puVar6[2] = 0;
  do {
    iVar13 = uVar11 * 2;
    uVar11 = uVar11 + 1 & 0xfffeffff;
    *(undefined2 *)(iVar1 + iVar13) = 0;
    iVar13 = DAT_00464d84;
  } while (uVar11 < 0x20);
  uVar11 = 0;
  do {
    *(undefined4 *)(iVar13 + uVar11 * 4) = 0;
    iVar1 = DAT_00464d88;
    uVar11 = uVar11 + 1 & 0xfffeffff;
  } while (uVar11 < 0x20);
  uVar11 = 0;
  do {
    *(undefined2 *)(iVar1 + uVar11 * 2) = 1;
    puVar8[uVar11] = 8;
    uVar11 = uVar11 + 1 & 0xfffeffff;
  } while (uVar11 < 0x20);
  *(undefined4 *)(puVar6 + 6) = 0x28;
  iVar1 = DAT_00464d8c;
  uVar11 = 0;
  if (0 < (short)puVar6[1]) {
    do {
      FUN_004642a0(uVar11);
      puVar15 = puVar5 + uVar11;
      uVar2 = *puVar15;
      FUN_004642a0(uVar11);
      uVar3 = *puVar15;
      FUN_004642a0(uVar11);
      uVar4 = *puVar15;
      FUN_004642a0(uVar11);
      if ((*puVar15 & 0xff) +
          ((uVar4 & 0xff) + ((uVar3 & 0xff) + (uint)uVar2 * 0x100) * 0x100) * 0x100 != DAT_00464d90)
      {
        return CONCAT44(DAT_00464d90,0xfffffffb);
      }
      FUN_004642a0(uVar11);
      uVar2 = *puVar15;
      FUN_004642a0(uVar11);
      uVar3 = *puVar15;
      FUN_004642a0(uVar11);
      uVar4 = *puVar15;
      FUN_004642a0(uVar11);
      iVar13 = (*puVar15 & 0xff) +
               ((uVar4 & 0xff) + ((uVar3 & 0xff) + (uint)uVar2 * 0x100) * 0x100) * 0x100;
      iVar16 = 0;
      DAT_00464d6c[uVar11] = iVar13;
      *(int *)(iVar7 + uVar11 * 4 + 4) = *(int *)(iVar7 + uVar11 * 4) + iVar13;
      do {
        FUN_004642a0(uVar11);
        iVar16 = (*puVar15 & 0x7f) + iVar16 * 0x80;
      } while ((*puVar15 & 0x80) != 0);
      *(int *)(iVar1 + uVar11 * 4) = iVar16;
      uVar11 = uVar11 + 1 & 0xfffeffff;
    } while ((int)uVar11 < (int)(short)puVar6[1]);
  }
  FUN_00465120();
  return CONCAT44(extraout_r1,1);
}

