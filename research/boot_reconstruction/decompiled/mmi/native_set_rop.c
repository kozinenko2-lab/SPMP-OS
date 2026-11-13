/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dde1c; original_symbol=native_set_rop */

undefined4 native_set_rop(ushort param_1)

{
  *DAT_002dde30 = param_1 & 0xff;
  return 0;
}

