/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de4fc; original_symbol=native_slot_32 */

undefined4 native_slot_32(int param_1)

{
  ushort *puVar1;
  int iVar2;
  
  if (param_1 == 0 || param_1 == 0xc) {
    FUN_003135b8(DAT_002de60c,1);
  }
  if (param_1 == 1 || param_1 == 0xc) {
    FUN_00312e08(DAT_002de60c,0);
  }
  if (param_1 == 3) {
    FUN_00312e08(DAT_002de60c,1);
  }
  if (param_1 == 4 || param_1 == 0xc) {
    FUN_00312eb8(DAT_002de60c);
  }
  if (param_1 == 5 || param_1 == 0xc) {
    FUN_00312f38(DAT_002de60c);
  }
  if (param_1 == 6 || param_1 == 0xc) {
    FUN_00312f28(DAT_002de60c);
  }
  puVar1 = DAT_002de610;
  if (param_1 == 7 || param_1 == 0xc) {
    *DAT_002de610 = 0;
    iVar2 = FUN_00312f98((int)DAT_002de60c);
    if (iVar2 != 0) {
      FUN_00312f48(DAT_002de60c,DAT_002de614,(uint)*puVar1);
    }
  }
  if (param_1 == 8 || param_1 == 0xc) {
    FUN_00312df8(DAT_002de60c);
    FUN_00312d98(DAT_002de60c);
  }
  if (param_1 == 9 || param_1 == 0xc) {
    FUN_00312f48(DAT_002de60c,0,0);
  }
  if (param_1 == 10 || param_1 == 0xc) {
    FUN_00313414(DAT_002de60c,0);
  }
  if (param_1 - 0xbU < 2) {
    FUN_003133b4(DAT_002de60c,0);
  }
  return 0;
}

