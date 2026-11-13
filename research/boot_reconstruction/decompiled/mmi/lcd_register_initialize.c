/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e7cf0; original_symbol=lcd_register_initialize */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 lcd_register_initialize(int param_1)

{
  undefined4 *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined8 uVar8;
  
  uVar4 = FUN_002969b0();
  puVar1 = DAT_002e7fb4;
  uVar5 = (*(code *)DAT_002e7fb4[2])();
  uVar6 = (*(code *)puVar1[3])();
  uVar8 = (*(code *)puVar1[8])();
  FUN_0029e264((int)uVar8,(uint)((ulonglong)uVar8 >> 0x20));
  _DAT_93000000 = _DAT_93000000 | 0x10000;
  (*(code *)puVar1[1])(*puVar1);
  puVar2 = DAT_002e7fb8;
  *DAT_002e7fb8 = *DAT_002e7fb8 & 0xffff0fff;
  uVar7 = (*(code *)puVar1[9])();
  *puVar2 = uVar7 | *puVar2;
  if (uVar6 < (ushort)puVar2[1]) {
    *(short *)(puVar2 + 1) = (short)uVar6;
  }
  if (uVar6 < (ushort)DAT_002e7fb8[2]) {
    *(short *)(DAT_002e7fb8 + 2) = (short)uVar6;
  }
  if (uVar5 < *(ushort *)((int)DAT_002e7fb8 + 6)) {
    *(short *)((int)DAT_002e7fb8 + 6) = (short)uVar5;
  }
  if (uVar5 < *(ushort *)((int)DAT_002e7fb8 + 10)) {
    *(short *)((int)DAT_002e7fb8 + 10) = (short)uVar5;
  }
  if (param_1 != 0) {
    lcd_enable(0);
  }
  puVar1 = DAT_002e7fb4;
  _DAT_93000004 = (*DAT_002e7fb8 & 0xf000) >> 0xc;
  if (5 < _DAT_93000004) {
    _DAT_93000004 = 0x104;
  }
  _DAT_9300000c = (*(code *)DAT_002e7fb4[4])();
  _DAT_93000010 = (*(code *)puVar1[5])();
  _DAT_93000014 = (*(code *)puVar1[10])();
  _DAT_93000040 = (*(code *)puVar1[0xc])();
  _DAT_9300001c = (*(code *)puVar1[6])();
  _DAT_93000020 = (*(code *)puVar1[7])();
  _DAT_93000024 = (*(code *)puVar1[0xb])();
  puVar2 = DAT_002e7fb8;
  _DAT_93000008 = (uint)*(ushort *)((int)DAT_002e7fb8 + 10);
  _DAT_93000018 = (uint)(ushort)DAT_002e7fb8[2];
  _DAT_93000044 = (uint)(ushort)DAT_002e7fb8[1];
  _DAT_93000034 = (uint)*(ushort *)((int)DAT_002e7fb8 + 6);
  FUN_002e75fc((byte)DAT_002e7fb8[7]);
  lcd_apply_pixel_mode();
  puVar3 = DAT_002e7fb8;
  _DAT_9300002c = 0;
  _DAT_93000030 = 0;
  _DAT_93000038 = 0;
  if ((*puVar2 & 0x10000) == 0) {
    _DAT_93000050 = 0;
  }
  else {
    _DAT_93000050 = 0x1000000;
  }
  _DAT_93000054 = DAT_002e7fb8[3];
  _DAT_93000070 = 0;
  _DAT_93000074 = 0;
  FUN_002e7bac(0);
  uVar5 = (*puVar3 & 0x300000) >> 0x14;
  uVar6 = uVar5 | 0x10000;
  if (uVar4 == 2) {
    uVar6 = uVar5 | 0x10002;
  }
  else if (uVar4 < 3) {
    if (uVar4 == 1) {
      uVar6 = uVar5 | 0x10001;
    }
  }
  else if (uVar4 == 3) {
    uVar6 = 0x10003;
  }
  if ((*DAT_002e7fb8 & 0x100) != 0) {
    uVar6 = uVar6 | 0x1000000;
  }
  if ((*DAT_002e7fb8 & 0x10000000) != 0) {
    uVar6 = uVar6 | 0x100000;
  }
  if (_DAT_93000000 != uVar6) {
    _DAT_93000000 = uVar6;
  }
  if (*DAT_002e7fbc != 0) {
    _DAT_93000078 = 1;
  }
  FUN_002e7bac(1);
  return 0;
}

