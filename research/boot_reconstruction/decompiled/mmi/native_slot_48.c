/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dd630; original_symbol=native_slot_48 */

undefined4 native_slot_48(ushort *param_1,undefined2 *param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  undefined2 uStack_30;
  undefined2 uStack_2e;
  undefined2 uStack_2c;
  undefined2 uStack_2a;
  undefined2 uStack_28;
  undefined2 uStack_26;
  
  uVar6 = 1;
  uVar5 = 2;
  uVar4 = 4;
  uVar3 = 8;
  if (param_2 == (undefined2 *)0x0 || param_1 == (ushort *)0x0) {
    uVar1 = 0x70000401;
  }
  else if ((param_1[2] < 0x400) && (param_1[3] < 0x400)) {
    if ((((ushort)(*param_1 + 0x400) < 0x800) && (-0x401 < (short)param_1[1])) &&
       ((short)param_1[1] < 0x400)) {
      uStack_30 = *param_2;
      uStack_2e = param_2[1];
      uStack_2c = param_2[2];
      uStack_2a = param_2[3];
      uStack_28 = param_2[4];
      uStack_26 = param_2[5];
      uVar2 = param_3[1];
      if (uVar2 == 2) {
        uVar4 = 8;
        uVar3 = 4;
      }
      else if (uVar2 < 3) {
        if (uVar2 == 1) {
          uVar6 = 2;
          uVar5 = 1;
        }
      }
      else if (uVar2 == 3) {
        uVar6 = 2;
        uVar5 = 1;
        uVar4 = 8;
        uVar3 = 4;
      }
      switch(*param_3) {
      case 0:
        FUN_00313534(DAT_002dd800,0,0,0);
        break;
      case 1:
        FUN_00313534(DAT_002dd800,uVar6,uVar5,(uint)&uStack_30);
        break;
      case 2:
        FUN_00313534(DAT_002dd800,uVar4,uVar4,(uint)&uStack_30);
        break;
      case 3:
        FUN_00313534(DAT_002dd800,uVar6 | uVar4,uVar5 | uVar3,(uint)&uStack_30);
      }
      FUN_0031429c(DAT_002dd800,*param_1,param_1[1],param_1[2],param_1[3],(ushort)*DAT_002dd804,
                   *DAT_002dd808);
      native_graphics_finish();
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

