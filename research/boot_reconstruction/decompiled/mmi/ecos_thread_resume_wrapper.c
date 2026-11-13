/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00339b6c; original_symbol=ecos_thread_resume_wrapper */

void ecos_thread_resume_wrapper(int *param_1,uint param_2,undefined4 param_3)

{
  uint extraout_r1;
  
  if (param_1[0xe] == 0x10) {
    FUN_0033a484(param_1,param_2,param_3,0x10);
    param_2 = extraout_r1;
  }
  ecos_thread_resume_core((uint)param_1,param_2);
  return;
}

