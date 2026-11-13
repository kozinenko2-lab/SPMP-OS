/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de8f0; original_symbol=native_slot_33 */

undefined4 native_slot_33(byte param_1,char param_2)

{
  undefined4 uVar1;
  
  if (param_2 == '\0') {
    FUN_003133d0(DAT_002de938,0);
  }
  else {
    FUN_003133d0(DAT_002de938,1);
  }
  uVar1 = 0x70000401;
  if (param_1 < 0x40) {
    uVar1 = 0;
  }
  return uVar1;
}

