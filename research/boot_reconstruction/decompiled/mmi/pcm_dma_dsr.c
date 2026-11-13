/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00291238; original_symbol=pcm_dma_dsr */

void pcm_dma_dsr(void)

{
  int *piVar1;
  int *piVar2;
  int *extraout_r2;
  
  piVar1 = DAT_002912e4;
  if ((*DAT_002912e4 == 4) && (piVar2 = *(int **)*DAT_002912e8, piVar2 == (int *)*DAT_002912ec)) {
    *DAT_002912ec = 0;
    *piVar1 = 8;
  }
  else {
    piVar2 = DAT_002912e4;
    if (*DAT_002912e4 == 8) {
      *DAT_002912e4 = 0x10;
      ecos_flag_setbits(DAT_002912f0,1,piVar1,0x10);
      piVar2 = extraout_r2;
    }
  }
  if (*DAT_002912f4 == 2) {
    ecos_flag_setbits(DAT_002912f8,1,piVar2,2);
  }
  else if (*DAT_002912f4 == 0x100) {
    return;
  }
  *DAT_002912fc = 0;
  return;
}

