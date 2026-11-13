/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00464da8; original_symbol=native_midi_read_samples */

int native_midi_read_samples(int param_1,int param_2)

{
  short *psVar1;
  int iVar2;
  uint uVar3;
  undefined4 extraout_r1;
  undefined4 extraout_r1_00;
  undefined4 uVar4;
  undefined2 *extraout_r2;
  undefined2 *extraout_r2_00;
  undefined2 *puVar5;
  uint uVar6;
  int iVar7;
  int local_24;
  
  iVar2 = DAT_00464d8c;
  psVar1 = DAT_00464d64;
  iVar7 = DAT_00464d8c + -0x200;
  local_24 = 0;
  while ((local_24 < param_2 && (0 < *psVar1))) {
    if (psVar1[2] == 0) {
      FUN_00465874();
      uVar6 = 0;
      uVar3 = 0;
      uVar4 = extraout_r1;
      puVar5 = extraout_r2;
      if (psVar1[1] != 0) {
        do {
          if (*(short *)(iVar7 + uVar6 * 2) != 0) {
            if (*(int *)(iVar2 + uVar6 * 4) == 0) {
              FUN_00465210((short)*(undefined4 *)(psVar1 + 0xc));
              FUN_004647e8(uVar6 & 0xffff);
              psVar1[0xc] = 0;
              psVar1[0xd] = 0;
              uVar4 = extraout_r1_00;
              puVar5 = extraout_r2_00;
            }
            *(int *)(iVar2 + uVar6 * 4) = *(int *)(iVar2 + uVar6 * 4) + -1;
          }
          uVar3 = (uint)psVar1[1];
          uVar6 = uVar6 + 1;
        } while (uVar6 < uVar3);
      }
      FUN_004658ec(uVar3,uVar4,puVar5);
      psVar1[2] = (short)*(undefined4 *)(psVar1 + 6);
      *(int *)(psVar1 + 0xc) = *(int *)(psVar1 + 0xc) + 1;
    }
    else {
      psVar1[2] = psVar1[2] + -1;
    }
    FUN_00465220((undefined2 *)(param_1 + local_24),&local_24);
  }
  return local_24;
}

