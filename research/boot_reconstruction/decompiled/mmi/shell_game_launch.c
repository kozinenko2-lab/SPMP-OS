/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00430694; original_symbol=shell_game_launch */

undefined4 shell_game_launch(int param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  uint uVar7;
  uint extraout_r1;
  int iVar8;
  byte *pbVar9;
  uint local_44 [6];
  int local_2c;
  uint local_28;
  undefined4 local_24;
  
  if (*DAT_00430aa0 == 1) {
    iVar4 = shell_game_stack_allocate();
    if (iVar4 != 0) {
      return 1;
    }
    FUN_00431f08(6,0,param_1);
    *DAT_00430aa4 = *DAT_00430aa4 | 2;
  }
  if ((*DAT_00430aa4 & 2) == 0) {
    uVar5 = 1;
  }
  else {
    if ((DAT_00430aa8[6] & 1U) == 0) {
      FUN_003e3958(1);
      cVar3 = FUN_003e30e0();
      if (((cVar3 == '\0') && (cVar3 = FUN_003e372c(), cVar3 == '\x01')) &&
         (cVar3 = FUN_003e3948(), cVar3 == '\0')) {
        FUN_002d6d60(0x12,(uint *)0x0,0);
      }
      piVar1 = DAT_00430aa8;
      DAT_00430aa8[6] = DAT_00430aa8[6] | 1;
      iVar8 = piVar1[4] + *DAT_00430aac * 0x460;
      iVar4 = FUN_00346104((uint *)(iVar8 + 0x60));
      iVar8 = FUN_00346104((uint *)(iVar8 + 0x260));
      iVar4 = iVar4 + iVar8;
      if (199 < iVar4) {
        mmi_general_free(piVar1[5]);
        piVar6 = mmi_general_alloc(iVar4 + 2);
        piVar1[5] = (int)piVar6;
      }
      piVar1 = DAT_00430aa8;
      iVar8 = DAT_00430aa8[4] + *DAT_00430aac * 0x460;
      FUN_0033e2e0(DAT_00430aa8[5],DAT_00430ab0,iVar8 + 0x260,iVar8 + 0x60);
      puVar2 = DAT_00430ab4;
      (*(code *)*DAT_00430ab4)(0);
      (*(code *)puVar2[7])();
      (*(code *)puVar2[6])();
      local_44[0] = 0;
      local_44[1] = 1;
      local_44[2] = 1;
      local_44[3] = 0;
      uVar7 = FUN_002d6d60(0xb01,local_44,0x10);
      FUN_003dbec0(uVar7,extraout_r1);
      FUN_003c2f3c(1);
      pbVar9 = (byte *)(iVar4 + piVar1[5] + -3);
      iVar4 = FUN_002db788(pbVar9,DAT_00430ab8);
      if (((iVar4 == 0) || (iVar4 = FUN_002db788(pbVar9,DAT_00430abc), iVar4 == 0)) ||
         (iVar4 = FUN_002db788(pbVar9,DAT_00430ac0), iVar4 == 0)) {
        DAT_00430aa8[6] = DAT_00430aa8[6] | 0x400;
        FUN_004315b0();
      }
      else {
        DAT_00430aa8[6] = DAT_00430aa8[6] & 0xfffffbff;
        FUN_0043154c();
      }
      cVar3 = FUN_003e30e0();
      if (cVar3 != '\0') {
        local_44[0] = 0;
        local_44[1] = 1;
        if (*DAT_00430ac4 == 0) {
          local_44[2] = 0;
          local_44[3] = 0;
          FUN_002d6d60(0xb01,local_44,0x10);
          FUN_003e3138(0xa0);
        }
        else {
          local_44[2] = 1;
          local_44[3] = 0;
          FUN_002d6d60(0xb01,local_44,0x10);
        }
        iVar4 = FUN_002db788(pbVar9,DAT_00430ab8);
        if ((iVar4 == 0) || (iVar4 = FUN_002db788(pbVar9,DAT_00430ac0), iVar4 == 0)) {
          FUN_002d6d60(0xb0b,(uint *)0x1,0);
        }
        else {
          FUN_002d6d60(0xb0b,(uint *)0x0,0);
        }
      }
      piVar1 = DAT_00430ac8;
      FUN_003bfd38((int *)*DAT_00430ac8,5);
      FUN_003bead4(*piVar1,0xf,0,0);
      cVar3 = FUN_003e30e0();
      if (cVar3 != '\0') {
        FUN_003e3958(0);
        FUN_002d6d60(0x11,(uint *)0x0,0);
      }
      FUN_003e32cc('\0');
      FUN_003e34a0('\0');
      cVar3 = FUN_003e30e0();
      piVar1 = DAT_00430acc;
      if ((cVar3 != '\0') && (*DAT_00430acc == 0)) {
        FUN_002d6d60(0xb0f,(uint *)0x0,0);
        *piVar1 = 1;
      }
      FUN_003bead4(*DAT_00430ad0,0x111,10,0);
      if (*DAT_00430aa0 == 1) {
        FUN_003bee44(*DAT_00430ad4,0x549,0,0);
      }
      else {
        FUN_003bee44(*DAT_00430aa8,0x549,0,0);
      }
      local_2c = DAT_00430aa8[5];
      local_28 = *DAT_00430ad8 + 3U & 0xfffffffc;
      local_24 = 0x100000;
      FUN_002d6d60(0xd01,local_44 + 4,0x14);
    }
    uVar5 = 0;
  }
  return uVar5;
}

