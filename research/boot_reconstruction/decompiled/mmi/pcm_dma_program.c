/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a14d0; original_symbol=pcm_dma_program */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void pcm_dma_program(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  
  iVar1 = param_1 * 4;
  *(int *)(iVar1 + -0x6cfefff8) = param_2;
  *(int *)(iVar1 + -0x6cfeffe8) = param_2 + param_3 + -4;
  *(int *)(iVar1 + -0x6cfeffb4) = param_4;
  *(int *)(iVar1 + -0x6cfeffa4) = param_4 + param_5 + -4;
  *(undefined4 *)(&DAT_93010028 + iVar1) = 0x93012004;
  if ((_DAT_93012000 & 0x100000) == 0) {
    *(undefined4 *)(&DAT_9301006c + param_1 * 4) = 0xde;
  }
  else {
    *(undefined4 *)(&DAT_9301006c + iVar1) = 0xfe;
  }
  pcm_engine_enable();
  return;
}

