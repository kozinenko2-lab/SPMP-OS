/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002ddf70; original_symbol=native_set_display_screen */

undefined4 native_set_display_screen(short *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  
  puVar2 = DAT_002de010;
  piVar1 = DAT_002de00c;
  if ((*param_1 < 0) || (param_1[1] < 0)) {
    return 0x70000401;
  }
  iVar7 = *DAT_002de00c;
  uVar4 = (*(code *)*DAT_002de010)();
  *(undefined2 *)(iVar7 + 8) = uVar4;
  puVar3 = DAT_002de014;
  iVar7 = *piVar1;
  uVar4 = (*(code *)*DAT_002de014)();
  *(undefined2 *)(iVar7 + 10) = uVar4;
  iVar7 = DAT_002de018;
  uVar5 = (*(code *)*DAT_002de01c)();
  *(undefined4 *)(iVar7 + 4) = uVar5;
  uVar6 = (*(code *)*puVar2)();
  uVar4 = (*(code *)*puVar3)();
  gfx_context_dimensions(iVar7,uVar6,uVar4);
  return 0;
}

