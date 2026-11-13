/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002ddea0; original_symbol=native_get_fg_color */

undefined4 native_get_fg_color(byte *param_1)

{
  ushort uVar1;
  
  uVar1 = FUN_00312d20(DAT_002ddee8);
  *param_1 = (byte)(uVar1 >> 8) & 0xf8;
  param_1[1] = (byte)((int)(uVar1 & 0x7e0) >> 3);
  param_1[2] = (char)uVar1 << 3;
  return 0;
}

