/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004611bc; original_symbol=native_parse_ngame_header */

undefined4
native_parse_ngame_header
          (undefined4 *param_1,int param_2,int *param_3,undefined4 *param_4,int *param_5)

{
  int iVar1;
  undefined4 uVar2;
  uint uStack_38;
  undefined1 local_33;
  
  FUN_00298c6c(&uStack_38,param_1,5);
  local_33 = 0;
  iVar1 = FUN_00345f54(&uStack_38,DAT_00461260);
  if (iVar1 == 0) {
    MMI_diag_stub_00298B64();
    *param_4 = 0x80;
    iVar1 = param_1[0x1e];
    *param_5 = iVar1;
    *param_3 = (param_2 - 0x80U) - iVar1;
    FUN_00298c6c(param_1,param_1 + 0x20,param_2 - 0x80U);
    uVar2 = 0;
  }
  else {
    *param_3 = param_2;
    *param_5 = 0;
    *param_4 = 0;
    uVar2 = 6;
  }
  return uVar2;
}

