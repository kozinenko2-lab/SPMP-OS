/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d3d38; original_symbol=mmi_game_request_handler */

int mmi_game_request_handler(undefined2 param_1,int *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  uint *puVar3;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint uVar4;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  undefined8 uVar8;
  
  iVar7 = 0;
  FUN_00298c6c(DAT_002d4094,param_2,0x14);
  MMI_diag_stub_00298B64();
  puVar3 = DAT_002d409c;
  switch(param_1) {
  case 0xd01:
    FUN_003b5318();
    FUN_002d2e0c();
    native_clock_prepare();
    puVar2 = DAT_002d4094;
    MMI_diag_stub_00298B64();
    ecos_thread_create_wrapper
              (0x18,DAT_002d40a8,puVar2[2],DAT_002d40ac,puVar2[3],puVar2[4],DAT_002d409c,
               DAT_002d40a4);
    uVar4 = extraout_r1_01;
    uVar5 = extraout_r2;
    if ((*(uint *)*DAT_002d40b0 & 1) != 0) {
      MMI_diag_stub_00298B64();
      uVar4 = extraout_r1_02;
      uVar5 = extraout_r2_00;
    }
    ecos_thread_resume_wrapper((int *)*DAT_002d409c,uVar4,uVar5);
    break;
  case 0xd02:
    break;
  case 0xd03:
    FUN_002d2e0c();
    FUN_002d2e50(param_2[1]);
    if (*(code **)(DAT_002d40b8 + 8) == (code *)0x0) {
      iVar7 = -1;
    }
    else {
      iVar7 = (**(code **)(DAT_002d40b8 + 8))(*param_2);
      if ((*(uint *)*DAT_002d40b0 & 2) != 0) {
        MMI_diag_stub_00298B64();
      }
    }
    break;
  case 0xd04:
    FUN_003b5318();
    iVar7 = -1;
    if ((short)DAT_002d40a4[0x12] != 0) {
      iVar6 = DAT_002d40a4[0xe];
      iVar1 = 0x13;
      while (iVar6 != 0x10) {
        if (*(code **)(DAT_002d40b8 + 0xc) != (code *)0x0) {
          uVar8 = (**(code **)(DAT_002d40b8 + 0xc))(*param_2);
          iVar7 = (int)uVar8;
          iVar6 = iVar7;
          if ((*(uint *)*DAT_002d40b0 & 1) != 0) {
            iVar6 = DAT_002d40c0;
          }
          FUN_003f8188(iVar6,(int)((ulonglong)uVar8 >> 0x20));
        }
        ecos_thread_delay_wrapper(10,0);
        if (iVar1 < 1) break;
        iVar1 = iVar1 + -1;
        iVar6 = DAT_002d40a4[0xe];
      }
    }
    native_clock_restore();
    break;
  case 0xd05:
    if (*(code **)(DAT_002d40b8 + 0x10) == (code *)0x0) {
      iVar7 = -1;
    }
    else {
      iVar7 = (**(code **)(DAT_002d40b8 + 0x10))(0);
      if ((*(uint *)*DAT_002d40b0 & 2) != 0) {
        MMI_diag_stub_00298B64();
      }
    }
    break;
  case 0xd06:
    FUN_002d3d18(1);
    FUN_0034cdc8(DAT_002d40d0);
    iVar7 = FUN_0029e830();
    if (iVar7 < 0) {
      iVar7 = 0x70000006;
    }
    else {
      iVar7 = 0;
    }
    break;
  case 0xd07:
    FUN_002d3d18(0);
    FUN_0034cdc8(DAT_002d40d4);
    if (*DAT_002d40cc == 0) {
      iVar7 = 0x70000006;
    }
    else {
      iVar7 = FUN_0029e830();
      if (iVar7 < 0) {
        iVar7 = 0x70000006;
      }
      else {
        iVar7 = 0;
      }
    }
    break;
  case 0xd08:
    iVar7 = FUN_0029e830();
    if (iVar7 < 0) {
      iVar7 = 0x70000006;
    }
    else {
      *DAT_002d40cc = *param_2;
      iVar7 = 0;
    }
    break;
  case 0xd09:
    if (*(code **)(DAT_002d40b8 + 0x14) == (code *)0x0) {
      iVar7 = -1;
    }
    else {
      iVar7 = (**(code **)(DAT_002d40b8 + 0x14))(0);
      if ((*(uint *)*DAT_002d40b0 & 2) != 0) {
        MMI_diag_stub_00298B64();
      }
    }
    break;
  case 0xd0a:
    uVar4 = extraout_r1;
    if (*DAT_002d409c != 0) {
      do {
        iVar7 = ecos_thread_delete_wrapper(*puVar3,uVar4);
        ecos_thread_delay_wrapper(3,0);
        uVar4 = extraout_r1_00;
      } while (iVar7 == 0);
      *DAT_002d409c = 0;
    }
    break;
  default:
    iVar7 = 0x70000007;
  }
  return iVar7;
}

