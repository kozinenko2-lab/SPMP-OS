/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294f30; original_symbol=pcm_queue_commit */

void pcm_queue_commit(uint param_1,int param_2)

{
  uint extraout_r1;
  undefined4 extraout_r2;
  
  if (param_2 == 1) {
    param_1 = param_1 | 0x10000000;
  }
  else if (param_2 == 2) {
    param_1 = param_1 | 0x20000000;
  }
  else if (param_2 == 3) {
    param_1 = param_1 | 0x40000000;
  }
  pcm_queue_commit_limit(0x31800);
  pcm_queue_publish_core(param_1,extraout_r1,extraout_r2);
  return;
}

