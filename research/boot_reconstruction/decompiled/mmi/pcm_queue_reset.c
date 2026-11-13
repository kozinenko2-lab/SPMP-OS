/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0029440c; original_symbol=pcm_queue_reset */

void pcm_queue_reset(undefined4 param_1,uint param_2)

{
  uint extraout_r1;
  
  FUN_0033a048(DAT_00294438,param_2);
  *DAT_0029443c = 1;
  FUN_0033a05c(DAT_00294438,extraout_r1);
  return;
}

