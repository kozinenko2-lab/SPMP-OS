/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462b48; original_symbol=native_fs_read */

longlong native_fs_read(uint param_1,uint param_2,uint param_3,uint *param_4)

{
  uint uVar1;
  undefined4 extraout_r1;
  
  if (param_1 == 0) {
    return CONCAT44(param_2,2);
  }
  MMI_diag_stub_00298B64();
  uVar1 = ecos_read(param_1,param_2,param_3);
  *param_4 = uVar1;
  if (uVar1 == param_3) {
    return (ulonglong)uVar1 << 0x20;
  }
  MMI_diag_stub_00298B64();
  return CONCAT44(extraout_r1,2);
}

