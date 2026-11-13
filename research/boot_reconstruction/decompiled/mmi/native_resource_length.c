/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461268; original_symbol=native_resource_length */

int native_resource_length(undefined4 *param_1)

{
  char *pcVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  undefined1 *puVar7;
  int *piVar8;
  int *piVar9;
  undefined1 *puVar10;
  int iVar11;
  int local_24;
  
  FUN_00298c6c(&local_24,param_1,4);
  piVar8 = param_1 + 1;
  pcVar1 = FUN_00346458((char *)&local_24,DAT_004614e0);
  if (pcVar1 != (char *)0x0) {
    uVar2 = 3;
    iVar3 = 0x30000;
    do {
      piVar9 = piVar8;
      *(char *)((int)&local_24 + (iVar3 >> 0x10)) = (char)*piVar9;
      uVar4 = uVar2 - 1;
      uVar2 = uVar4 & 0xffff;
      iVar3 = uVar4 * 0x10000;
      piVar8 = (int *)((int)piVar9 + 1);
    } while (-1 < iVar3);
    iVar11 = local_24 + 8;
    uVar2 = 1;
    iVar3 = 0x10000;
    puVar7 = (undefined1 *)((int)piVar9 + 3);
    do {
      *(undefined1 *)((int)&local_24 + (iVar3 >> 0x10)) = *puVar7;
      uVar4 = uVar2 - 1;
      uVar2 = uVar4 & 0xffff;
      iVar3 = uVar4 * 0x10000;
      puVar7 = puVar7 + 1;
    } while (-1 < iVar3);
    puVar7 = (undefined1 *)((int)param_1 + iVar11);
    uVar2 = 0;
    if (0 < (short)local_24) {
      iVar3 = (int)(short)local_24;
      do {
        uVar4 = 3;
        iVar5 = 0x30000;
        puVar10 = puVar7 + 4;
        do {
          puVar7 = puVar10 + 1;
          *(undefined1 *)((int)&local_24 + (iVar5 >> 0x10)) = *puVar10;
          uVar6 = uVar4 - 1;
          uVar4 = uVar6 & 0xffff;
          iVar5 = uVar6 * 0x10000;
          puVar10 = puVar7;
        } while (-1 < iVar5);
        puVar7 = puVar7 + local_24;
        iVar11 = iVar11 + local_24 + 8;
        uVar4 = uVar2 + 1;
        uVar2 = uVar4 & 0xffff;
      } while ((int)(uVar4 * 0x10000) >> 0x10 < iVar3);
    }
    return iVar11;
  }
  pcVar1 = FUN_00346458((char *)&local_24,DAT_004614e4);
  if (pcVar1 != (char *)0x0) {
    return *piVar8 + 8;
  }
  pcVar1 = FUN_00346458((char *)&local_24,DAT_004614e8);
  if (pcVar1 == (char *)0x0) {
    return 0;
  }
  uVar2 = 3;
  iVar3 = 0x30000;
  do {
    piVar9 = (int *)((int)piVar8 + 1);
    *(char *)((int)&local_24 + (iVar3 >> 0x10)) = (char)*piVar8;
    iVar11 = local_24;
    uVar4 = uVar2 - 1;
    uVar2 = uVar4 & 0xffff;
    iVar3 = uVar4 * 0x10000;
    piVar8 = piVar9;
  } while (-1 < iVar3);
  uVar2 = 3;
  iVar3 = 0x30000;
  puVar7 = (undefined1 *)((int)piVar9 + local_24 + 4);
  do {
    puVar10 = puVar7 + 1;
    *(undefined1 *)((int)&local_24 + (iVar3 >> 0x10)) = *puVar7;
    uVar4 = uVar2 - 1;
    uVar2 = uVar4 & 0xffff;
    iVar3 = uVar4 * 0x10000;
    puVar7 = puVar10;
  } while (-1 < iVar3);
  iVar11 = iVar11 + local_24;
  uVar2 = 3;
  iVar3 = 0x30000;
  puVar7 = puVar10 + local_24 + 4;
  do {
    puVar10 = puVar7 + 1;
    *(undefined1 *)((int)&local_24 + (iVar3 >> 0x10)) = *puVar7;
    uVar4 = uVar2 - 1;
    uVar2 = uVar4 & 0xffff;
    iVar3 = uVar4 * 0x10000;
    puVar7 = puVar10;
  } while (-1 < iVar3);
  iVar11 = iVar11 + local_24;
  uVar2 = 3;
  iVar3 = 0x30000;
  puVar7 = puVar10 + local_24 + 4;
  do {
    *(undefined1 *)((int)&local_24 + (iVar3 >> 0x10)) = *puVar7;
    uVar4 = uVar2 - 1;
    uVar2 = uVar4 & 0xffff;
    iVar3 = uVar4 * 0x10000;
    puVar7 = puVar7 + 1;
  } while (-1 < iVar3);
  return iVar11 + local_24 + 0x20;
}

