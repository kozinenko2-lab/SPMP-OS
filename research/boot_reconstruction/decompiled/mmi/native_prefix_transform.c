/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462cd0; original_symbol=native_prefix_transform */

undefined4
native_prefix_transform
          (undefined4 *param_1,int param_2,int param_3,undefined4 *param_4,uint param_5,byte param_6
          )

{
  int iVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  
  if (((param_2 == 0 || param_1 == (undefined4 *)0x0) || (param_4 == (undefined4 *)0x0)) ||
     ((param_3 + 7U & 0xfffffff8) == 0)) {
    return 0;
  }
  native_cipher_key_init(param_4,param_5);
  iVar3 = DAT_00462de0;
  iVar1 = (int)(param_3 + 7U) >> 3;
  if (*DAT_00462ddc == '\0') {
    iVar3 = 0;
    if (0 < iVar1) {
      do {
        native_cipher_block(param_1,param_2,DAT_00462de0,param_6);
        iVar3 = iVar3 + 1;
        param_1 = param_1 + 2;
        param_2 = param_2 + 8;
      } while (iVar3 < iVar1);
    }
  }
  else {
    iVar4 = 0;
    if (0 < iVar1) {
      do {
        native_cipher_block(param_1,param_2,iVar3,param_6);
        cVar2 = '\x01' - param_6;
        if (1 < param_6) {
          cVar2 = '\0';
        }
        native_cipher_block(param_1,(int)param_1,iVar3 + 0x300,cVar2);
        native_cipher_block(param_1,(int)param_1,iVar3,param_6);
        iVar4 = iVar4 + 1;
        param_1 = param_1 + 2;
        param_2 = param_2 + 8;
      } while (iVar4 < iVar1);
    }
  }
  return 1;
}

