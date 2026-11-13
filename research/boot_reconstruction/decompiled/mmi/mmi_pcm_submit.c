/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002c567c; original_symbol=mmi_pcm_submit */

void mmi_pcm_submit(undefined4 *param_1,uint param_2)

{
  uint *puVar1;
  undefined4 *puVar2;
  uint extraout_r1;
  uint uVar3;
  int iVar4;
  
  if (*DAT_002c583c == 0) {
    if ((*(uint *)*DAT_002c5840 & 1) != 0) {
      MMI_diag_stub_00298B64();
    }
    if (*DAT_002c5848 == 1) {
      puVar2 = (undefined4 *)pcm_queue_reserve(param_2 << 1,0);
    }
    else {
      puVar2 = (undefined4 *)pcm_queue_reserve(param_2,0);
    }
    if (0 < (int)puVar2) {
      iVar4 = 0;
      if (*DAT_002c5848 == 1) {
        do {
          *(undefined2 *)(puVar2 + iVar4) = *(undefined2 *)(iVar4 * 2 + (int)param_1);
          *(undefined2 *)((int)puVar2 + iVar4 * 4 + 2) = *(undefined2 *)(iVar4 * 2 + (int)param_1);
          iVar4 = iVar4 + 1;
        } while (iVar4 * 2 < (int)param_2);
        *DAT_002c584c = *DAT_002c584c + param_2 * 2;
      }
      else {
        *DAT_002c584c = param_2 + *DAT_002c584c;
        FUN_00298c6c(puVar2,param_1,param_2);
      }
    }
    puVar1 = DAT_002c584c;
    pcm_queue_commit(*DAT_002c584c,0);
    *puVar1 = 0;
    pcm_driver_start();
    *DAT_002c583c = 1;
    return;
  }
  if (*DAT_002c5848 == 1) {
    uVar3 = param_2 << 1;
  }
  else {
    uVar3 = param_2;
  }
  puVar2 = (undefined4 *)pcm_queue_reserve(uVar3,1);
  if ((int)puVar2 < 1) {
    return;
  }
  iVar4 = 0;
  if (*DAT_002c5848 == 1) {
    do {
      *(undefined2 *)(puVar2 + iVar4) = *(undefined2 *)(iVar4 * 2 + (int)param_1);
      *(undefined2 *)((int)puVar2 + iVar4 * 4 + 2) = *(undefined2 *)(iVar4 * 2 + (int)param_1);
      iVar4 = iVar4 + 1;
    } while (iVar4 * 2 < (int)param_2);
  }
  else {
    FUN_00298c6c(puVar2,param_1,uVar3);
  }
  if ((int)param_2 < 1) {
    pcm_queue_commit(uVar3,0);
    return;
  }
  pcm_queue_commit(uVar3,0);
  iVar4 = pcm_driver_get_state();
  if (iVar4 == 4) {
    pcm_driver_control(4,extraout_r1);
    return;
  }
  return;
}

