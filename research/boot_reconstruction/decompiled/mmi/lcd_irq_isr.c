/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a3020; original_symbol=lcd_irq_isr */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 lcd_irq_isr(void)

{
  uint *puVar1;
  int *piVar2;
  uint uVar3;
  uint uVar4;
  bool bVar5;
  bool bVar6;
  
  uVar3 = lcd_get_flags();
  _DAT_93000050 = 0x1010000;
  bVar5 = (uVar3 & 0x10000000) != 0;
  if (bVar5) {
    uVar4 = _DAT_93000000 & 0xfffffffc | (uVar3 & 0x300000) >> 0x14 | 0x110000;
  }
  else {
    uVar4 = _DAT_93000000 & 0xffefffff;
  }
  bVar6 = (uVar3 & 0x100) != 0;
  if (bVar6) {
    uVar4 = uVar4 & 0xfffffffc | (uVar3 & 0x300000) >> 0x14 | 0x1010000;
  }
  else {
    uVar4 = uVar4 & 0xfeffffff;
  }
  if (_DAT_93000000 != uVar4) {
    _DAT_93000000 = uVar4;
    MMI_diag_stub_00298B64();
  }
  puVar1 = DAT_002a314c;
  if (((bVar6 || bVar5) && (*DAT_002a3148 == 1)) && ((uVar3 & 0x20000) == 0)) {
    *DAT_002a314c = *DAT_002a314c + 1;
    uVar3 = FUN_0034c098();
    if (uVar3 < *puVar1) {
      FUN_002e8494();
      *puVar1 = 0;
    }
  }
  piVar2 = DAT_002a3150;
  if (*DAT_002a3150 == 2) {
    FUN_002e7fc0((int)*DAT_002a3154,(int)DAT_002a3154[1],(uint)(ushort)DAT_002a3154[2],
                 (uint)(ushort)DAT_002a3154[3]);
    *piVar2 = 0;
  }
  *DAT_002a3158 = *DAT_002a3158 + 1;
  return 2;
}

