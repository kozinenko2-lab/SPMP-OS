/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033c0fc; original_symbol=ecos_scheduler_start */

void ecos_scheduler_start(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_r1;
  undefined4 extraout_r2;
  undefined4 extraout_r3;
  
  iVar1 = FUN_0033b8b4(DAT_0033c148,param_2,param_3);
  if (iVar1 == 0) {
    FUN_002815bc(DAT_0033c14c,extraout_r1,extraout_r2,extraout_r3);
  }
  *DAT_0033c150 = 0;
  *DAT_0033c154 = iVar1;
                    /* WARNING: Subroutine does not return */
  ecos_context_restore((undefined4 *)(iVar1 + 0xc));
}

