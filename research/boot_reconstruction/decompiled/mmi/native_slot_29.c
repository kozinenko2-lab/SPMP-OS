/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de618; original_symbol=native_slot_29 */

undefined4 native_slot_29(char param_1,char param_2)

{
  if (((byte)(param_1 + 0x10U) < 0x20 && -0x11 < param_2) && (param_2 < '\x10')) {
    FUN_00312d28(DAT_002de694,(int)param_2);
    FUN_00312d28(DAT_002de698,(int)param_1);
    return 0;
  }
  return 0x70000401;
}

