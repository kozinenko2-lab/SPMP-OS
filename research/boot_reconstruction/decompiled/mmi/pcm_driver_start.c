/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00292d44; original_symbol=pcm_driver_start */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 pcm_driver_start(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar5;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  int local_1c;
  
  local_1c = 0;
  iVar3 = FUN_002925e8(DAT_00292f98,&local_1c);
  if (iVar3 == 0) {
    if (local_1c == 0) {
      MMI_diag_stub_00298B64();
      FUN_002926bc(DAT_00292f98,1);
      _DAT_93010004 = 1 << (*DAT_00292fa8 & 0xff);
      *(undefined4 *)(&DAT_9301006c + *DAT_00292fa8 * 4) = 0;
      if ((*(uint *)*DAT_00292fac & 1) != 0) {
        MMI_diag_stub_00298B64();
      }
      iVar3 = 0;
      do {
        pcm_stage_292bec();
        iVar3 = iVar3 + 1;
      } while (iVar3 < 3);
      uVar5 = extraout_r1;
      if ((*(uint *)*DAT_00292fac & 1) != 0) {
        FUN_002952d0(1);
        MMI_diag_stub_00298B64();
        uVar5 = extraout_r1_00;
      }
      FUN_0033a048(DAT_00292fb8,uVar5);
      iVar3 = *DAT_00292fbc;
      *DAT_00292fc0 = iVar3;
      *DAT_00292fc4 = *(int *)(iVar3 + 0x10);
      if ((*(uint *)*DAT_00292fac & 0x10) != 0) {
        MMI_diag_stub_00298B64();
      }
      piVar2 = DAT_00292fc4;
      piVar1 = DAT_00292fbc;
      pcm_dma_program(*DAT_00292fa8,*(int *)*DAT_00292fbc,((int *)*DAT_00292fbc)[2],
                      *(int *)*DAT_00292fc4,((int *)*DAT_00292fc4)[2]);
      *(uint *)(*piVar1 + 4) = *(uint *)(*piVar1 + 4) & 0xfffffffd | 4;
      *(uint *)(*piVar2 + 4) = *(uint *)(*piVar2 + 4) & 0xfffffffd | 4;
      *piVar1 = *(int *)(*piVar1 + 0x10);
      *DAT_00292fa4 = 2;
      FUN_0033a05c(DAT_00292fb8,extraout_r1_01);
    }
    else {
      MMI_diag_stub_00298B64();
      iVar3 = 0;
      do {
        pcm_stage_292bec();
        iVar3 = iVar3 + 1;
      } while (iVar3 < 3);
      MMI_diag_stub_00298B64();
      piVar1 = DAT_00292fbc;
      iVar3 = *DAT_00292fbc;
      *DAT_00292fc0 = iVar3;
      piVar2 = DAT_00292fc4;
      *DAT_00292fc4 = *(int *)(iVar3 + 0x10);
      *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) & 0xfffffffd | 4;
      *(uint *)(*piVar2 + 4) = *(uint *)(*piVar2 + 4) & 0xfffffffd | 4;
      *piVar1 = *(int *)(*piVar1 + 0x10);
      FUN_00290b58();
      uVar4 = FUN_00292654(DAT_00292f98,extraout_r1_02);
      pcm_driver_control(uVar4,extraout_r1_03);
    }
    uVar4 = 1;
  }
  else {
    uVar4 = 0x10000000;
  }
  return uVar4;
}

