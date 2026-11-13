/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dd2bc; original_symbol=native_slot_51 */

undefined4 native_slot_51(short *param_1,int param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iStack_8c;
  undefined4 *puStack_7c;
  undefined2 uStack_78;
  undefined2 uStack_76;
  undefined4 auStack_74 [18];
  
  uVar9 = param_3 & 0xff;
  param_4 = param_4 & 0xff;
  uVar1 = uVar9;
  if (uVar9 < 0x19) {
    uVar1 = param_4;
  }
  if (uVar1 < 0x19) {
    uVar4 = gfx_stage_312f98((int)DAT_002dd4dc);
    FUN_00298d34(auStack_74,0,0x48);
    uVar1 = (int)(uVar9 + 7) >> 3;
    uVar2 = (int)(param_4 + 7) >> 3;
    iVar7 = 0;
    iStack_8c = 0;
    uVar8 = 0;
    if (param_4 != 0) {
      do {
        uVar6 = 0;
        if (uVar1 != 0) {
          do {
            if (((param_3 & 7) == 0) || (uVar5 = param_3 & 7, uVar6 < uVar1 - 1)) {
              uVar5 = 8;
            }
            uVar5 = FUN_002dd4e8(param_2,uVar8 * uVar9 + uVar6 * 8,uVar5);
            *(char *)((int)auStack_74 + iStack_8c + (uVar6 * 3 + iVar7) * 8) = (char)uVar5;
            uVar6 = uVar6 + 1;
          } while (uVar6 < uVar1);
        }
        iStack_8c = iStack_8c + 1;
        if (iStack_8c == 8) {
          iStack_8c = 0;
          iVar7 = iVar7 + 1;
        }
        uVar8 = uVar8 + 1;
      } while (uVar8 < param_4);
    }
    FUN_0029e450();
    native_set_rop(0xf0);
    uStack_78 = 8;
    uStack_76 = 8;
    FUN_00312f48(DAT_002dd4dc,(uint)&puStack_7c,0);
    uVar9 = 0;
    if (uVar2 != 0) {
      do {
        uVar8 = 0;
        if (uVar1 != 0) {
          do {
            puStack_7c = auStack_74 + (uVar8 * 3 + uVar9) * 2;
            gfx_blit_dispatch(DAT_002dd4dc,*param_1 + (short)uVar8 * 8,param_1[1] + (short)uVar9 * 8
                              ,8,8,DAT_002dd4e0,0,0,*DAT_002dd4e4);
            uVar8 = uVar8 + 1;
          } while (uVar8 < uVar1);
        }
        uVar9 = uVar9 + 1;
      } while (uVar9 < uVar2);
    }
    native_graphics_finish();
    FUN_00312f48(DAT_002dd4dc,0,0);
    if (uVar4 == 0) {
      gfx_stage_312fa0(DAT_002dd4dc);
    }
    else {
      FUN_00312f48(DAT_002dd4dc,uVar4,1);
    }
    uVar3 = 0;
  }
  else {
    uVar3 = 0x70000401;
  }
  return uVar3;
}

