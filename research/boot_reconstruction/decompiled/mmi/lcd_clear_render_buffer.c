/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a3e5c; original_symbol=lcd_clear_render_buffer */

void lcd_clear_render_buffer(void)

{
  ushort uVar1;
  ushort uVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)*DAT_002a3e94;
  uVar1 = lcd_get_width();
  uVar2 = lcd_get_height();
  lcd_render_clear_core((uint)uVar1,(uint)uVar2,puVar3);
  FUN_0029e450();
  return;
}

