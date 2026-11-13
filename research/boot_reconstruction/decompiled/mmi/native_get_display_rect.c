/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de020; original_symbol=native_get_display_rect */

undefined4 native_get_display_rect(undefined2 *param_1)

{
  undefined2 uVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  uVar1 = (*(code *)*DAT_002de064)();
  param_1[2] = uVar1;
  uVar1 = (*(code *)*DAT_002de068)();
  param_1[3] = uVar1;
  return 0;
}

