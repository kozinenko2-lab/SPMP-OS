/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e8310; original_symbol=lcd_state_initialize */

void lcd_state_initialize
               (ushort param_1,ushort param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5)

{
  undefined4 *puVar1;
  
  puVar1 = DAT_002e837c;
  *DAT_002e837c = param_5;
  *(ushort *)(puVar1 + 1) = param_2;
  *(ushort *)((int)puVar1 + 6) = param_1;
  *(ushort *)(puVar1 + 2) = param_2;
  *(ushort *)((int)puVar1 + 10) = param_1;
  puVar1[3] = param_3;
  *(undefined1 *)(puVar1 + 7) = 0;
  *(ushort *)(puVar1 + 4) = param_1 & 0xfff0;
  *(ushort *)(puVar1 + 5) = param_1 & 0xfff0;
  *(ushort *)((int)puVar1 + 0x12) = param_2 & 0xfff0;
  *(ushort *)((int)puVar1 + 0x16) = param_2 & 0xfff0;
  *(undefined1 *)((int)puVar1 + 0x1d) = 0x40;
  puVar1[6] = param_4;
  lcd_register_initialize(0);
  lcd_enable(1);
  return;
}

