/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003e48a8; original_symbol=mmi_launch_shell */

undefined4 mmi_launch_shell(undefined4 param_1,int param_2)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  int *piVar8;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 uVar9;
  bool bVar10;
  bool bVar11;
  undefined8 uVar12;
  uint local_29c;
  byte *local_298;
  undefined1 auStack_294 [4];
  uint local_290;
  uint local_27c;
  uint local_15c;
  undefined1 local_158;
  int aiStack_5c [3];
  int aiStack_50 [4];
  undefined4 local_40;
  
  bVar10 = false;
  local_298 = (byte *)0x0;
  bVar11 = true;
  uVar2 = FUN_00339ba8();
  FUN_00339bb8(uVar2,0xd);
  iVar3 = (**(code **)(DAT_003e4c68 + 0x5c))(auStack_294);
  if (((iVar3 != 0) && ((local_290 & 0x10) != 0)) &&
     (bVar11 = (local_27c & 0x10) == 0, (local_27c & 0x40) != 0)) {
    bVar10 = true;
  }
  local_15c = *DAT_003e4c6c;
  local_158 = (undefined1)DAT_003e4c6c[1];
  FUN_00345dec(&local_15c,DAT_003e4c70);
  piVar4 = FUN_003e0c90(&local_15c);
  if (piVar4 != (int *)0x0) {
    piVar5 = FUN_003db49c((int)piVar4,aiStack_5c);
    *DAT_003e4c74 = (int)piVar5;
    mmi_general_free((uint)piVar4);
  }
  FUN_003e3a08();
  MMI_diag_stub_00298B64();
  FUN_003b9624();
  if (!bVar10) {
    piVar4 = (int *)FUN_003c0dac(param_2);
    puVar1 = DAT_003e4c6c;
    local_15c = *DAT_003e4c6c;
    local_158 = (undefined1)DAT_003e4c6c[1];
    FUN_00345dec(&local_15c,DAT_003e4c7c);
    piVar5 = FUN_003c26a4(0,(int)&local_15c,0,0,0,10);
    piVar6 = FUN_003c21d0(piVar4);
    piVar7 = (int *)FUN_003c1c48(piVar6,piVar5);
    piVar8 = (int *)FUN_0041c60c(piVar4,0x140,0xf0);
    FUN_003c22f4(piVar8,0,0,0x140,0xf0,piVar6,0,0,0);
    FUN_003c1c48(piVar6,piVar7);
    FUN_003c1d4c(piVar5);
    FUN_003c0e90(piVar6);
    local_15c = *puVar1;
    local_158 = (undefined1)puVar1[1];
    FUN_00345dec(&local_15c,DAT_003e4c80);
    piVar5 = FUN_003c26a4(0,(int)&local_15c,0,0,0,10);
    piVar6 = FUN_003c21d0(piVar4);
    piVar7 = (int *)FUN_003c1c48(piVar6,piVar5);
    piVar8 = (int *)FUN_0041c758(piVar4,0x140,0xf0);
    FUN_003c22f4(piVar8,0,0,0x140,0xf0,piVar6,0,0,0);
    FUN_003c1c48(piVar6,piVar7);
    FUN_003c1d4c(piVar5);
    FUN_003c0e90(piVar6);
    FUN_0041c90c(0,0x30040003);
    FUN_003c0de4(0,piVar4);
    FUN_003b9624();
  }
  piVar4 = FUN_003e1410(*DAT_003e4c74,0,0x50000000,&local_298,param_2,0,param_1,0,DAT_003e4c84);
  *DAT_003e4c88 = piVar4;
  FUN_003e5f3c(param_1,param_2);
  uVar2 = (**(code **)(DAT_003e4c8c + 8))();
  if (799 < uVar2) {
    FUN_002d4d20(1);
  }
  FUN_003bfd38(piVar4,5);
  FUN_003bfebc((int)piVar4);
  FUN_003bff14((int)piVar4);
  *DAT_003e4c90 = 0;
  FUN_002d6d60(0x2d,(uint *)0x0,0);
  local_29c = 0x32;
  MMI_diag_stub_00298B64();
  FUN_002d6d60(0x2e,&local_29c,4);
  uVar2 = FUN_00339ba8();
  uVar12 = FUN_00339bb8(uVar2,0x19);
  uVar9 = extraout_r2;
  if (bVar11) {
    FUN_00315eac();
    uVar12 = FUN_0034d664();
    uVar9 = extraout_r2_00;
  }
  FUN_002d559c((int)uVar12,(uint)((ulonglong)uVar12 >> 0x20),uVar9);
  FUN_002d8774();
  MMI_diag_stub_00298B64();
  while (bVar11 = gui_get_message(aiStack_50,0,0,0), bVar11) {
    gui_translate_message((int)aiStack_50);
    mmi_shell_message_dispatch((int)piVar4,(int)aiStack_50);
  }
  FUN_003bf64c(local_298);
  FUN_003da040((int *)*DAT_003e4c74);
  return local_40;
}

