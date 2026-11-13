/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00447540; original_symbol=mmi_system_init */

void mmi_system_init(void)

{
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 uVar1;
  undefined4 extraout_r3;
  undefined4 uVar2;
  undefined8 uVar3;
  
  ecos_flag_init(DAT_004475b0);
  ecos_flag_init(DAT_004475b4);
  mmi_subsystem_init();
  ecos_flag_wait(DAT_004475b4,1,3);
  uVar2 = 1;
  uVar1 = extraout_r2;
  if (*DAT_004475b8 != 1) {
    FUN_002d1b28();
    uVar1 = extraout_r2_00;
    uVar2 = extraout_r3;
  }
  ecos_flag_setbits(DAT_004475bc,1,uVar1,uVar2);
  mmi_system_init_retry();
  uVar3 = FUN_002d8b88(0x581,0,0);
  mmi_create_background((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20));
  ecos_thread_exit_wrapper();
  return;
}

