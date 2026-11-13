/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0029307c; original_symbol=pcm_driver_control */

undefined4 pcm_driver_control(undefined4 param_1,uint param_2)

{
  uint *puVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0033a048(DAT_0029311c,param_2);
  puVar1 = DAT_00293120;
  uVar2 = (uint)((ulonglong)uVar3 >> 0x20);
  if ((*DAT_00293120 & 0x204) == 0) {
    FUN_0033a05c(DAT_0029311c,uVar2);
    return 0x10000000;
  }
  if (*DAT_00293120 == 4) {
    pcm_engine_enable();
    FUN_002a1434();
    *puVar1 = 2;
    uVar2 = extraout_r1;
  }
  else if (*DAT_00293120 == 0x200) {
    FUN_002a13c0((int)uVar3);
    FUN_002a144c();
    *puVar1 = 0x100;
    uVar2 = extraout_r1_00;
  }
  FUN_0033a05c(DAT_0029311c,uVar2);
  if ((code *)*DAT_00293124 != (code *)0x0) {
    (*(code *)*DAT_00293124)(4);
  }
  return 1;
}

