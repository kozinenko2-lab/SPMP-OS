/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294878; original_symbol=pcm_set_chunk_size */

undefined4 pcm_set_chunk_size(int param_1)

{
  if (param_1 < 1) {
    MMI_diag_stub_00298B64();
    return 0xffffffff;
  }
  *DAT_002948f4 =
       *DAT_002948f0 +
       ((int)((longlong)DAT_002948ec * (longlong)param_1 >> 0x24) - (param_1 >> 0x1f)) * -300;
  MMI_diag_stub_00298B64();
  return 0;
}

