/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004631c8; original_symbol=native_cipher_rotate */

void native_cipher_rotate(undefined4 *param_1,int param_2,uint param_3)

{
  undefined4 *puVar1;
  
  puVar1 = DAT_00463218;
  FUN_00298c6c(DAT_00463218,param_1,param_3);
  FUN_00298c6c(param_1,(undefined4 *)((int)param_1 + param_3),param_2 - param_3);
  FUN_00298c6c((undefined4 *)((int)param_1 + (param_2 - param_3)),puVar1,param_3);
  return;
}

