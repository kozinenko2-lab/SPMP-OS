/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e65e4; original_symbol=blitter_control */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void blitter_control(int param_1)

{
  undefined2 *puVar1;
  uint *puVar2;
  
  FUN_0029e36c();
  puVar2 = DAT_002e66bc;
  if ((*DAT_002e66ac == 0) || ((_DAT_92007000 & 0x100) == 0)) {
    MMI_diag_stub_00298B64();
    FUN_002e59ec();
    puVar1 = DAT_002e66b4;
    *DAT_002e66b4 = 0;
    ecos_flag_setbits(DAT_002e66b8,1,0,puVar1);
    return;
  }
  *DAT_002e66bc = (uint)(param_1 != 0);
  _DAT_92007000 = 0x100;
  FUN_0029e450();
  _DAT_92007000 = _DAT_92007000 | 1;
  if (*puVar2 == 0) {
    return;
  }
  FUN_002e5b7c();
  return;
}

