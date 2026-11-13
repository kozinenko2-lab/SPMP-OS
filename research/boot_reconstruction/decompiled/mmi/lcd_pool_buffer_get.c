/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002b97cc; original_symbol=lcd_pool_buffer_get */

undefined4 lcd_pool_buffer_get(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = DAT_002b98bc;
  iVar1 = (**(code **)(DAT_002b98bc + 8))();
  iVar2 = (**(code **)(iVar3 + 0xc))();
  iVar3 = DAT_002b98c0;
  uVar4 = iVar2 * iVar1 * 2;
  if (param_1 == 0) {
    if (1 < param_2) {
      return 0;
    }
    if (*(int *)(param_2 * 4 + DAT_002b98c0) == 0) {
      iVar1 = mmi_game_pool_alloc(uVar4);
      *(int *)(param_2 * 4 + iVar3) = iVar1;
      if (iVar1 == 0) {
        MMI_diag_stub_00298B64();
      }
    }
    return *(undefined4 *)(DAT_002b98c0 + param_2 * 4);
  }
  if (param_1 != 1) {
    return 0;
  }
  if (3 < param_2) {
    return 0;
  }
  if (*(int *)(DAT_002b98c8 + param_2 * 4) == 0) {
    if (uVar4 < 0xb7840) {
      uVar4 = 0xb7840;
    }
    iVar3 = mmi_game_pool_alloc(uVar4);
    *(int *)(DAT_002b98c8 + param_2 * 4) = iVar3;
    if (iVar3 == 0) {
      MMI_diag_stub_00298B64();
    }
  }
  return *(undefined4 *)(DAT_002b98c8 + param_2 * 4);
}

