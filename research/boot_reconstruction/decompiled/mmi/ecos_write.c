/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002eb610; original_symbol=ecos_write */

void ecos_write(uint param_1,uint param_2,undefined4 param_3)

{
  uint local_18;
  undefined4 local_14;
  
  local_18 = param_2;
  local_14 = param_3;
  ecos_io_vector_transfer(param_1,&local_18,1,2);
  return;
}

