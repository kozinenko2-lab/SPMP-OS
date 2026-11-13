/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de124; original_symbol=native_get_framebuffer_dimensions */

undefined4 native_get_framebuffer_dimensions(undefined2 *param_1,undefined2 *param_2)

{
  undefined2 uVar1;
  
  uVar1 = (*(code *)*DAT_002de160)();
  *param_1 = uVar1;
  uVar1 = (*(code *)*DAT_002de164)();
  *param_2 = uVar1;
  return 0;
}

