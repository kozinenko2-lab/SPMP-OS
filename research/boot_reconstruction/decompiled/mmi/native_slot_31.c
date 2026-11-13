/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de35c; original_symbol=native_slot_31 */

undefined4 native_slot_31(int param_1)

{
  ushort *puVar1;
  undefined4 *puVar2;
  int iVar3;
  
  if (param_1 == 0 || param_1 == 0xc) {
    FUN_003135b8(DAT_002de4d4,0);
  }
  if (param_1 == 1 || param_1 == 0xc) {
    FUN_00312e08(DAT_002de4d4,1);
  }
  if (param_1 == 3) {
    FUN_00312e08(DAT_002de4d4,0);
  }
  if (param_1 == 4 || param_1 == 0xc) {
    FUN_00312e94(DAT_002de4d4,DAT_002de4d8);
  }
  if (param_1 == 5 || param_1 == 0xc) {
    FUN_00312ed8(DAT_002de4d4,(uint)*DAT_002de4dc);
  }
  if (param_1 == 6 || param_1 == 0xc) {
    FUN_00312f00(DAT_002de4d4,(uint)*DAT_002de4e0);
  }
  puVar1 = DAT_002de4e4;
  if (param_1 == 7 || param_1 == 0xc) {
    *DAT_002de4e4 = 1;
    iVar3 = FUN_00312f98((int)DAT_002de4d4);
    if (iVar3 != 0) {
      FUN_00312f48(DAT_002de4d4,DAT_002de4e8,(uint)*puVar1);
    }
  }
  if (param_1 == 8 || param_1 == 0xc) {
    FUN_00312de8(DAT_002de4d4);
    if (*DAT_002de4ec != 0) {
      FUN_00312d80(DAT_002de4d4,*DAT_002de4ec);
    }
  }
  if (param_1 == 9 || param_1 == 0xc) {
    FUN_00312f48(DAT_002de4d4,DAT_002de4e8,(uint)*DAT_002de4e4);
  }
  puVar2 = DAT_002de4f0;
  if (param_1 == 10 || param_1 == 0xc) {
    *DAT_002de4f0 = DAT_002de4f4;
    puVar2[1] = 0;
    FUN_003133ac((int)DAT_002de4d4,puVar2);
    FUN_00313414(DAT_002de4d4,1);
  }
  puVar2 = DAT_002de4f0;
  if (param_1 - 0xbU < 2) {
    *DAT_002de4f0 = 0;
    puVar2[1] = DAT_002de4f8;
    FUN_003133ac((int)DAT_002de4d4,puVar2);
    FUN_003133b4(DAT_002de4d4,1);
  }
  return 0;
}

