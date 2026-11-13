/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462e68; original_symbol=native_cipher_block */

void native_cipher_block(undefined4 *param_1,int param_2,int param_3,char param_4)

{
  undefined4 *puVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = 0;
  native_cipher_unpack_bits((int)DAT_00462f90,param_2,0x40);
  native_cipher_permutation(DAT_00462f90,(int)DAT_00462f90,DAT_00462f94,0x40);
  piVar3 = DAT_00462fa0;
  piVar2 = DAT_00462f9c;
  puVar1 = DAT_00462f98;
  if (param_4 == '\0') {
    do {
      FUN_00298c6c(puVar1,(undefined4 *)*piVar3,0x20);
      native_cipher_round((undefined4 *)*piVar3,param_3 + iVar4 * 0x30);
      piVar2 = DAT_00462f9c;
      native_cipher_xor(*piVar3,*DAT_00462f9c,0x20);
      FUN_00298c6c((undefined4 *)*piVar2,puVar1,0x20);
      iVar4 = iVar4 + 1;
    } while (iVar4 < 0x10);
  }
  else {
    iVar4 = 0xf;
    do {
      FUN_00298c6c(puVar1,(undefined4 *)*piVar2,0x20);
      native_cipher_round((undefined4 *)*piVar2,param_3 + iVar4 * 0x30);
      piVar3 = DAT_00462fa0;
      native_cipher_xor(*piVar2,*DAT_00462fa0,0x20);
      FUN_00298c6c((undefined4 *)*piVar3,puVar1,0x20);
      iVar4 = iVar4 + -1;
    } while (-1 < iVar4);
  }
  native_cipher_permutation(DAT_00462f90,(int)DAT_00462f90,DAT_00462fa4,0x40);
  native_cipher_pack_bits(param_1,(int)DAT_00462f90,0x40);
  return;
}

