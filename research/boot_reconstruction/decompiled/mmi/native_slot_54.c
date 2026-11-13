/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461fb4; original_symbol=native_slot_54 */

int native_slot_54(uint *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  
  puVar3 = (uint *)*DAT_004620e8;
  MMI_diag_stub_00298B64();
  if (param_2 == (uint *)0x0 || puVar3 == (uint *)0x0) {
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    *DAT_00462100 = 0;
    return 0;
  }
  iVar1 = FUN_00345f54(param_1,DAT_00462104);
  if (iVar1 == 0) {
    return 4;
  }
  iVar1 = FUN_00345f54(param_1,DAT_00462108);
  if (iVar1 == 0) {
    return 8;
  }
  do {
    iVar1 = FUN_00345f54(puVar3,DAT_0046210c);
    if (iVar1 == 0) {
LAB_004620a0:
      iVar1 = FUN_00346104(param_1);
      iVar1 = FUN_00345f54((uint *)((int)param_1 + iVar1 + -4),DAT_00462110);
      if (iVar1 == 0) {
        return 2;
      }
      iVar1 = FUN_00346104(param_1);
      uVar2 = FUN_00345f54((uint *)((int)param_1 + iVar1 + -4),DAT_00462114);
      iVar1 = 1 - uVar2;
      if (1 < uVar2) {
        iVar1 = 0;
      }
      return iVar1;
    }
    iVar1 = FUN_00345f54(puVar3,param_1);
    if (iVar1 == 0) {
      uVar2 = FUN_00461268((undefined4 *)puVar3[4]);
      param_2[1] = uVar2;
      *param_2 = puVar3[4];
      goto LAB_004620a0;
    }
    puVar3 = puVar3 + 5;
  } while( true );
}

