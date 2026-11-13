/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a12b8; original_symbol=pcm_engine_enable */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void pcm_engine_enable(void)

{
  _DAT_93012000 = _DAT_93012000 & 0xffffdfff | 1;
  return;
}

