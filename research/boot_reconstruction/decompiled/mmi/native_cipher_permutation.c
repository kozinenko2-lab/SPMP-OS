/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0046313c; original_symbol=native_cipher_permutation */

void native_cipher_permutation(undefined4 *param_1,int param_2,int param_3,uint param_4)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = DAT_00463190;
  iVar2 = 0;
  if (0 < (int)param_4) {
    do {
      *(undefined1 *)((int)puVar1 + iVar2) =
           *(undefined1 *)((uint)*(byte *)(param_3 + iVar2) + param_2 + -1);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)param_4);
  }
  FUN_00298c6c(param_1,DAT_00463190,param_4);
  return;
}

