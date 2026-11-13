/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00460ab0; original_symbol=native_game_thread */

int native_game_thread(uint *param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined1 *puVar5;
  int iVar6;
  uint uVar7;
  int aiStack_58 [10];
  uint uStack_30;
  undefined4 uStack_2c;
  
  ecos_thread_delay_wrapper(10,0);
  MMI_diag_stub_00298B64();
  iVar4 = FUN_00346104(param_1);
  iVar6 = iVar4;
  do {
    iVar1 = iVar6;
    iVar6 = iVar1 + -1;
    if (iVar6 < 0) goto LAB_00460b10;
  } while (*(char *)((int)param_1 + iVar6) != '.');
  if (iVar6 == 0) {
    iVar6 = 4;
  }
  else {
LAB_00460b10:
    uVar7 = (iVar4 - iVar6) - 1;
    FUN_00346320(&uStack_2c,(uint *)((int)param_1 + iVar1),uVar7);
    iVar6 = 0;
    if (0 < (int)uVar7) {
      do {
        iVar4 = FUN_004be770((uint)*(byte *)((int)aiStack_58 + iVar6 + 0x2c));
        *(char *)((int)aiStack_58 + iVar6 + 0x2c) = (char)iVar4;
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)uVar7);
    }
    *DAT_00460fd8 = -1;
    switch((undefined1)uStack_2c) {
    case 0x42:
      if ((uStack_2c._1_1_ == 'I') || (uStack_2c._2_1_ == 'N')) {
        *DAT_00460fd8 = 6;
      }
      break;
    case 0x47:
      if ((int)uVar7 < 3) {
        *DAT_00460fd8 = 1;
      }
      else if (uStack_2c._2_1_ == 'C') {
        *DAT_00460fd8 = 1;
      }
      else if (uStack_2c._2_1_ == 'A') {
        *DAT_00460fd8 = 4;
      }
      break;
    case 0x4e:
      *DAT_00460fd8 = 0;
      break;
    case 0x53:
      if ((uStack_2c._1_1_ == 'F') || (uStack_2c._2_1_ == 'C')) {
        *DAT_00460fd8 = 2;
      }
      else if (uStack_2c._1_1_ == 'M') {
        *DAT_00460fd8 = 3;
      }
      else if (uStack_2c._1_1_ == 'W') {
        *DAT_00460fd8 = 5;
      }
      else if (uStack_2c._1_1_ == 'G') {
        *DAT_00460fd8 = 8;
      }
    }
    if (*DAT_00460fd8 == 8 || *DAT_00460fd8 == 6) {
      iVar6 = mmi_game_pool_alloc(0x25800);
      *DAT_00460fdc = iVar6;
      iVar6 = mmi_game_pool_alloc(0x25800);
      *DAT_00460fe0 = iVar6;
    }
    if (*DAT_00460fd8 == 5) {
      iVar6 = mmi_game_pool_alloc(0x20000);
      *DAT_00460fe4 = iVar6;
      *DAT_00460fe8 = 0x20000;
      if (iVar6 == 0) {
        MMI_diag_stub_00298B64();
        FUN_002d8b88(0x560,0,0);
      }
    }
    else if (*DAT_00460fd8 == 4) {
      mmi_game_pool_info(aiStack_58);
      puVar5 = (undefined1 *)mmi_game_pool_alloc(uStack_30 - 0x80000);
      *DAT_00460fe4 = (int)puVar5;
      if (&SUB_00b00000 < puVar5) {
        MMI_diag_stub_00298B64();
        FUN_002d8b88(0x560,0,0);
      }
      else {
        *DAT_00460fe8 = puVar5 + (uStack_30 - 0xb80000);
        MMI_diag_stub_00298B64();
      }
    }
    else {
      mmi_game_pool_info(aiStack_58);
      puVar5 = (undefined1 *)mmi_game_pool_alloc(uStack_30);
      *DAT_00460fe4 = (int)puVar5;
      if (&SUB_00a00000 < puVar5) {
        MMI_diag_stub_00298B64();
        FUN_002d8b88(0x560,0,0);
      }
      else {
        *DAT_00460fe8 = puVar5 + (uStack_30 - 0xa00000);
        MMI_diag_stub_00298B64();
      }
    }
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    switch(*DAT_00460fd8) {
    case 0:
      iVar6 = native_n1_loader((int)param_1);
      break;
    case 1:
      iVar6 = FUN_00460280(param_1);
      break;
    case 2:
      iVar6 = FUN_004603a4(param_1);
      break;
    case 3:
      iVar6 = FUN_004604c8(param_1);
      break;
    case 4:
      iVar6 = FUN_004605e8(param_1);
      break;
    case 5:
      iVar6 = FUN_00460708(param_1);
      break;
    case 6:
      iVar6 = native_bin_loader((int)param_1);
      break;
    default:
      iVar6 = -2;
      break;
    case 8:
      native_clock_restore();
      iVar6 = FUN_00460944(param_1);
    }
    MMI_diag_stub_00298B64();
    if (iVar6 == 1) {
      FUN_002d8b88(0x560,0,0);
    }
    else if (iVar6 != 0) {
      FUN_002d8b88(0x560,0,0);
      if ((iVar6 + 5U < 4) && ((1 << (iVar6 + 5U & 0xff) & 0xdU) != 0)) {
        FUN_002d8b88(0x555,0x70000002,0);
      }
    }
    puVar3 = DAT_00461000;
    *DAT_00461000 = 0;
    puVar3[1] = 0;
    puVar3[2] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
    puVar3[6] = 0;
    piVar2 = DAT_00460fe4;
    mmi_game_pool_free((int *)*DAT_00460fe4);
    *piVar2 = 0;
    piVar2 = DAT_00460fdc;
    if (*DAT_00460fd8 == 8 || *DAT_00460fd8 == 6) {
      mmi_game_pool_free((int *)*DAT_00460fdc);
      *piVar2 = 0;
      piVar2 = DAT_00460fe0;
      mmi_game_pool_free((int *)*DAT_00460fe0);
      *piVar2 = 0;
    }
  }
  return iVar6;
}

