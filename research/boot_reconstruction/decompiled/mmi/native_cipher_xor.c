/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00463194; original_symbol=native_cipher_xor */

void native_cipher_xor(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = 0;
  if (param_3 < 1) {
    return;
  }
  do {
    *(byte *)(param_1 + iVar1) = *(byte *)(param_1 + iVar1) ^ *(byte *)(param_2 + iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < param_3);
  return;
}

