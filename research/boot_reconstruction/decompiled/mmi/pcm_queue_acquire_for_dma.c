/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002953cc; original_symbol=pcm_queue_acquire_for_dma */

int pcm_queue_acquire_for_dma(uint *param_1,uint param_2)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  int iVar2;
  
  FUN_0033a048(DAT_00295430,param_2);
  iVar1 = pcm_queue_take_core(param_1,param_2);
  FUN_0033a05c(DAT_00295430,extraout_r1);
  FUN_0033a048(DAT_00295430,extraout_r1_00);
  iVar2 = *DAT_00295434;
  FUN_0033a05c(DAT_00295430,extraout_r1_01);
  if (iVar2 != 0) {
    ecos_flag_setbits(DAT_00295438,1,extraout_r2,extraout_r3);
  }
  return iVar1;
}

