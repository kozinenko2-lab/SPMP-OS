/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002ddb80; original_symbol=native_query_image */

undefined4 native_query_image(uint param_1,undefined1 param_2,ushort *param_3)

{
  int iVar1;
  
  param_1 = param_1 & 0xff;
  if ((param_1 < 2) || (*DAT_002ddc38 <= param_1)) {
    return 0x406;
  }
  iVar1 = *DAT_002ddc3c + param_1 * 0x24;
  if (*(char *)(iVar1 + -0x24) == '\0') {
    return 0x406;
  }
  switch(param_2) {
  case 1:
    *param_3 = *(ushort *)(iVar1 + -0x1c);
    break;
  case 2:
    *param_3 = *(ushort *)(iVar1 + -0x1a);
    break;
  case 3:
    *param_3 = *(ushort *)(iVar1 + -0x20);
    break;
  case 4:
    *param_3 = (ushort)*(byte *)(iVar1 + -0x13);
    break;
  default:
    return 0x406;
  }
  return 0;
}

