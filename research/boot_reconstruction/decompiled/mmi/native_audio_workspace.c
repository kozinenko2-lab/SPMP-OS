/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461e14; original_symbol=native_audio_workspace */

int native_audio_workspace(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0xf00000;
  iVar2 = FUN_0045ffbc();
  piVar1 = DAT_00461e74;
  if ((iVar2 == 5) && (*DAT_00461e74 == 0)) {
    iVar3 = mmi_game_pool_alloc(0x132000);
    *piVar1 = iVar3;
    FUN_003b9624();
    MMI_diag_stub_00298B64();
  }
  return iVar3;
}

