/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a3e98; original_symbol=lcd_wait_sync */

void lcd_wait_sync(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_0029e310();
  if (iVar1 == 0) {
    return;
  }
  uVar2 = lcd_get_flags();
  if ((uVar2 & 0x10000) == 0) {
    return;
  }
  FUN_00339f50(DAT_002a3edc,0);
  ecos_flag_wait(DAT_002a3edc,1,1);
  return;
}

