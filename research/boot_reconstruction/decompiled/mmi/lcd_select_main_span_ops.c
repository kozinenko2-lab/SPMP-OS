/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4034; original_symbol=lcd_select_main_span_ops */

void lcd_select_main_span_ops(void)

{
  uint uVar1;
  
  uVar1 = lcd_get_flags();
  if ((uVar1 & 1) != 0) {
    *DAT_002a4144 = DAT_002a4140;
    *DAT_002a414c = DAT_002a4148;
    return;
  }
  if ((uVar1 & 2) != 0) {
    *DAT_002a4144 = DAT_002a4150;
    *DAT_002a414c = DAT_002a4154;
    return;
  }
  if ((uVar1 & 4) != 0) {
    *DAT_002a4144 = DAT_002a4158;
    *DAT_002a414c = DAT_002a415c;
    return;
  }
  if ((uVar1 & 0x10) != 0) {
    *DAT_002a4144 = DAT_002a4160;
    *DAT_002a414c = DAT_002a4164;
    return;
  }
  if ((uVar1 & 0x20) != 0) {
    *DAT_002a4144 = DAT_002a4168;
    *DAT_002a414c = DAT_002a416c;
    return;
  }
  if ((uVar1 & 0x40) != 0) {
    *DAT_002a4144 = DAT_002a4170;
    *DAT_002a414c = DAT_002a4174;
    return;
  }
  if ((uVar1 & 0x80) == 0) {
    return;
  }
  *DAT_002a4144 = DAT_002a4178;
  *DAT_002a414c = DAT_002a417c;
  return;
}

