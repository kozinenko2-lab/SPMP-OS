/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00045d78; original_symbol=rb_apply_bootcfg */

void rb_apply_bootcfg(void)

{
  int iVar1;
  undefined4 *puVar2;
  int *piVar3;
  uint uVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  uint extraout_r1_07;
  uint extraout_r1_08;
  uint extraout_r1_09;
  uint extraout_r1_10;
  uint extraout_r1_11;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 extraout_r2_04;
  undefined4 extraout_r2_05;
  undefined4 extraout_r2_06;
  undefined4 extraout_r2_07;
  undefined4 extraout_r2_08;
  undefined4 extraout_r2_09;
  undefined4 extraout_r2_10;
  undefined4 extraout_r2_11;
  undefined4 extraout_r2_12;
  undefined4 uVar5;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  uint extraout_r3_01;
  undefined8 uVar6;
  undefined1 auStack_140 [4];
  uint local_13c;
  uint local_128;
  
  uVar4 = FUN_00033ae4();
  iVar1 = DAT_00045fb8;
  if (uVar4 != 0) {
    uVar4 = DAT_00045fc4;
    if (*(int *)(DAT_00045fb8 + 0x14) == 0) {
      uVar4 = DAT_00045fc0;
    }
    FUN_0004563c(DAT_00045fbc,uVar4,*(undefined4 *)(DAT_00045fb8 + 0x14),DAT_00045fc0);
    uVar4 = DAT_00045fc4;
    if (*(int *)(iVar1 + 0x44) == 0) {
      uVar4 = DAT_00045fc0;
    }
    FUN_0004563c(DAT_00045fc8,uVar4,*(undefined4 *)(iVar1 + 0x44),DAT_00045fc0);
    uVar4 = DAT_00045fc4;
    if (*(int *)(iVar1 + 0x4c) == 0) {
      uVar4 = DAT_00045fc0;
    }
    FUN_0004563c(DAT_00045fcc,uVar4,*(undefined4 *)(iVar1 + 0x4c),DAT_00045fc0);
  }
  *DAT_00045fd0 = 0;
  *DAT_00045fd4 = 0;
  *DAT_00045fd8 = 0;
  *DAT_00045fdc = 0;
  *DAT_00045fe0 = 0;
  *DAT_00045fe4 = 0;
  uVar6 = (**(code **)(DAT_00045fe8 + 0x5c))(auStack_140);
  if ((int)uVar6 == 0) {
    FUN_0004563c(DAT_00045fec,(uint)((ulonglong)uVar6 >> 0x20),extraout_r2,extraout_r3);
  }
  else {
    uVar4 = rb_isp_status_bit();
    if (uVar4 == 0) {
      if ((local_13c & 0x10) != 0) {
        FUN_0004563c(DAT_00045ff4,extraout_r1,extraout_r2_00,local_13c);
        puVar2 = DAT_00045fd0;
        uVar4 = extraout_r1_00;
        uVar5 = extraout_r2_01;
        if ((local_128 & 1) != 0) {
          *DAT_00045fd0 = 1;
          FUN_0004563c(DAT_00045ff8,extraout_r1_00,1,puVar2);
          uVar4 = extraout_r1_01;
          uVar5 = extraout_r2_02;
        }
        if ((local_128 & 8) != 0) {
          FUN_0004563c(DAT_00045ffc,uVar4,uVar5,local_128);
          uVar4 = extraout_r1_02;
          uVar5 = extraout_r2_03;
        }
        puVar2 = DAT_00045fd4;
        if ((local_128 & 2) != 0) {
          *DAT_00045fd4 = 1;
          FUN_0004563c(DAT_00046000,uVar4,1,puVar2);
          uVar4 = extraout_r1_03;
          uVar5 = extraout_r2_04;
        }
        piVar3 = DAT_00045fd8;
        if ((local_128 & 4) != 0) {
          *DAT_00045fd8 = 1;
          FUN_0004563c(DAT_00046004,uVar4,1,piVar3);
          uVar4 = extraout_r1_04;
          uVar5 = extraout_r2_05;
        }
        puVar2 = DAT_00045fdc;
        if ((local_128 & 0x40000000) != 0) {
          *DAT_00045fdc = 1;
          FUN_0004563c(DAT_00046008,uVar4,uVar5,puVar2);
          puVar2 = DAT_00045fe4;
          uVar4 = extraout_r1_05;
          uVar5 = extraout_r2_06;
          if ((local_128 & 0x80) != 0) {
            *DAT_00045fe4 = 1;
            FUN_0004563c(DAT_0004600c,extraout_r1_05,extraout_r2_06,puVar2);
            uVar4 = extraout_r1_06;
            uVar5 = extraout_r2_07;
          }
        }
        puVar2 = DAT_00045fe0;
        if (((local_128 & 0x20000000) != 0) && (*DAT_00045fd8 != 0)) {
          *DAT_00045fe0 = 1;
          FUN_0004563c(DAT_00046010,uVar4,1,puVar2);
          uVar4 = extraout_r1_07;
          uVar5 = extraout_r2_08;
        }
        if ((local_128 & 0x1000000) != 0) {
          FUN_0004563c(DAT_00046014,uVar4,uVar5,local_128);
          uVar4 = extraout_r1_08;
          uVar5 = extraout_r2_09;
        }
        if ((local_128 & 0x2000000) != 0) {
          FUN_0004563c(DAT_00046018,uVar4,uVar5,local_128);
          uVar4 = extraout_r1_09;
          uVar5 = extraout_r2_10;
        }
        if ((local_128 & 0x4000000) != 0) {
          FUN_0004563c(DAT_0004601c,uVar4,uVar5,local_128);
          uVar4 = extraout_r1_10;
          uVar5 = extraout_r2_11;
        }
        if ((local_128 & 0x8000000) != 0) {
          FUN_0004563c(DAT_00046020,uVar4,uVar5,local_128);
          uVar4 = extraout_r1_11;
          uVar5 = extraout_r2_12;
          local_128 = extraout_r3_01;
        }
        FUN_0004563c(DAT_00046024,uVar4,uVar5,local_128);
      }
    }
    else {
      FUN_0004563c(DAT_00045ff0,extraout_r1,extraout_r2_00,extraout_r3_00);
    }
  }
  return;
}

