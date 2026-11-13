/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de24c; original_symbol=native_slot_38 */

int native_slot_38(uint param_1)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  
  param_1 = param_1 & 0xff;
  uVar2 = FUN_002dd204();
  iVar1 = DAT_002de34c;
  if ((uVar2 & 0xffff) != 0) {
    return (int)(short)uVar2;
  }
  if (param_1 == 1) {
    uVar3 = (*(code *)*DAT_002de350)(0);
    *(undefined4 *)(iVar1 + 4) = uVar3;
    gfx_context_dimensions
              (iVar1,(uint)*(ushort *)(*DAT_002de354 + 8),*(undefined2 *)(*DAT_002de354 + 10));
  }
  else {
    if ((param_1 < 2) || (*DAT_002de358 <= param_1)) {
      return 0x70000406;
    }
    if (*(char *)(*DAT_002de354 + param_1 * 0x24 + -0x24) == '\0') {
      return 0x70000406;
    }
    if (*(int *)(*DAT_002de354 + param_1 * 0x24 + -0x20) != 0) {
      return 0x70000406;
    }
    iVar4 = *DAT_002de354 + param_1 * 0x24;
    *(undefined4 *)(DAT_002de34c + 4) = *(undefined4 *)(iVar4 + -0x18);
    gfx_context_dimensions(iVar1,(uint)*(ushort *)(iVar4 + -0x1c),*(undefined2 *)(iVar4 + -0x1a));
  }
  return 0;
}

