/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462de4; original_symbol=native_cipher_key_init */

void native_cipher_key_init(undefined4 *param_1,uint param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  
  puVar1 = DAT_00462e58;
  FUN_00298d34(DAT_00462e58,0,0x10);
  uVar2 = param_2;
  if (0xf < (int)param_2) {
    uVar2 = 0x10;
  }
  FUN_00298c6c(puVar1,param_1,uVar2);
  native_cipher_key_schedule(DAT_00462e5c,(int)puVar1);
  if (8 < (int)param_2) {
    native_cipher_key_schedule(DAT_00462e60,(int)(puVar1 + 2));
  }
  *(bool *)DAT_00462e64 = 8 < (int)param_2;
  return;
}

