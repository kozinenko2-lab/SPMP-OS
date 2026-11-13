/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de168; original_symbol=native_slot_39 */

undefined4 native_slot_39(char param_1)

{
  if (param_1 == '\0') {
    FUN_00312dcc(DAT_002de1a8,0);
  }
  else {
    if (param_1 != '\x01') {
      return 0x70000406;
    }
    FUN_00312dcc(DAT_002de1a8,1);
  }
  return 0;
}

