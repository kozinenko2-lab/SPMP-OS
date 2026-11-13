/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de1b4; original_symbol=native_camera_mode */

undefined4 native_camera_mode(char param_1)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if (param_1 == ' ') {
    native_enable_double_buffer(1);
  }
  else {
    if (param_1 != '!') {
      return 0x70000406;
    }
    native_enable_double_buffer(1);
    iVar1 = (*(code *)*DAT_002de240)();
    iVar2 = (*(code *)*DAT_002de244)();
    puVar3 = (undefined4 *)(*(code *)*DAT_002de248)();
    FUN_00298d34(puVar3,0,iVar2 * iVar1 * 2);
  }
  return 0;
}

