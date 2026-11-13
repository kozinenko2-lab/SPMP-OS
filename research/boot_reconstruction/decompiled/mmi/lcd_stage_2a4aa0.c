/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4aa0; original_symbol=lcd_stage_2a4aa0 */

void lcd_stage_2a4aa0(void)

{
  int *piVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar2;
  undefined4 extraout_r2;
  undefined4 uVar3;
  uint *puVar4;
  int iVar5;
  undefined8 uVar6;
  
  uVar6 = FUN_0029e310();
  piVar1 = DAT_002a4b54;
  uVar2 = (uint)((ulonglong)uVar6 >> 0x20);
  puVar4 = (uint *)*DAT_002a4b50;
  if (puVar4 == (uint *)0x0) {
    return;
  }
  uVar3 = extraout_r2;
  if (((int)uVar6 == 0) && (puVar4 = (uint *)0x0, *DAT_002a4b54 != 0)) {
    lcd_register_initialize(1);
    uVar6 = lcd_enable(1);
    iVar5 = *piVar1;
    while (uVar2 = (uint)((ulonglong)uVar6 >> 0x20), iVar5 = iVar5 + -1, iVar5 != 0) {
      uVar6 = FUN_0029e244((int)uVar6,uVar2);
    }
    *DAT_002a4b54 = 0;
    if ((*DAT_002a4b58 & 1) != 0) {
      FUN_002e788c(1);
      uVar2 = extraout_r1;
    }
    if ((*DAT_002a4b58 & 2) != 0) {
      FUN_002e7860(1);
      uVar2 = extraout_r1_00;
    }
    puVar4 = DAT_002a4b58;
    uVar3 = 0;
    *DAT_002a4b58 = 0;
  }
  FUN_00339cd8(0x17,uVar2,uVar3,puVar4);
  return;
}

