/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0042f954; original_symbol=shell_game_stack_free */

void shell_game_stack_free(void)

{
  undefined4 *puVar1;
  
  puVar1 = DAT_0042f980;
  if ((int *)*DAT_0042f980 == (int *)0x0) {
    return;
  }
  mmi_game_pool_free((int *)*DAT_0042f980);
  *puVar1 = 0;
  return;
}

