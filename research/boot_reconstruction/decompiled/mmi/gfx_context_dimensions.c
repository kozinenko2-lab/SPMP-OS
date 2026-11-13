/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00312ce8; original_symbol=gfx_context_dimensions */

void gfx_context_dimensions(int param_1,uint param_2,undefined2 param_3)

{
  *(short *)(param_1 + 8) = (short)param_2;
  *(undefined2 *)(param_1 + 10) = param_3;
  if (*(char *)(param_1 + 0x29) == '\n') {
    *(short *)(param_1 + 0x2a) = (short)(param_2 >> 1);
    return;
  }
  if (*(char *)(param_1 + 0x29) == '\v') {
    *(short *)(param_1 + 0x2a) = (short)param_2;
  }
  else {
    *(short *)(param_1 + 0x2a) = (short)(param_2 << 1);
  }
  return;
}

