/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a6b8c; original_symbol=ecos_readdir_r */

undefined4 ecos_readdir_r(uint param_1,uint param_2,uint *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 extraout_r2;
  undefined8 uVar3;

  *param_3 = 0;
  if (param_1 == 0) {
    return 9;
  }
  uVar3 = ecos_read(param_1,param_2,0x104);
  iVar1 = (int)uVar3;
  if (iVar1 < 0) {
    puVar2 = (undefined4 *)ecos_errno_location(iVar1,(uint)((ulonglong)uVar3 >> 0x20),extraout_r2);
    return *puVar2;
  }
  if (0 < iVar1) {
    *param_3 = param_2;
  }
  return 0;
}
