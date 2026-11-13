/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461130; original_symbol=native_decode_prefix */

undefined4 native_decode_prefix(undefined4 *param_1,uint param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  if (0x3ff < param_2) {
    param_2 = 0x400;
  }
  piVar1 = mmi_general_alloc(param_2);
  if (piVar1 == (int *)0x0) {
    uVar2 = 0xffffffff;
  }
  else {
    native_prefix_transform(piVar1,(int)param_1,param_2,DAT_004611b8,8,1);
    FUN_00298c6c(param_1,piVar1,param_2);
    mmi_general_free((uint)piVar1);
    ecos_thread_delay_wrapper(5,0);
    uVar2 = 0;
  }
  return uVar2;
}

