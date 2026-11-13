/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002ddd48; original_symbol=native_slot_52 */

undefined4 native_slot_52(uint param_1,uint param_2,uint param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = param_1 & 0xff;
  if ((uVar1 < 2) || (*DAT_002dde14 <= uVar1)) {
    return 0x70000406;
  }
  iVar2 = *DAT_002dde18 + uVar1 * 0x24;
  if (*(char *)(iVar2 + -0x24) == '\0') {
    return 0x70000406;
  }
  if (1 < *(int *)(iVar2 + -0x20) - 1U) {
    return 0x70000406;
  }
  if (*(char *)(iVar2 + -0x13) != '\0') {
    return 0x70000406;
  }
  if (*(char *)(iVar2 + -0x14) == '\0') {
    return 0x70000406;
  }
  if (*(char *)(iVar2 + -0x14) == '\x01') {
    FUN_00298c6c((undefined4 *)(*(int *)(iVar2 + -0x10) + (param_2 & 0xff) * 2),param_4,
                 param_3 & 0xff);
  }
  return 0;
}

