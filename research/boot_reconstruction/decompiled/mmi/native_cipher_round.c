/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046304c; original_symbol=native_cipher_round */

void native_cipher_round(undefined4 *param_1,int param_2)

{
  native_cipher_permutation((undefined4 *)DAT_004630a8,(int)param_1,DAT_004630ac,0x30);
  native_cipher_xor((int)DAT_004630a8,param_2,0x30);
  native_cipher_sbox((int)param_1,DAT_004630a8);
  native_cipher_permutation(param_1,(int)param_1,DAT_004630b0,0x20);
  return;
}

