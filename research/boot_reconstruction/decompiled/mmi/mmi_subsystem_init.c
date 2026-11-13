/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002886f0; original_symbol=mmi_subsystem_init */

undefined4 mmi_subsystem_init(void)

{
  ushort uVar1;
  short *psVar2;
  short *psVar3;
  uint *puVar4;
  uint *puVar5;
  int iVar6;
  undefined4 *puVar7;
  int *piVar8;
  int *piVar9;
  
  MMI_diag_stub_00298B64();
  if (*DAT_0028889c != 0) {
    MMI_diag_stub_00298B64();
    return 1;
  }
  if (*DAT_002888a4 == 0) {
    *DAT_002888a4 = 1;
    FUN_00288674();
  }
  iVar6 = FUN_002831e4(2,DAT_002888a8);
  if (iVar6 != 0) {
    return 0xffffffff;
  }
  iVar6 = FUN_002831f8(2,DAT_002888ac);
  if (iVar6 != 0) {
    return 0xffffffff;
  }
  FUN_002b84b0(2,DAT_002888b0);
  FUN_002b84e8(2,DAT_002888b4);
  MMI_diag_stub_00298B64();
  puVar7 = FUN_002ccd28(DAT_002888bc);
  *DAT_002888c0 = puVar7;
  psVar3 = DAT_002888c4;
  psVar2 = DAT_002888bc;
  if (puVar7 == (undefined4 *)0x0) {
    return 0xffffffff;
  }
  *DAT_002888c4 = *DAT_002888bc;
  psVar3[1] = psVar2[1];
  uVar1 = psVar2[2];
  psVar3[2] = uVar1;
  psVar3[3] = uVar1 >> 9;
  psVar2 = DAT_002888cc;
  piVar9 = DAT_002888c8;
  if (*DAT_002888c8 == 0) {
    DAT_002888cc[1] = psVar3[1];
    psVar2[2] = psVar3[2];
    psVar2[3] = psVar3[3];
    *psVar2 = *psVar3;
    MMI_diag_stub_00298B64();
    piVar8 = mmi_general_alloc((ushort)psVar3[2] + 0x40);
    *piVar9 = (int)piVar8;
    puVar4 = DAT_002888d4;
    *DAT_002888d4 = (int)piVar8 + 0x3fU & 0xffffffc0;
    piVar9 = mmi_general_alloc((ushort)psVar3[2] + 0x40);
    *DAT_002888d8 = piVar9;
    puVar5 = DAT_002888dc;
    *DAT_002888dc = (int)piVar9 + 0x3fU & 0xffffffc0;
    piVar9 = mmi_general_alloc((ushort)psVar3[2] + 0x40);
    *DAT_002888e0 = piVar9;
    *DAT_002888e4 = (int)piVar9 + 0x3fU & 0xffffffc0;
    *DAT_002888e8 = *puVar5;
    *DAT_002888ec = *puVar4 | 0x10000000;
  }
  return 1;
}

