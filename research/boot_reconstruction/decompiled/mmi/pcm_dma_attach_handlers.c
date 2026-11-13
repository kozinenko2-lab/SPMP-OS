/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a2f64; original_symbol=pcm_dma_attach_handlers */

void pcm_dma_attach_handlers(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r3;
  
  iVar1 = DAT_002a2fc8;
  FUN_00339c10(param_1 + 0x18U,2,0,param_2,param_3,(undefined4 *)(param_1 * 4 + DAT_002a2fc8),
               (undefined4 *)(DAT_002a2fcc + param_1 * 0x1c));
  FUN_00339c74(*(uint **)(param_1 * 4 + iVar1),extraout_r1,extraout_r2);
  FUN_00339cd8(param_1 + 0x18U,extraout_r1_00,extraout_r2_00,extraout_r3);
  return;
}

