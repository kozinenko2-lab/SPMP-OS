/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0029477c; original_symbol=pcm_queue_commit_limit */

undefined4 pcm_queue_commit_limit(int param_1)

{
  undefined4 uVar1;
  
  if (-param_1 + 0x40000U < 0x40001) {
    *DAT_0029479c = (int)&DAT_00042000 + -param_1;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffffff;
  }
  return uVar1;
}

