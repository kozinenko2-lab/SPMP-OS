/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046190c; original_symbol=native_pause_callback */

undefined4 native_pause_callback(void)

{
  uint extraout_r1;
  uint extraout_r1_00;
  
  MMI_diag_stub_00298B64();
  native_resource_pause((uint)*DAT_00461950);
  ecos_thread_suspend_wrapper(*DAT_00461954,extraout_r1);
  ecos_thread_suspend_wrapper(*DAT_00461958,extraout_r1_00);
  return 0;
}

