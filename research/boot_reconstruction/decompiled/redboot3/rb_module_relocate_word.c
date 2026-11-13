/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000413d0; original_symbol=rb_module_relocate_word */

undefined4 rb_module_relocate_word(undefined4 param_1,uint *param_2,int param_3,undefined4 param_4)

{
  switch(param_1) {
  case 1:
  case 0x1c:
  case 0x1d:
    *param_2 = *param_2 & 0xff000000 |
               ((param_3 - (int)param_2) + (*param_2 & 0xffffff) * 4 & 0x3ffffff) >> 2;
    return 0;
  case 2:
    *param_2 = *param_2 + param_3;
    return 0;
  default:
    FUN_0004563c(DAT_000414bc,(uint)param_2,param_3,param_4);
    return 0xffffffff;
  }
}

