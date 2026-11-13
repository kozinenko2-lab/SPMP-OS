/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004619ac; original_symbol=native_exit_control */

undefined4 native_exit_control(int param_1)

{
  uint *puVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  
  MMI_diag_stub_00298B64();
  if (*DAT_00461a44 == 0) {
    return 0;
  }
  FUN_004623bc((uint)*DAT_00461a48);
  FUN_00339b58(*DAT_00461a4c,extraout_r1);
  if (-1 < param_1) {
    FUN_00339b58(*DAT_00461a44,extraout_r1_00);
  }
  MMI_diag_stub_00298B64();
  FUN_004629cc();
  FUN_002d6d60(0xd07,(uint *)0x0,0);
  puVar1 = DAT_00461a44;
  if (-1 < param_1) {
    FUN_00339b94(*DAT_00461a44,extraout_r1_01);
    *puVar1 = 0;
  }
  return 0;
}

