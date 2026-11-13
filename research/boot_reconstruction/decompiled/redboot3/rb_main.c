/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00046904; original_symbol=rb_main */

void rb_main(void)

{
  bool bVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r1_01;
  uint *extraout_r1_02;
  uint *extraout_r1_03;
  uint extraout_r1_04;
  uint extraout_r1_05;
  uint extraout_r1_06;
  uint extraout_r1_07;
  uint *extraout_r1_08;
  uint *extraout_r1_09;
  uint *extraout_r1_10;
  uint *extraout_r1_11;
  uint *extraout_r1_12;
  uint *extraout_r1_13;
  uint *extraout_r1_14;
  uint *extraout_r1_15;
  uint extraout_r1_16;
  uint *extraout_r1_17;
  uint *extraout_r1_18;
  uint *puVar7;
  uint *extraout_r1_19;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 extraout_r2_02;
  undefined4 extraout_r2_03;
  undefined4 extraout_r2_04;
  undefined4 extraout_r2_05;
  undefined4 extraout_r2_06;
  undefined4 extraout_r2_07;
  int *extraout_r2_08;
  int *extraout_r2_09;
  int *extraout_r2_10;
  int *piVar8;
  int *extraout_r2_11;
  int *extraout_r2_12;
  int *extraout_r2_13;
  int *extraout_r2_14;
  undefined4 extraout_r2_15;
  undefined4 extraout_r2_16;
  int *extraout_r2_17;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 extraout_r3_01;
  undefined4 extraout_r3_02;
  undefined4 extraout_r3_03;
  undefined4 extraout_r3_04;
  undefined4 extraout_r3_05;
  undefined4 extraout_r3_06;
  undefined4 extraout_r3_07;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  int *piVar12;
  int *extraout_r3_08;
  undefined4 *puVar13;
  int *extraout_r3_09;
  int *extraout_r3_10;
  undefined4 extraout_r3_11;
  undefined4 *puVar14;
  int iVar15;
  undefined8 uVar16;
  uint *local_18;
  
  iVar15 = DAT_00046ec8;
  *(undefined4 *)(DAT_00046ec8 + 0x38) = DAT_00046ecc;
  *(undefined4 *)(iVar15 + 0x54) = DAT_00046ed0;
  FUN_00044660(DAT_00046ed4);
  FUN_00044244();
  uVar2 = (**(code **)(iVar15 + 0x34))(0xffffffff);
  uVar16 = (**(code **)(iVar15 + 0x34))(0);
  FUN_00034480((int)uVar16,(int)((ulonglong)uVar16 >> 0x20));
  *(undefined4 *)(iVar15 + 0x5c) = DAT_00046ed8;
  *(undefined4 *)(iVar15 + 0x60) = DAT_00046edc;
  uVar3 = rb_mount_romfs();
  uVar4 = rb_customization_requested(uVar3,extraout_r1,extraout_r2,extraout_r3);
  if (uVar4 != 0) {
    rb_load_customize(uVar4,extraout_r1_00,extraout_r2_00,extraout_r3_00);
    rb_call_customize();
  }
  *(uint *)(DAT_00046ee0 + 0x40) = *(uint *)(DAT_00046ee0 + 0x40) | 7;
  uVar4 = FUN_00033ab8();
  if (uVar4 != 0) {
    FUN_0003688c();
  }
  uVar4 = FUN_00033aa0();
  uVar16 = CONCAT44(extraout_r1_01,uVar4);
  uVar3 = extraout_r2_01;
  uVar9 = extraout_r3_01;
  if (uVar4 == 0) {
    FUN_00033e68(0,extraout_r1_01,extraout_r2_01);
    FUN_00033e50();
    uVar16 = (**(code **)(DAT_00046ec8 + 0x48))(DAT_00046ee4);
    uVar3 = extraout_r2_02;
    uVar9 = extraout_r3_02;
  }
  uVar4 = rb_customization_requested((int)uVar16,(int)((ulonglong)uVar16 >> 0x20),uVar3,uVar9);
  if (uVar4 == 0) {
    *DAT_00046ee8 = 1;
    *DAT_00046eec = 0;
    *DAT_00046ef0 = 0xf00000;
    *DAT_00046ef8 = DAT_00046ef4;
    *DAT_00046efc = 0xf00000;
  }
  else {
    rb_call_customize();
    *DAT_00046ee8 = 1;
    *DAT_00046eec = 0;
    *DAT_00046ef0 = 0xf00000;
    *DAT_00046ef8 = DAT_00046ef4;
    *DAT_00046efc = 0xf00000;
  }
  *DAT_00046f00 = 0xffffffff;
  (*DAT_00046f04)();
  *DAT_00046f08 = 1;
  rb_apply_bootcfg();
  if (DAT_00046f0c != DAT_00046f10) {
    puVar13 = DAT_00046f0c;
    do {
      puVar14 = puVar13 + 1;
      (*(code *)*puVar13)();
      puVar13 = puVar14;
    } while (puVar14 != DAT_00046f10);
  }
  puVar13 = DAT_00046f14;
  *DAT_00046f14 = *DAT_00046ef8;
  puVar13[1] = *DAT_00046efc;
  uVar16 = FUN_00045adc(0,0,puVar13);
  uVar4 = rb_customization_requested
                    ((int)uVar16,(int)((ulonglong)uVar16 >> 0x20),extraout_r2_03,extraout_r3_03);
  puVar7 = extraout_r1_02;
  if (uVar4 != 0) {
    rb_call_customize();
    puVar7 = extraout_r1_03;
  }
  piVar8 = DAT_00046f18;
  iVar15 = *DAT_00046f18;
  if (iVar15 == 0) {
    piVar12 = (int *)0x0;
    if (*DAT_00046f1c == 0) goto LAB_00046ec0;
    *DAT_00046f18 = 1;
    if (*DAT_00046f20 == 0) {
      uVar5 = FUN_0003e8e8(DAT_00046f34);
      uVar4 = extraout_r1_07;
      uVar3 = extraout_r2_07;
      uVar9 = extraout_r3_07;
    }
    else if (*DAT_00046f24 == 0) {
      if (*DAT_00046f2c == 0) {
        uVar5 = rb_buffered_load(DAT_00046f28,DAT_00046f34,piVar8,0);
        uVar4 = extraout_r1_06;
        uVar3 = extraout_r2_06;
        uVar9 = extraout_r3_06;
      }
      else {
        uVar5 = rb_buffered_load(DAT_00046f30,DAT_00046f34,piVar8,*DAT_00046f2c);
        uVar4 = extraout_r1_05;
        uVar3 = extraout_r2_05;
        uVar9 = extraout_r3_05;
      }
    }
    else {
      uVar5 = rb_fast_load(DAT_00046f28);
      uVar4 = extraout_r1_04;
      uVar3 = extraout_r2_04;
      uVar9 = extraout_r3_04;
    }
    if (uVar5 == 0) {
      FUN_0004563c(DAT_00046f48,uVar4,uVar3,uVar9);
      puVar7 = extraout_r1_10;
      piVar8 = extraout_r2_08;
    }
    else {
      FUN_0004563c(DAT_00046f38,uVar5,uVar3,uVar9);
      if (*DAT_00046f2c == 0) {
        FUN_00045890(DAT_00046f3c,DAT_00046f44,uVar5,0);
        puVar7 = extraout_r1_09;
      }
      else {
        FUN_00045890(DAT_00046f3c,DAT_00046f40,uVar5,*DAT_00046f2c);
        puVar7 = extraout_r1_08;
      }
      piVar8 = DAT_00046f3c;
      *DAT_00046f18 = (int)DAT_00046f3c;
    }
    iVar15 = *DAT_00046f18;
    piVar12 = DAT_00046f18;
    if (iVar15 == 0) goto LAB_00046ec0;
  }
  piVar12 = DAT_00046f24;
  if (*DAT_00046f24 == 0) {
    iVar6 = *DAT_00046f08 * 10;
    iVar10 = (int)((ulonglong)((longlong)iVar6 * (longlong)DAT_00046f4c) >> 0x20);
    uVar4 = (iVar10 >> 6) - (iVar6 >> 0x1f);
    FUN_0004563c(DAT_00046f50,uVar4,iVar6 + uVar4 * -1000,iVar10);
    *DAT_00046f18 = 0;
    puVar7 = extraout_r1_11;
    if (9 < iVar6) {
      do {
        uVar16 = FUN_00047e88(DAT_00046f54,0x100,10);
        puVar7 = (uint *)((ulonglong)uVar16 >> 0x20);
        iVar10 = (int)uVar16;
        if (iVar10 < 1) {
          piVar8 = extraout_r2_09;
          if (iVar10 != -1) break;
          iVar6 = iVar6 + -10;
        }
        else {
          iVar11 = (int)((ulonglong)((longlong)iVar6 * (longlong)DAT_00046f4c) >> 0x20);
          uVar4 = (iVar11 >> 6) - (iVar6 >> 0x1f);
          FUN_0004563c(DAT_00046f50,uVar4,iVar6 + uVar4 * -1000,iVar11);
          puVar7 = extraout_r1_12;
          piVar8 = extraout_r2_10;
        }
      } while (9 < iVar6);
      piVar12 = DAT_00046f18;
      if (iVar10 != -2) {
        *DAT_00046f18 = iVar15;
        goto LAB_00046ec0;
      }
    }
    piVar12 = DAT_00046f18;
    piVar8 = (int *)0x0;
    *DAT_00046f18 = 0;
  }
LAB_00046ec0:
  bVar1 = true;
  do {
    while( true ) {
      while( true ) {
        while( true ) {
          if (bVar1) {
            FUN_0004563c(DAT_00046f58,(uint)puVar7,piVar8,piVar12);
            bVar1 = false;
          }
          puVar13 = DAT_00046f5c;
          *DAT_00046f5c = 1;
          uVar16 = FUN_00047e88(DAT_00046f54,0x100,10);
          puVar7 = (uint *)((ulonglong)uVar16 >> 0x20);
          iVar15 = (int)uVar16;
          *puVar13 = 0;
          if (iVar15 != -1) break;
          piVar12 = (int *)*DAT_00046f60;
          piVar8 = extraout_r2_11;
          if (piVar12 == (int *)0x0) {
            FUN_000365dc(0xffffffff,puVar7,(uint)extraout_r2_11);
            puVar7 = extraout_r1_13;
            piVar8 = extraout_r2_12;
            piVar12 = extraout_r3_08;
          }
        }
        uVar4 = rb_customization_requested(iVar15,puVar7,extraout_r2_11,0);
        if (uVar4 != 0) {
          rb_call_customize();
        }
        if (iVar15 != 0) break;
        *DAT_00046f64 = 1;
        iVar15 = 0;
        do {
          iVar6 = DAT_00046ec8;
          (**(code **)(DAT_00046ec8 + 0x34))(iVar15);
          puVar13 = *(undefined4 **)(iVar6 + 0x14);
          (*(code *)puVar13[5])(*puVar13,4);
          iVar15 = iVar15 + 1;
        } while (iVar15 != 2);
        (**(code **)(iVar6 + 0x34))(uVar2);
        uVar4 = *DAT_00046efc & 0xfffffff0;
        puVar13 = (undefined4 *)(uVar4 - 0x50);
        iVar15 = 0;
        do {
          puVar13[iVar15] = iVar15;
          iVar15 = iVar15 + 1;
        } while (iVar15 != 0xb);
        *puVar13 = DAT_00046f68;
        *(uint *)(uVar4 - 0x1c) = uVar4;
        uVar3 = DAT_00046f6c;
        *(undefined4 *)(uVar4 - 0x18) = DAT_00046f6c;
        *(undefined4 *)(uVar4 - 0x14) = uVar3;
        *(undefined4 *)(uVar4 - 0x10) = 0x13;
        puVar7 = DAT_00046efc;
        *DAT_00046efc = (uint)puVar13;
        FUN_0004ab00(puVar7,DAT_00046f70,puVar13,0x13);
        *DAT_00046f64 = 0;
        iVar15 = DAT_00046ec8;
        (**(code **)(DAT_00046ec8 + 0x30))(0xffffffff);
        (**(code **)(iVar15 + 0x34))();
        puVar7 = extraout_r1_14;
        piVar8 = extraout_r2_13;
        piVar12 = extraout_r3_09;
      }
      local_18 = DAT_00046f54;
      piVar12 = (int *)(uint)(byte)*DAT_00046f54;
      piVar8 = (int *)(uint)(piVar12 == (int *)0x3d);
      puVar7 = DAT_00046f54;
      if (piVar12 != (int *)0x3d && piVar12 != (int *)0x23) break;
      if (piVar8 == (int *)0x0) goto LAB_00046cb4;
      FUN_0004563c(DAT_00046f74,(uint)((int)DAT_00046f54 + 2),piVar8,piVar12);
      bVar1 = true;
      puVar7 = extraout_r1_15;
      piVar8 = extraout_r2_14;
      piVar12 = extraout_r3_10;
    }
    while ((byte)*local_18 != 0) {
      iVar15 = FUN_000484d4(&local_18,DAT_00046f78,(byte *)DAT_00046f7c);
      if (iVar15 == 0) {
        FUN_0004563c(DAT_00046f90,*DAT_00046f7c,extraout_r2_15,DAT_00046f7c);
        puVar7 = extraout_r1_19;
        piVar8 = extraout_r2_17;
      }
      else {
        *DAT_00046f84 = DAT_00046f80;
        iVar6 = FUN_0004ab3c(DAT_00046f88);
        if (iVar6 == 0) {
          (**(code **)(iVar15 + 0xc))(*DAT_00046f78,DAT_00046f7c);
          puVar7 = extraout_r1_18;
        }
        else {
          FUN_0004563c(DAT_00046f8c,extraout_r1_16,extraout_r2_16,extraout_r3_11);
          puVar7 = extraout_r1_17;
        }
        piVar8 = (int *)0x0;
        *DAT_00046f84 = 0;
      }
    }
    piVar12 = (int *)0x0;
LAB_00046cb4:
    bVar1 = true;
  } while( true );
}

