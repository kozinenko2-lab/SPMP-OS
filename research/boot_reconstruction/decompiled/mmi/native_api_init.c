/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d32b0; original_symbol=native_api_init */

void native_api_init(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined8 uVar4;
  
  uVar2 = DAT_002d3544;
  puVar1 = DAT_002d3540;
  iVar3 = 0;
  do {
    puVar1[iVar3] = uVar2;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 0x55);
  uVar4 = ecos_current_ticks();
  *DAT_002d3548 = (int)uVar4;
  *puVar1 = DAT_002d354c;
  puVar1[1] = DAT_002d3550;
  puVar1[2] = DAT_002d3554;
  puVar1[3] = DAT_002d3558;
  puVar1[4] = DAT_002d355c;
  puVar1[5] = DAT_002d3560;
  puVar1[6] = DAT_002d3564;
  puVar1[7] = DAT_002d3568;
  puVar1[8] = DAT_002d356c;
  puVar1[9] = DAT_002d3570;
  puVar1[10] = DAT_002d3574;
  puVar1[0xb] = DAT_002d3578;
  puVar1[0xc] = DAT_002d357c;
  puVar1[0xd] = DAT_002d3580;
  puVar1[0xe] = DAT_002d3584;
  puVar1[0xf] = DAT_002d3588;
  puVar1[0x10] = DAT_002d358c;
  puVar1[0x11] = DAT_002d3590;
  puVar1[0x12] = DAT_002d3594;
  puVar1[0x13] = DAT_002d3598;
  puVar1[0x14] = DAT_002d359c;
  puVar1[0x15] = DAT_002d35a0;
  puVar1[0x16] = DAT_002d35a4;
  puVar1[0x17] = DAT_002d35a8;
  puVar1[0x18] = DAT_002d35ac;
  puVar1[0x19] = DAT_002d35b0;
  puVar1[0x1a] = DAT_002d35b4;
  puVar1[0x1b] = DAT_002d35b8;
  puVar1[0x1c] = DAT_002d35bc;
  puVar1[0x1d] = DAT_002d35c0;
  puVar1[0x1e] = DAT_002d35c4;
  puVar1[0x1f] = DAT_002d35c8;
  puVar1[0x20] = DAT_002d35cc;
  puVar1[0x21] = DAT_002d35d0;
  puVar1[0x22] = DAT_002d35d4;
  puVar1[0x23] = DAT_002d35d8;
  puVar1[0x24] = DAT_002d35dc;
  puVar1[0x25] = DAT_002d35e0;
  puVar1[0x26] = DAT_002d35e4;
  puVar1[0x27] = DAT_002d35e8;
  puVar1[0x28] = DAT_002d35ec;
  puVar1[0x29] = DAT_002d35f0;
  puVar1[0x2a] = DAT_002d35f4;
  puVar1[0x2b] = DAT_002d35f8;
  puVar1[0x2c] = DAT_002d35fc;
  puVar1[0x2d] = DAT_002d3600;
  puVar1[0x2e] = DAT_002d3604;
  puVar1[0x2f] = DAT_002d3608;
  puVar1[0x30] = DAT_002d360c;
  puVar1[0x31] = DAT_002d3610;
  puVar1[0x32] = DAT_002d3614;
  puVar1[0x33] = DAT_002d3618;
  puVar1[0x34] = DAT_002d361c;
  puVar1[0x35] = DAT_002d3620;
  puVar1[0x36] = DAT_002d3624;
  puVar1[0x37] = DAT_002d3628;
  puVar1[0x38] = DAT_002d362c;
  puVar1[0x39] = DAT_002d3630;
  puVar1[0x3a] = DAT_002d3634;
  puVar1[0x3b] = DAT_002d3638;
  puVar1[0x3c] = DAT_002d363c;
  puVar1[0x54] = 0;
  puVar1[0x40] = DAT_002d3640;
  puVar1[0x47] = DAT_002d3644;
  puVar1[0x49] = DAT_002d3648;
  puVar1[0x4c] = DAT_002d364c;
  puVar1[0x4d] = DAT_002d3650;
  puVar1[0x4e] = DAT_002d3654;
  puVar1[0x4f] = DAT_002d3658;
  puVar1[0x50] = DAT_002d365c;
  puVar1[0x51] = DAT_002d3660;
  puVar1[0x52] = DAT_002d3664;
  puVar1[0x53] = DAT_002d3668;
  puVar1[0x42] = DAT_002d366c;
  return;
}

