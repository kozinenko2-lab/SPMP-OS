/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0045ffcc; original_symbol=native_read_file */

undefined4 native_read_file(uint param_1,int param_2,uint param_3,uint *param_4)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar4;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  undefined4 extraout_r3_02;
  undefined4 uVar5;
  undefined8 uVar6;
  undefined1 auStack_48 [16];
  uint local_38;
  
  if (param_2 == 0 || param_1 == 0) {
    uVar1 = 2;
  }
  else {
    uVar2 = ecos_open(param_2,1,param_3);
    MMI_diag_stub_00298B64();
    if (((int)uVar2 < 0) ||
       (iVar3 = FUN_002eb7b0(uVar2,(uint)auStack_48,extraout_r2,extraout_r3), iVar3 < 0)) {
      MMI_diag_stub_00298B64();
      uVar1 = 2;
    }
    else {
      if (local_38 < param_3) {
        uVar6 = ecos_read(uVar2,param_1,local_38);
        uVar4 = (uint)((ulonglong)uVar6 >> 0x20);
        uVar1 = extraout_r2_00;
        uVar5 = extraout_r3_00;
        if (local_38 != (uint)uVar6) {
          MMI_diag_stub_00298B64();
          uVar4 = extraout_r1;
          uVar1 = extraout_r2_01;
          uVar5 = extraout_r3_01;
        }
      }
      else {
        MMI_diag_stub_00298B64();
        uVar4 = extraout_r1_00;
        uVar1 = extraout_r2_02;
        uVar5 = extraout_r3_02;
      }
      *param_4 = local_38;
      uVar1 = ecos_close(uVar2,uVar4,uVar1,uVar5);
    }
  }
  return uVar1;
}

