/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294ab0; original_symbol=pcm_queue_reserve */

int pcm_queue_reserve(int param_1,uint param_2)

{
  uint uVar1;
  char *pcVar2;
  undefined4 *puVar3;
  int iVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  uint extraout_r1_03;
  uint extraout_r1_04;
  int iVar5;
  longlong lVar6;
  
  iVar5 = 0;
  if (param_2 == 0) {
    iVar4 = pcm_queue_reserve_try(param_1);
  }
  else {
    FUN_0033a048(DAT_00294ba8,param_2);
    iVar4 = pcm_queue_reserve_try(param_1);
    FUN_0033a05c(DAT_00294ba8,extraout_r1);
    *DAT_00294bac = 1;
    puVar3 = DAT_00294bb0;
    pcVar2 = DAT_00294ba8;
    uVar1 = extraout_r1_00;
    do {
      if (1 < iVar4 + 1U) break;
      FUN_0033a048(pcVar2,uVar1);
      *puVar3 = 1;
      FUN_0033a05c(pcVar2,extraout_r1_01);
      lVar6 = ecos_current_ticks();
      FUN_00339f98(DAT_00294bb4,1,1,(uint)(lVar6 + 100),(uint)((ulonglong)(lVar6 + 100) >> 0x20));
      FUN_0033a048(pcVar2,extraout_r1_02);
      *puVar3 = 0;
      iVar4 = pcm_queue_reserve_try(param_1);
      FUN_0033a05c(pcVar2,extraout_r1_03);
      iVar5 = iVar5 + 1;
      uVar1 = extraout_r1_04;
    } while (iVar5 < 6);
    *DAT_00294bac = 0;
  }
  return iVar4;
}

