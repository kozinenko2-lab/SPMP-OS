/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dde4c; original_symbol=native_set_fg_color */

undefined4 native_set_fg_color(byte *param_1)

{
  int iVar1;
  
  iVar1 = (param_1[1] & 0xfc) * 8 + (*param_1 & 0xf8) * 0x100 + (uint)(param_1[2] >> 3);
  *DAT_002dde98 = iVar1;
  FUN_00312d18(DAT_002dde9c,(short)iVar1);
  return 0;
}

