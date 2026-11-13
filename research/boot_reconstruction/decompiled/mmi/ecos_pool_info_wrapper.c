/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0032e5e4; original_symbol=ecos_pool_info_wrapper */

void ecos_pool_info_wrapper(int param_1,uint param_2,undefined4 *param_3)

{
  *(undefined1 *)(param_3 + 7) = 0;
  ecos_var_pool_status(param_1,param_2 & 0xffff,param_3);
  return;
}

