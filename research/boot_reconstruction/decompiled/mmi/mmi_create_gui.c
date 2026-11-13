/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002807c8; original_symbol=mmi_create_gui */

void mmi_create_gui(void)

{
  undefined4 *puVar1;
  uint extraout_r1;
  undefined4 extraout_r2;
  
  FUN_003be5a0(DAT_00280824);
  puVar1 = DAT_0028082c;
  ecos_thread_create_wrapper
            (0x19,DAT_00280834,0,DAT_00280838,DAT_00280828,0x4000,DAT_0028082c,DAT_00280830);
  ecos_thread_resume_wrapper((int *)*puVar1,extraout_r1,extraout_r2);
  return;
}

