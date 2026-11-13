/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dce18; original_symbol=native_slot_45 */

undefined4 native_slot_45(uint param_1,ushort *param_2,ushort *param_3)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  
  param_1 = param_1 & 0xff;
  uVar4 = 0;
  if (param_1 < *DAT_002dd0f0) {
    iVar3 = *DAT_002dd0f4 + param_1 * 0x24;
    if (param_1 == 1) {
      uVar2 = (*(code *)*DAT_002dd0f8)();
      *(undefined4 *)(iVar3 + -0x18) = uVar2;
    }
    else if (*(char *)(iVar3 + -0x24) == '\0') {
      return 0x70000406;
    }
    if ((param_2[2] < 0x400) && (param_2[3] < 0x400)) {
      if (((short)*param_2 < 0) || ((short)param_2[1] < 0)) {
        uVar2 = 0x70000401;
      }
      else if ((int)(uint)*(ushort *)(iVar3 + -0x1c) <
               (int)((int)(short)*param_2 + (uint)param_2[2])) {
        uVar2 = 0x70000401;
      }
      else if ((int)(uint)*(ushort *)(iVar3 + -0x1a) <
               (int)((int)(short)param_2[1] + (uint)param_2[3])) {
        uVar2 = 0x70000401;
      }
      else if ((((ushort)(*param_3 + 0x400) < 0x800) && (-0x401 < (short)param_3[1])) &&
              ((short)param_3[1] < 0x400)) {
        FUN_00312ce8((int)DAT_002dd0fc,(uint)*(ushort *)(iVar3 + -0x1c),
                     *(undefined2 *)(iVar3 + -0x1a));
        uVar2 = native_image_pixel_format(*(undefined4 *)(iVar3 + -0x20));
        FUN_00312e38((int)DAT_002dd0fc,(char)uVar2);
        switch(*(undefined4 *)(iVar3 + -0x20)) {
        case 1:
        case 2:
          FUN_00312e70(DAT_002dd0fc,*(uint *)(iVar3 + -0x10));
          break;
        case 3:
          bVar1 = FUN_00312e24(DAT_002dd100);
          uVar4 = (uint)bVar1;
          FUN_00312e08(DAT_002dd0fc,1);
          break;
        case 4:
          bVar1 = FUN_00312e24(DAT_002dd100);
          uVar4 = (uint)bVar1;
          FUN_00312e08(DAT_002dd0fc,1);
        }
        DAT_002dd0fc[1] = *(uint *)(iVar3 + -0x18);
        bVar1 = FUN_00312d70(DAT_002dd100);
        if ((bVar1) || ((*DAT_002dd100 & 0x100004) == 0x100004)) {
          MMI_diag_stub_00298B64();
          FUN_00312de8(DAT_002dd100);
          FUN_00313f40(DAT_002dd100,*param_3,param_3[1],param_2[2],param_2[3],DAT_002dd0fc,
                       *DAT_002dd108,DAT_002dd108[1],*DAT_002dd10c);
        }
        else {
          FUN_00313f40(DAT_002dd100,*param_3,param_3[1],param_2[2],param_2[3],DAT_002dd0fc,*param_2,
                       param_2[1],*DAT_002dd10c);
        }
        FUN_002e479c();
        if (*(int *)(iVar3 + -0x20) - 3U < 2) {
          FUN_00312e08(DAT_002dd0fc,uVar4);
        }
        uVar2 = 0;
      }
      else {
        uVar2 = 0x70000401;
      }
    }
    else {
      uVar2 = 0x70000401;
    }
  }
  else {
    uVar2 = 0x70000406;
  }
  return uVar2;
}

