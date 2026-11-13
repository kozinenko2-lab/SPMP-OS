/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dd80c; original_symbol=native_load_image */

undefined4 native_load_image(undefined4 *param_1,char *param_2)

{
  int *piVar1;
  uint uVar2;
  undefined1 *puVar3;
  int iVar4;
  
  if (param_1[2] == 1) {
    if ((*(ushort *)(param_1 + 1) & 1) != 0) {
      return 0x406;
    }
    if (*(byte *)(param_1 + 3) != 0) {
      if (*(char *)(*DAT_002dda40 + (uint)*(byte *)(param_1 + 3) * 0x24 + -0x24) == '\0') {
        return 0x406;
      }
      *(undefined2 *)(param_1 + 5) = 0;
    }
    iVar4 = (uint)*(ushort *)((int)param_1 + 6) * (uint)*(ushort *)(param_1 + 1) +
            (uint)*(ushort *)(param_1 + 5) * 2;
  }
  else if (param_1[2] == 2) {
    if ((*(ushort *)(param_1 + 1) & 3) != 0) {
      return 0x406;
    }
    if (*(byte *)(param_1 + 3) != 0) {
      if (*(char *)(*DAT_002dda40 + (uint)*(byte *)(param_1 + 3) * 0x24 + -0x24) == '\0') {
        return 0x406;
      }
      *(undefined2 *)(param_1 + 5) = 0;
    }
    iVar4 = (uint)*(ushort *)(param_1 + 5) * 2 +
            ((int)((uint)*(ushort *)((int)param_1 + 6) * (uint)*(ushort *)(param_1 + 1)) >> 1);
  }
  else {
    iVar4 = (uint)*(ushort *)((int)param_1 + 6) * (uint)*(ushort *)(param_1 + 1) * 2;
  }
  uVar2 = native_image_find_slot();
  piVar1 = DAT_002dda40;
  if ((int)(uVar2 * 0x10000) < 0) {
    return 0x403;
  }
  puVar3 = (undefined1 *)((short)uVar2 * 0x24 + *DAT_002dda40);
  *(undefined4 *)(puVar3 + 0xc) = *param_1;
  *puVar3 = 1;
  *(undefined4 *)(puVar3 + 4) = param_1[2];
  *(undefined2 *)(puVar3 + 8) = *(undefined2 *)(param_1 + 1);
  *(undefined2 *)(puVar3 + 10) = *(undefined2 *)((int)param_1 + 6);
  puVar3[0x11] = *(undefined1 *)(param_1 + 3);
  puVar3[0x1b] = 0;
  puVar3[0x1a] = 0;
  puVar3[0x1a] = puVar3[0x1a] | *(byte *)((int)param_1 + 0x16) & 0x40;
  puVar3[0x1a] = puVar3[0x1a] | *(byte *)((int)param_1 + 0x17) & 0x80;
  if (param_1[2] - 1 < 2) {
    if (*(char *)(param_1 + 3) == '\0') {
      puVar3[0x10] = 1;
      *(undefined4 *)(puVar3 + 0x14) = param_1[4];
      *(undefined2 *)(puVar3 + 0x18) = 0;
    }
    else {
      puVar3[0x10] = 1;
      *(undefined4 *)(puVar3 + 0x14) =
           *(undefined4 *)(*piVar1 + (uint)*(byte *)(param_1 + 3) * 0x24 + -0x10);
      *(undefined2 *)(puVar3 + 0x18) = 0;
    }
  }
  else {
    puVar3[0x10] = 0;
    *(undefined4 *)(puVar3 + 0x14) = 0;
    *(undefined2 *)(puVar3 + 0x18) = 0;
    *(int *)(puVar3 + 0x1c) = *(int *)(puVar3 + 0xc);
    *(int *)(puVar3 + 0x20) = iVar4 + *(int *)(puVar3 + 0xc);
  }
  *param_2 = (char)uVar2 + '\x01';
  return 0;
}

