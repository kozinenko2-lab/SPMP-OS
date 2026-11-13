/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002deab8; original_symbol=native_enable_double_buffer */

undefined4 native_enable_double_buffer(int param_1)

{
  undefined4 uVar1;
  
  if (param_1 == 0) {
    uVar1 = (*(code *)*DAT_002deb18)();
    *(undefined4 *)(DAT_002deb14 + 4) = uVar1;
    FUN_002dd204();
  }
  else if (param_1 == 1) {
    uVar1 = (*(code *)*DAT_002deb10)();
    *(undefined4 *)(DAT_002deb14 + 4) = uVar1;
    FUN_002dd204();
  }
  return 0;
}

