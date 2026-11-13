/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00339f64; original_symbol=ecos_flag_wait */

int * ecos_flag_wait(uint *param_1,uint param_2,uint param_3)

{
  int *piVar1;
  
  if ((param_2 == 0) || ((param_3 & 0xfc) != 0)) {
    return (int *)0x0;
  }
  piVar1 = ecos_flag_wait_core(param_1,param_2,param_3 & 0xff,param_3 & 0xff);
  return piVar1;
}

