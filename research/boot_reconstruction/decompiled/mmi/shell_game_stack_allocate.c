/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0042f910; original_symbol=shell_game_stack_allocate */

undefined4 shell_game_stack_allocate(void)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = DAT_0042f950;
  if (*DAT_0042f950 == 0) {
    iVar2 = mmi_game_pool_alloc(0x100010);
    *piVar1 = iVar2;
    if (iVar2 == 0) {
      return 1;
    }
  }
  return 0;
}

