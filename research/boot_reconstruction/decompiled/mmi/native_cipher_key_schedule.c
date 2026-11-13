/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462fa8; original_symbol=native_cipher_key_schedule */

void native_cipher_key_schedule(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = 0;
  native_cipher_unpack_bits((int)DAT_00463034,param_2,0x40);
  native_cipher_permutation(DAT_00463034,(int)DAT_00463034,DAT_00463038,0x38);
  do {
    bVar1 = *(byte *)(DAT_0046303c + iVar2);
    native_cipher_rotate((undefined4 *)*DAT_00463040,0x1c,(uint)bVar1);
    native_cipher_rotate((undefined4 *)*DAT_00463044,0x1c,(uint)bVar1);
    native_cipher_permutation
              ((undefined4 *)(param_1 + iVar2 * 0x30),(int)DAT_00463034,DAT_00463048,0x30);
    iVar2 = iVar2 + 1;
  } while (iVar2 < 0x10);
  return;
}

