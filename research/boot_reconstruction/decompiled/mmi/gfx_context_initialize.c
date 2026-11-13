/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00312fb0; original_symbol=gfx_context_initialize */

void gfx_context_initialize(undefined4 *param_1)

{
  *param_1 = 0;
  *(undefined2 *)(param_1 + 2) = 0;
  *(undefined2 *)((int)param_1 + 10) = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  *(undefined2 *)(param_1 + 8) = 0;
  *(undefined2 *)((int)param_1 + 0x22) = 0;
  *(undefined2 *)(param_1 + 9) = 0;
  *(undefined2 *)((int)param_1 + 0x26) = 0;
  *(undefined1 *)(param_1 + 10) = 0;
  *(undefined1 *)((int)param_1 + 0x29) = 0;
  *(undefined2 *)((int)param_1 + 0x2a) = 0;
  param_1[1] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  return;
}

