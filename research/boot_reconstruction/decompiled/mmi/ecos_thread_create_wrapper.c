/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00339aac; original_symbol=ecos_thread_create_wrapper */

void ecos_thread_create_wrapper
               (uint param_1,int param_2,int param_3,int param_4,int param_5,uint param_6,
               undefined4 *param_7,int *param_8)

{
  ecos_thread_create_core(param_8,param_1,param_2,param_3,param_4,param_5,param_6);
  *param_7 = param_8;
  return;
}

