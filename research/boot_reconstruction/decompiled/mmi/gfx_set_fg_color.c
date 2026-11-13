/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00312d18; original_symbol=gfx_set_fg_color */

void gfx_set_fg_color(int param_1,undefined2 param_2)

{
  *(undefined2 *)(param_1 + 0x24) = param_2;
  return;
}

