/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002c5850; original_symbol=mmi_pcm_stop */

void mmi_pcm_stop(void)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint uVar4;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  
  *DAT_002c5904 = 0;
  pcm_queue_reserve(0x1000,1);
  while ((iVar2 = pcm_driver_get_state(), iVar2 != 4 && (iVar2 = pcm_driver_get_state(), iVar2 != 0)
         )) {
    if ((*(uint *)*DAT_002c5908 & 1) != 0) {
      pcm_driver_get_state();
      MMI_diag_stub_00298B64();
    }
    iVar2 = pcm_driver_get_state();
    if (iVar2 == 4) {
      pcm_driver_control(4,extraout_r1);
    }
    else {
      iVar2 = pcm_driver_get_state();
      if (iVar2 == 1) {
        FUN_00293308();
      }
    }
  }
  puVar1 = DAT_002c5910;
  MMI_diag_stub_00298B64();
  uVar3 = *puVar1;
  uVar4 = extraout_r1_00;
  if (-1 < (int)uVar3) {
    uVar3 = ecos_close(uVar3,extraout_r1_00,extraout_r2,extraout_r3);
    *puVar1 = 0xffffffff;
    uVar4 = extraout_r1_01;
  }
  pcm_queue_shutdown(uVar3,uVar4);
  return;
}

