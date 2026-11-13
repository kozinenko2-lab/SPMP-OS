/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004473bc; original_symbol=mmi_background */

void mmi_background(void)

{
  int *piVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar3;
  undefined8 uVar4;
  
  FUN_00438394(0);
  FUN_0029e830();
  FUN_00447368(0);
  FUN_00447388(0);
  FUN_004472c4();
  do {
    iVar2 = FUN_002cdacc();
    piVar1 = DAT_00447454;
    if (iVar2 == 0) {
      uVar4 = FUN_002cdb14(0,extraout_r1);
    }
    else {
      uVar3 = extraout_r1;
      if ((*DAT_00447454 == 0) &&
         (iVar2 = FUN_002d885c(iVar2,extraout_r1), uVar3 = extraout_r1_00, iVar2 == 0)) {
        iVar2 = FUN_002d8784();
        *piVar1 = 1;
        uVar3 = extraout_r1_01;
      }
      else {
        *DAT_00447454 = 1;
      }
      uVar4 = FUN_002cc3e4(iVar2,uVar3);
      uVar4 = FUN_002cc48c((int)uVar4,(uint)((ulonglong)uVar4 >> 0x20));
    }
    FUN_002cdc88((int)uVar4,(uint)((ulonglong)uVar4 >> 0x20));
    ecos_thread_delay_wrapper(100,0);
  } while( true );
}

