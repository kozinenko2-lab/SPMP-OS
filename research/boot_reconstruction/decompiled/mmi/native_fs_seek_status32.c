/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462c64; original_symbol=native_fs_seek_status32 */

undefined4 native_fs_seek_status32(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  
  uVar1 = ecos_seek32(param_1,param_2,param_3,param_4);
  if ((int)uVar1 < 0) {
    MMI_diag_stub_00298B64();
    return 2;
  }
  return 0;
}

