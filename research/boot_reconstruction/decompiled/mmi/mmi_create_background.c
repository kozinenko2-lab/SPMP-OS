/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00447458; original_symbol=mmi_create_background */

void mmi_create_background(undefined4 param_1,uint param_2)

{
  uint extraout_r1;
  int iVar1;
  int extraout_r2;
  
  iVar1 = *DAT_004474c0;
  if (iVar1 == 0) {
    ecos_thread_create_wrapper
              (0x1e,DAT_004474cc,0,DAT_004474d0,DAT_004474c4,0x2980,DAT_004474c0,DAT_004474c8);
    MMI_diag_stub_00298B64();
    param_2 = extraout_r1;
    iVar1 = extraout_r2;
  }
  ecos_thread_resume_wrapper((int *)*DAT_004474c0,param_2,iVar1);
  return;
}

