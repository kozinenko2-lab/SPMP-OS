/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00463254; original_symbol=native_cipher_pack_bits */

void native_cipher_pack_bits(undefined4 *param_1,int param_2,int param_3)

{
  uint uVar1;
  
  uVar1 = 0;
  FUN_00298d34(param_1,0,param_3 >> 3);
  if (param_3 < 1) {
    return;
  }
  do {
    *(byte *)(((int)uVar1 >> 3) + (int)param_1) =
         *(byte *)(((int)uVar1 >> 3) + (int)param_1) | *(char *)(param_2 + uVar1) << (uVar1 & 7);
    uVar1 = uVar1 + 1;
  } while ((int)uVar1 < param_3);
  return;
}

