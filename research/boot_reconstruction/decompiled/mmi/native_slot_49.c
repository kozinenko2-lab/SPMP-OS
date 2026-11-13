/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dd52c; original_symbol=native_slot_49 */

undefined4 native_slot_49(ushort *param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  if ((param_1[2] < 0x400) && (param_1[3] < 0x400)) {
    if (((ushort)(*param_1 + 0x400) < 0x800) &&
       ((-0x401 < (short)param_1[1] && ((short)param_1[1] < 0x400)))) {
      uVar2 = FUN_00312f98((int)DAT_002dd628);
      FUN_00312fa0(DAT_002dd628);
      FUN_00313f40(DAT_002dd628,*param_1,param_1[1],param_1[2],param_1[3],DAT_002dd62c,0,0,0xf0);
      FUN_002e479c();
      if (uVar2 == 0) {
        FUN_00312fa0(DAT_002dd628);
      }
      else {
        FUN_00312f48(DAT_002dd628,uVar2,1);
      }
      uVar1 = 0;
    }
    else {
      uVar1 = 0x70000401;
    }
  }
  else {
    uVar1 = 0x70000401;
  }
  return uVar1;
}

