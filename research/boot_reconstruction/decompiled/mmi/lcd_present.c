/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a3fd4; original_symbol=lcd_present */

undefined4 lcd_present(void)

{
  int *piVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  
  iVar2 = lcd_get_scanout();
  if (iVar2 == *DAT_002a402c) {
    return 1;
  }
  FUN_0033a048(DAT_002a4030,extraout_r1);
  iVar2 = lcd_get_scanout();
  piVar1 = DAT_002a402c;
  lcd_set_scanout(*DAT_002a402c);
  *piVar1 = iVar2;
  FUN_0033a05c(DAT_002a4030,extraout_r1_00);
  lcd_wait_sync();
  return 1;
}

