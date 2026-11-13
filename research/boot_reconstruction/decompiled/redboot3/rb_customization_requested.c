/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00033ef4; original_symbol=rb_customization_requested */

uint rb_customization_requested
               (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = DAT_00033f18;
  FUN_0004563c(DAT_00033f1c,*(uint *)(DAT_00033f18 + 0x18),param_3,param_4);
  return *(uint *)(iVar1 + 0x18) >> 3 & 1;
}

