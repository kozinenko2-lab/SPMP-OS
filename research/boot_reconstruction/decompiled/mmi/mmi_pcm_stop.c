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
  FUN_00294ab0(0x1000,1);
  while ((iVar2 = FUN_00292fd8(), iVar2 != 4 && (iVar2 = FUN_00292fd8(), iVar2 != 0))) {
    if ((*(uint *)*DAT_002c5908 & 1) != 0) {
      FUN_00292fd8();
      MMI_diag_stub_00298B64();
    }
    iVar2 = FUN_00292fd8();
    if (iVar2 == 4) {
      FUN_0029307c(4,extraout_r1);
    }
    else {
      iVar2 = FUN_00292fd8();
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
  FUN_00294440(uVar3,uVar4);
  return;
}

