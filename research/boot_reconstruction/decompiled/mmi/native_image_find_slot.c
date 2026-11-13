/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002ddc40; original_symbol=native_image_find_slot */

uint native_image_find_slot(void)

{
  ushort uVar1;
  ushort *puVar2;
  uint *puVar3;
  uint uVar4;
  int *piVar5;
  uint uVar6;
  
  uVar4 = 0;
  uVar6 = 1;
  if (1 < *DAT_002ddd38) {
    do {
      if (*(char *)(*DAT_002ddd3c + uVar6 * 0x24) == '\0') {
        uVar4 = uVar6 & 0xff;
        break;
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < *DAT_002ddd38);
  }
  if (uVar4 != 0) {
    return uVar4;
  }
  if (0xfe < *DAT_002ddd38 + 0x14) {
    return 0xfffffffb;
  }
  piVar5 = mmi_general_alloc((uint)*DAT_002ddd38 * 0x24 + 0x2d0);
  puVar3 = DAT_002ddd3c;
  puVar2 = DAT_002ddd38;
  if (piVar5 == (int *)0x0) {
    return 0xffffffff;
  }
  FUN_00298c6c(piVar5,(undefined4 *)*DAT_002ddd3c,(uint)*DAT_002ddd38 * 0x24);
  FUN_00298d34(piVar5 + (uint)*puVar2 * 9,0,0x2d0);
  mmi_general_free(*puVar3);
  *puVar3 = (uint)piVar5;
  uVar1 = *puVar2;
  *puVar2 = uVar1 + 0x14;
  return uVar1 & 0xff;
}

