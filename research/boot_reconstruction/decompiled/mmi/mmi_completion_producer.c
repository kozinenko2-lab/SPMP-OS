/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d2c28; original_symbol=mmi_completion_producer */

undefined4 mmi_completion_producer(char param_1)

{
  short *psVar1;
  int iVar2;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r2_01;
  undefined4 uVar3;
  undefined4 extraout_r3;
  undefined4 *extraout_r3_00;
  undefined4 extraout_r3_01;
  undefined4 *puVar4;
  
  iVar2 = FUN_0029b718();
  ecos_flag_init(DAT_002d2d94);
  psVar1 = DAT_002d2d98;
  *DAT_002d2d98 = *(short *)(iVar2 + 0x10);
  psVar1[1] = 2;
  psVar1[2] = *psVar1 + 2;
  psVar1[3] = *(short *)(iVar2 + 0x12) + -2;
  psVar1[8] = *(short *)(iVar2 + 0x1c);
  psVar1[5] = *(short *)(iVar2 + 0x1e);
  iVar2 = FUN_002d106c((uint)(ushort)psVar1[8]);
  *(char *)((int)psVar1 + 0x13) = (char)iVar2;
  iVar2 = FUN_002d106c((uint)(ushort)psVar1[5]);
  *(char *)(psVar1 + 10) = (char)iVar2;
  iVar2 = FUN_002d106c(0x200);
  *(char *)((int)psVar1 + 0x15) = (char)iVar2;
  *(char *)(psVar1 + 9) =
       (char)((int)(uint)(ushort)psVar1[5] >> (uint)*(byte *)((int)psVar1 + 0x15));
  psVar1[7] = -1;
  psVar1[0xb] = (short)(0x10 << (((uint)*(byte *)((int)psVar1 + 0x13) + (uint)*(byte *)(psVar1 + 10)
                                 ) - (uint)*(byte *)((int)psVar1 + 0x15) & 0xff));
  psVar1[4] = 0x10;
  MMI_diag_stub_00298B64();
  FUN_002d25f8();
  MMI_diag_stub_00298B64();
  FUN_002d24f8();
  psVar1[0x4c] = 0;
  psVar1[0x4d] = 0;
  iVar2 = FUN_002d0b40((uint)(ushort)psVar1[3]);
  if (iVar2 == -1) {
    MMI_diag_stub_00298B64();
    ecos_flag_setbits(DAT_002d2da8,1,extraout_r2,extraout_r3);
    return 0xffffffff;
  }
  iVar2 = FUN_002d0d6c();
  if (iVar2 == -1) {
    MMI_diag_stub_00298B64();
    ecos_flag_setbits(DAT_002d2da8,1,extraout_r2_01,extraout_r3_01);
    return 0xffffffff;
  }
  uVar3 = extraout_r2_00;
  puVar4 = extraout_r3_00;
  if (param_1 != '\0') {
    FUN_002d1d28(iVar2,extraout_r1);
    puVar4 = DAT_002d2db0;
    uVar3 = 1;
    *DAT_002d2db0 = 1;
  }
  ecos_flag_setbits(DAT_002d2da8,1,uVar3,puVar4);
  return 0;
}

