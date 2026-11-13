/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00292058; original_symbol=pcm_stage_292058 */

void pcm_stage_292058(int param_1)

{
  if (0 < param_1) {
    *DAT_00292094 = param_1;
    return;
  }
  if ((*(uint *)*DAT_00292098 & 0x10) == 0) {
    return;
  }
  MMI_diag_stub_00298B64();
  return;
}

