/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004474d8; original_symbol=mmi_system_init_retry */

void mmi_system_init_retry(void)

{
  int iVar1;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 uVar2;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  undefined4 uVar3;
  int iVar4;
  
  iVar4 = 0;
  MMI_diag_stub_00298B64();
  do {
    iVar1 = FUN_00438394(2);
    uVar2 = extraout_r2;
    uVar3 = extraout_r3;
    if (iVar1 == 0) break;
    MMI_diag_stub_00298B64();
    iVar4 = iVar4 + 1;
    ecos_thread_delay_wrapper(10,0);
    uVar2 = extraout_r2_00;
    uVar3 = extraout_r3_00;
  } while (iVar4 < 10);
  ecos_flag_setbits(DAT_0044753c,1,uVar2,uVar3);
  return;
}

