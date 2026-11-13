/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00297ce8; original_symbol=mmi_bootstrap */

void mmi_bootstrap(void)

{
  undefined4 extraout_r2;
  undefined8 uVar1;
  
  FUN_00297d04();
  FUN_00297d08();
  uVar1 = mmi_create_gui();
  ecos_scheduler_start((int)uVar1,(uint)((ulonglong)uVar1 >> 0x20),extraout_r2);
  return;
}

