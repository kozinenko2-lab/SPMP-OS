/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462bb0; original_symbol=native_fs_write */

undefined4 native_fs_write(uint param_1,uint param_2,int param_3,int *param_4)

{
  int iVar1;
  
  if (0x20 < param_1) {
    MMI_diag_stub_00298B64();
    return 2;
  }
  iVar1 = ecos_write(param_1,param_2,param_3);
  *param_4 = iVar1;
  if (iVar1 == param_3) {
    return 0;
  }
  MMI_diag_stub_00298B64();
  return 2;
}

