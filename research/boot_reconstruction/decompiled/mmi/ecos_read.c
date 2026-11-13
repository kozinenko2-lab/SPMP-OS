/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002eb5e0; original_symbol=ecos_read */

void ecos_read(uint param_1,uint param_2,undefined4 param_3)

{
  uint local_18;
  undefined4 local_14;
  
  local_18 = param_2;
  local_14 = param_3;
  ecos_io_vector_transfer(param_1,&local_18,1,1);
  return;
}

