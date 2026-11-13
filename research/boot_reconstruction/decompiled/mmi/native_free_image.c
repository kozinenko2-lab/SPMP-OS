/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dda44; original_symbol=native_free_image */

undefined4 native_free_image(uint param_1)

{
  uint uVar1;
  int iVar2;
  char *pcVar3;
  
  param_1 = param_1 & 0xff;
  if ((param_1 < 2) || (*DAT_002ddb70 <= param_1)) {
    return 0x406;
  }
  iVar2 = *DAT_002ddb74 + param_1 * 0x24;
  pcVar3 = (char *)(iVar2 + -0x24);
  if (*pcVar3 == '\0') {
    return 0x406;
  }
  if (*(char *)(iVar2 + -9) == '\x01') {
    if (*(uint *)(iVar2 + -0x18) != 0) {
      mmi_general_free(*(uint *)(iVar2 + -0x18));
    }
    if (*(int *)(iVar2 + -0x10) != 0) {
      uVar1 = 1;
      if (1 < *DAT_002ddb70) {
        do {
          if ((uVar1 != param_1 - 1) &&
             (*(int *)(iVar2 + -0x10) == *(int *)(*DAT_002ddb74 + uVar1 * 0x24 + 0x14))) break;
          uVar1 = uVar1 + 1 & 0xffff;
        } while (uVar1 < *DAT_002ddb70);
      }
      if ((*DAT_002ddb70 == uVar1) && (*(uint *)(iVar2 + -0x10) != 0)) {
        mmi_general_free(*(uint *)(iVar2 + -0x10));
      }
    }
  }
  pcVar3[0] = '\0';
  pcVar3[1] = '\0';
  pcVar3[2] = '\0';
  pcVar3[3] = '\0';
  *(undefined4 *)(iVar2 + -0x20) = 0;
  *(undefined4 *)(iVar2 + -0x1c) = 0;
  *(undefined4 *)(iVar2 + -0x18) = 0;
  *(undefined4 *)(iVar2 + -0x14) = 0;
  *(undefined4 *)(iVar2 + -0x10) = 0;
  *(undefined4 *)(iVar2 + -0xc) = 0;
  *(undefined4 *)(iVar2 + -8) = 0;
  *(undefined4 *)(iVar2 + -4) = 0;
  return 0;
}

