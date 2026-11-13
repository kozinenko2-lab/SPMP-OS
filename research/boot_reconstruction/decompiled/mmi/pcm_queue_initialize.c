/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002948fc; original_symbol=pcm_queue_initialize */

undefined4 pcm_queue_initialize(void)

{
  undefined4 uVar1;
  
  *DAT_002949a8 = 0;
  FUN_0033a020(DAT_002949ac);
  ecos_flag_init(DAT_002949b0);
  *DAT_002949b4 = 0;
  uVar1 = *DAT_002949b8;
  *DAT_002949bc = uVar1;
  *DAT_002949c0 = 0;
  *DAT_002949c4 = 0;
  *DAT_002949c8 = 0;
  *DAT_002949cc = uVar1;
  *DAT_002949d4 = *DAT_002949d0;
  *DAT_002949d8 = 0;
  *DAT_002949dc = 0x400000;
  *DAT_002949e0 = 0;
  *DAT_002949e4 = 0x400000;
  *DAT_002949ec = DAT_002949e8;
  return 0;
}

