/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462c14; original_symbol=native_fs_close */

undefined4 native_fs_close(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  if (0x20 < param_1) {
    MMI_diag_stub_00298B64();
    return 2;
  }
  iVar1 = ecos_close(param_1,param_1,param_3,param_4);
  if (iVar1 < 0) {
    MMI_diag_stub_00298B64();
    return 2;
  }
  return 0;
}

