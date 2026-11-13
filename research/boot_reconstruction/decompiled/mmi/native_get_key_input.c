/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d2f7c; original_symbol=native_get_key_input */

undefined4 native_get_key_input(int param_1)

{
  uint uVar1;
  uint unaff_r6;
  undefined1 auStack_7c [4];
  int local_78;
  
  if (*DAT_002d2ff8 == 1) {
    native_cooked_key_core((int)auStack_7c);
    if (local_78 == 0) {
      if (*DAT_002d2ff8 == 1) {
        unaff_r6 = FUN_00350630(0);
      }
      uVar1 = FUN_00350758(unaff_r6);
      *(uint *)(param_1 + 4) = uVar1;
    }
    else {
      *(int *)(param_1 + 4) = local_78;
    }
  }
  else {
    native_cooked_key_core(param_1);
  }
  return 1;
}

