/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046195c; original_symbol=native_resume_callback */

undefined4 native_resume_callback(void)

{
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  
  MMI_diag_stub_00298B64();
  native_resource_resume((uint)*DAT_004619a0);
  ecos_thread_resume_wrapper((int *)*DAT_004619a4,extraout_r1,extraout_r2);
  ecos_thread_resume_wrapper((int *)*DAT_004619a8,extraout_r1_00,extraout_r2_00);
  return 0;
}

