/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de6d8; original_symbol=native_slot_23 */

undefined4 native_slot_23(ushort *param_1)

{
  int iVar1;
  int iVar2;
  ushort *puVar3;
  
  puVar3 = DAT_002de7c8;
  if (((((*param_1 + 0x400 & 0xffff) < 0x800) && (-0x401 < (short)param_1[1])) &&
      (iVar1 = (int)(((uint)*param_1 + (uint)param_1[2]) * 0x10000) >> 0x10,
      (short)param_1[1] < 0x400 && -0x401 < iVar1)) &&
     ((iVar2 = (int)(((uint)param_1[1] + (uint)param_1[3]) * 0x10000) >> 0x10,
      iVar1 < 0x400 && -0x401 < iVar2 && (iVar2 < 0x400)))) {
    *DAT_002de7c8 = *param_1;
    puVar3[1] = param_1[1];
    puVar3[2] = param_1[2];
    puVar3[3] = param_1[3];
    FUN_00312e94(DAT_002de7cc,(uint)puVar3);
    return 0;
  }
  return 0x70000401;
}

