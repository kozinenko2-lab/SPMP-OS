/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00312ec8; original_symbol=gfx_context_reset */

void gfx_context_reset(uint *param_1)

{
  *param_1 = *param_1 | 0x20000000;
  return;
}

