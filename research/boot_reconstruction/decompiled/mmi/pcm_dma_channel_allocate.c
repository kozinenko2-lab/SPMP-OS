/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a2e88; original_symbol=pcm_dma_channel_allocate */

uint pcm_dma_channel_allocate(undefined4 param_1,uint param_2)

{
  uint *puVar1;
  uint uVar2;
  
  FUN_0033a048(DAT_002a2f0c,param_2);
  puVar1 = DAT_002a2f10;
  uVar2 = *DAT_002a2f10;
  while (uVar2 == 0) {
    FUN_0033a0c0(DAT_002a2f14);
    uVar2 = *puVar1;
  }
  uVar2 = 0;
  do {
    if ((*DAT_002a2f10 & 1 << (uVar2 & 0xff)) != 0) break;
    uVar2 = uVar2 + 1;
  } while (uVar2 < 2);
  *DAT_002a2f10 = *DAT_002a2f10 & ~(1 << (uVar2 & 0xff));
  FUN_0033a05c(DAT_002a2f0c,1);
  return uVar2;
}

