/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461c24; original_symbol=native_slot_60 */

undefined4 native_slot_60(uint *param_1,uint param_2,uint param_3,uint param_4,int param_5)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  undefined4 extraout_r3_02;
  undefined8 uVar6;
  undefined4 auStack_24c [128];
  undefined1 auStack_4c [32];
  
  param_3 = param_3 & 0xff;
  MMI_diag_stub_00298B64();
  if (param_2 == 0 || param_1 == (uint *)0x0) {
    MMI_diag_stub_00298B64();
    uVar2 = 0xffffffff;
  }
  else {
    MMI_diag_stub_00298B64();
    if (param_3 == 1) {
      param_3 = 1;
    }
    else if (param_3 != 3) {
      param_3 = 1;
    }
    FUN_00298d34(auStack_24c,0,0x200);
    puVar1 = DAT_00461df0;
    uVar3 = FUN_00346104((uint *)*DAT_00461df0);
    FUN_00298c6c(auStack_24c,(undefined4 *)*puVar1,uVar3);
    iVar4 = FUN_00346104((uint *)*puVar1);
    uVar3 = FUN_00346104(param_1);
    FUN_00298c6c((undefined4 *)((int)auStack_24c + iVar4),param_1,uVar3);
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    uVar3 = ecos_open((int)auStack_24c,param_3,extraout_r2);
    if (((int)uVar3 < 0) ||
       (iVar4 = FUN_002eb7b0(uVar3,(uint)auStack_4c,extraout_r2_00,extraout_r3), iVar4 < 0)) {
      MMI_diag_stub_00298B64();
      uVar2 = 2;
    }
    else {
      MMI_diag_stub_00298B64();
      MMI_diag_stub_00298B64();
      uVar5 = ecos_seek32(uVar3,param_4,0,extraout_r3_00);
      if ((int)uVar5 < 0) {
        ecos_close(uVar3,extraout_r1,extraout_r2_01,extraout_r3_01);
        MMI_diag_stub_00298B64();
        uVar2 = 2;
      }
      else {
        uVar6 = ecos_read(uVar3,param_2,param_5);
        if ((int)uVar6 == param_5) {
          iVar4 = ecos_close(uVar3,(uint)((ulonglong)uVar6 >> 0x20),extraout_r2_02,extraout_r3_02);
          if (iVar4 < 0) {
            MMI_diag_stub_00298B64();
            uVar2 = 2;
          }
          else {
            uVar2 = 0;
          }
        }
        else {
          MMI_diag_stub_00298B64();
          uVar2 = 2;
        }
      }
    }
  }
  return uVar2;
}

