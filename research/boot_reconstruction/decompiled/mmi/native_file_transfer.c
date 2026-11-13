/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461a54; original_symbol=native_file_transfer */

undefined4 native_file_transfer(uint *param_1,uint param_2,byte param_3,uint param_4,int param_5)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  uint uVar5;
  undefined8 uVar6;
  undefined4 auStack_22c [128];
  
  MMI_diag_stub_00298B64();
  if (param_2 == 0 || param_1 == (uint *)0x0) {
    MMI_diag_stub_00298B64();
    return 0xffffffff;
  }
  if (param_3 == 3) {
    uVar5 = 3;
  }
  else {
    if (param_3 < 4) {
      if (param_3 == 2) {
        uVar5 = 2;
        goto LAB_00461ad0;
      }
    }
    else if (param_3 == 8) {
      uVar5 = 10;
      goto LAB_00461ad0;
    }
    uVar5 = 10;
  }
LAB_00461ad0:
  FUN_00298d34(auStack_22c,0,0x200);
  puVar1 = DAT_00461c08;
  uVar2 = FUN_00346104((uint *)*DAT_00461c08);
  FUN_00298c6c(auStack_22c,(undefined4 *)*puVar1,uVar2);
  iVar3 = FUN_00346104((uint *)*puVar1);
  uVar2 = FUN_00346104(param_1);
  FUN_00298c6c((undefined4 *)((int)auStack_22c + iVar3),param_1,uVar2);
  MMI_diag_stub_00298B64();
  MMI_diag_stub_00298B64();
  uVar5 = ecos_open((int)auStack_22c,uVar5,extraout_r2);
  if ((int)uVar5 < 0) {
    MMI_diag_stub_00298B64();
    uVar4 = 2;
  }
  else {
    uVar2 = ecos_seek32(uVar5,param_4,0,extraout_r3);
    if ((int)uVar2 < 0) {
      MMI_diag_stub_00298B64();
      ecos_close(uVar5,extraout_r1,extraout_r2_00,extraout_r3_00);
      uVar4 = 2;
    }
    else {
      uVar6 = ecos_write(uVar5,param_2,param_5);
      uVar2 = (uint)((ulonglong)uVar6 >> 0x20);
      if ((int)uVar6 == param_5) {
        iVar3 = ecos_close(uVar5,uVar2,extraout_r2_01,extraout_r3_01);
        if (iVar3 < 0) {
          MMI_diag_stub_00298B64();
          uVar4 = 2;
        }
        else {
          uVar4 = 0;
        }
      }
      else {
        ecos_close(uVar5,uVar2,extraout_r2_01,extraout_r3_01);
        MMI_diag_stub_00298B64();
        uVar4 = 2;
      }
    }
  }
  return uVar4;
}

