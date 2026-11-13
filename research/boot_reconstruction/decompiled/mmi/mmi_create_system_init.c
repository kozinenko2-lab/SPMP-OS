/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004475c0; original_symbol=mmi_create_system_init */

void mmi_create_system_init(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int *piVar1;
  uint extraout_r1;
  undefined4 extraout_r2;
  
  piVar1 = DAT_00447628;
  if (*DAT_00447628 == 0) {
    MMI_diag_stub_00298B64();
    ecos_thread_create_wrapper
              (0xc,DAT_00447638,0,DAT_0044763c,DAT_00447630,0x2980,piVar1,DAT_00447634);
    param_2 = extraout_r1;
    param_3 = extraout_r2;
  }
  ecos_thread_resume_wrapper((int *)*DAT_00447628,param_2,param_3);
  return;
}

