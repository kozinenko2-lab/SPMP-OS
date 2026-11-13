/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00292198; original_symbol=pcm_stage_292198 */

void pcm_stage_292198(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = DAT_00292230;
  iVar2 = DAT_0029222c;
  iVar4 = 0;
  do {
    iVar1 = iVar4 * 0x14;
    iVar5 = iVar1 + iVar2;
    *(undefined4 *)(iVar5 + 4) = 1;
    *(undefined4 *)(iVar1 + iVar2) = 0;
    *(undefined4 *)(iVar5 + 8) = 0;
    *(int *)(iVar5 + 0x10) = iVar1 + iVar3;
    iVar1 = DAT_0029222c;
    iVar4 = iVar4 + 1;
  } while (iVar4 < 3);
  *(int *)(DAT_0029222c + iVar4 * 0x14 + -4) = DAT_0029222c;
  *DAT_00292234 = iVar1;
  *DAT_00292238 = iVar1;
  *DAT_0029223c = iVar1;
  *DAT_00292240 = iVar1;
  *DAT_00292244 = 0;
  *DAT_00292248 = *DAT_00292248 + 1;
  return;
}

