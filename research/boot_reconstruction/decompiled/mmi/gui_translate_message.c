/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003bf4c4; original_symbol=gui_translate_message */

undefined4 gui_translate_message(int param_1)

{
  if ((param_1 != 0) && (*(int *)(param_1 + 0xc) == 0x100)) {
    FUN_003bee44(*(int *)(param_1 + 8),0x102,*(int *)(param_1 + 0x10),*(int *)(param_1 + 0x14));
  }
  return 0;
}

