/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004630b4; original_symbol=native_cipher_sbox */

void native_cipher_sbox(int param_1,byte *param_2)

{
  uint uVar1;
  
  uVar1 = 0;
  do {
    native_cipher_unpack_bits
              (param_1,((uint)param_2[2] * 4 + (uint)param_2[1] * 8 + (uint)param_2[3] * 2 +
                        (uint)param_2[4] & 0xff) +
                       (((uint)param_2[5] + (uint)*param_2 * 2 & 0xff) + uVar1 * 4) * 0x10 +
                       DAT_00463138,4);
    uVar1 = uVar1 + 1 & 0xff;
    param_2 = param_2 + 6;
    param_1 = param_1 + 4;
  } while (uVar1 < 8);
  return;
}

