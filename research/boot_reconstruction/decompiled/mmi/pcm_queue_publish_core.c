/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00294bb8; original_symbol=pcm_queue_publish_core */

undefined4 pcm_queue_publish_core(uint param_1,uint param_2,undefined4 param_3)

{
  bool bVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  char cVar6;
  bool bVar7;
  bool bVar8;
  char cVar9;
  char cVar10;
  char cVar11;
  byte in_Q;
  
  bVar8 = false;
  bVar1 = false;
  bVar7 = (param_1 & 0x10000000) != 0;
  if (bVar7) {
    param_1 = param_1 & 0xefffffff;
  }
  uVar5 = (uint)bVar7;
  if (!bVar7) {
    bVar8 = (param_1 & 0x20000000) != 0;
    if (bVar8) {
      param_1 = param_1 & 0xdfffffff;
    }
    else if ((param_1 & 0x40000000) != 0) {
      param_1 = param_1 & 0xbfffffff;
      bVar1 = true;
    }
  }
  uVar4 = uVar5 ^ 1;
  if (0 < (int)param_1) {
    uVar4 = 0;
  }
  if ((uVar4 != 0) && (!bVar1)) {
    if (param_1 == 0) {
      FUN_00342114(DAT_00294dcc,0,param_3,uVar4);
      return 0xffffffff;
    }
    FUN_00342114(DAT_00294dcc,param_1,param_3,uVar4);
    return 0xffffffff;
  }
  FUN_0033a048(DAT_00294dd0,param_2);
  uVar3 = *DAT_00294dd8;
  uVar4 = *DAT_00294dd4 - uVar3;
  if ((int)uVar4 < 0) {
    uVar4 = (*DAT_00294ddc - uVar3) + *DAT_00294dd4;
  }
  if (bVar1) {
    uVar3 = *DAT_00294dd4;
    if (*DAT_00294ddc < uVar3 + 4) {
      *DAT_00294dd8 = 0;
      uVar4 = uVar3;
    }
    else {
      *DAT_00294dd8 = *DAT_00294dd4 + 4;
      uVar4 = *DAT_00294ddc - 4;
    }
    uVar5 = 1;
  }
  if ((int)uVar4 <= (int)param_1) {
    FUN_0033a05c(DAT_00294dd0,uVar3);
    return 0xffffffff;
  }
  uVar4 = *DAT_00294dd8;
  *DAT_00294dd8 = param_1 + uVar4;
  if (uVar5 == 0) {
    if (bVar8) {
      *DAT_00294de8 = 1;
      *DAT_00294dec = *DAT_00294dd8;
    }
  }
  else {
    *DAT_00294de0 = 1;
    *DAT_00294de4 = param_1 + uVar4;
  }
  puVar2 = DAT_00294dd8;
  uVar5 = *DAT_00294dd8;
  uVar4 = *DAT_00294ddc;
  cVar10 = uVar4 <= uVar5;
  cVar11 = SBORROW4(uVar5,uVar4);
  cVar6 = (int)(uVar5 - uVar4) < 0;
  cVar9 = uVar5 == uVar4;
  if ((bool)cVar10 && !(bool)cVar9) {
    uVar3 = *DAT_00294df0;
    cVar6 = (int)uVar3 < 0;
    cVar9 = uVar3 == 0;
    cVar10 = true;
    cVar11 = false;
    if ((bool)cVar9) {
      *DAT_00294df0 = 1;
      *DAT_00294df4 = uVar5;
      *puVar2 = 0;
    }
  }
  FUN_0033a05c(DAT_00294dd0,uVar3);
  *DAT_00294df8 = (uint)(byte)(cVar6 << 4 | cVar9 << 3 | cVar10 << 2 | cVar11 << 1 | in_Q) << 0x1b;
  do {
    coproc_movefrom_Data_cache_operations(3);
  } while ((((byte)(cVar9 << 3 | cVar10 << 2 | cVar11 << 1 | in_Q) & 0x1f) >> 3 & 1) == 0);
  coproc_moveto_Data_Synchronization(0);
  return 1;
}

