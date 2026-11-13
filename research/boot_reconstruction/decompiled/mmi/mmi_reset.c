/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00280040; original_symbol=mmi_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void processEntry mmi_reset(void)

{
  undefined4 *puVar1;
  uint uVar2;
  undefined4 *puVar4;
  undefined4 *puVar3;
  
  uVar2 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar2 & 0xffffdfff);
  _DAT_00000004 = *(undefined4 *)(DAT_002804ec + 4);
  _DAT_00000024 = *(undefined4 *)(DAT_002804ec + 0x24);
  uRam00000008 = *(undefined4 *)(DAT_002804ec + 8);
  do {
  } while (*DAT_002804c0 != 0);
  *DAT_002804c0 = DAT_00280160;
  puVar1 = DAT_0028049c;
  _DAT_00000028 = *(undefined4 *)(DAT_002804bc + 0x28);
  uRam00000018 = *(undefined4 *)(DAT_002804bc + 0x18);
  uRam00000038 = *(undefined4 *)(DAT_002804bc + 0x38);
  _DAT_0000001c = *(undefined4 *)(DAT_002804bc + 0x1c);
  _DAT_0000003c = *(undefined4 *)(DAT_002804bc + 0x3c);
  _DAT_0000000c = *(undefined4 *)(DAT_002804bc + 0xc);
  uRam0000002c = *(undefined4 *)(DAT_002804bc + 0x2c);
  _DAT_00000010 = *(undefined4 *)(DAT_002804bc + 0x10);
  _DAT_00000030 = *(undefined4 *)(DAT_002804bc + 0x30);
  puVar3 = DAT_00280498;
  if (DAT_00280498 != DAT_0028049c) {
    do {
      puVar4 = puVar3 + 1;
      *puVar3 = 0;
      puVar3 = puVar4;
    } while (puVar4 <= puVar1);
  }
  FUN_00299c90();
  FUN_002976d0();
  mmi_run_constructors();
  mmi_bootstrap();
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

