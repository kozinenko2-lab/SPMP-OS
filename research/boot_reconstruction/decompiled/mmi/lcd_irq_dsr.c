/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a315c; original_symbol=lcd_irq_dsr */

void lcd_irq_dsr(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  ecos_flag_setbits(DAT_002a3178,1,param_3,param_4);
  return;
}

