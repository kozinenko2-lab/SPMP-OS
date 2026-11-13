/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00339b18; original_symbol=ecos_thread_delete_wrapper */

undefined4 ecos_thread_delete_wrapper(uint param_1,uint param_2)

{
  if ((*(int *)(param_1 + 0x38) == 0x10) ||
     (ecos_thread_kill_core(param_1,param_2), *(int *)(param_1 + 0x38) == 0x10)) {
    ecos_thread_destruct_core(param_1);
    return 1;
  }
  return 0;
}

