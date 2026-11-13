/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00447640; original_symbol=mmi_system_coordinator */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 mmi_system_coordinator(void)

{
  int iVar1;
  undefined4 uVar2;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined8 uVar3;
  
  MMI_diag_stub_00298B64();
  FUN_002da8e8(10,0);
  FUN_002d97d0();
  _DAT_90005040 = _DAT_90005040 | 7;
  FUN_004428bc();
  iVar1 = FUN_004caa44();
  if (iVar1 == 0) {
    FUN_004cb19c();
  }
  uVar3 = FUN_0033a020(DAT_004476c8);
  FUN_002d559c((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20),extraout_r2);
  FUN_002b9a20();
  uVar3 = FUN_004ae688();
  FUN_0034d6b8((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20));
  uVar3 = FUN_003389e0();
  uVar2 = FUN_002922f4((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20));
  mmi_create_system_init(uVar2,extraout_r1,extraout_r2_00);
  FUN_003e2994();
  return 0;
}

