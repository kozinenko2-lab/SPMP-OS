/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046321c; original_symbol=native_cipher_unpack_bits */

void native_cipher_unpack_bits(int param_1,int param_2,int param_3)

{
  uint uVar1;
  
  uVar1 = 0;
  if (param_3 < 1) {
    return;
  }
  do {
    *(byte *)(param_1 + uVar1) =
         (byte)((int)(uint)*(byte *)(param_2 + ((int)uVar1 >> 3)) >> (uVar1 & 7)) & 1;
    uVar1 = uVar1 + 1;
  } while ((int)uVar1 < param_3);
  return;
}

