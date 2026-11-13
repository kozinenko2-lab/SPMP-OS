/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0033a2dc; original_symbol=ecos_thread_create_core */

void ecos_thread_create_core
               (int *param_1,uint param_2,int param_3,int param_4,int param_5,int param_6,
               uint param_7)

{
  ushort uVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  uint extraout_r1;
  ushort *extraout_r2;
  ushort *puVar6;
  uint uVar7;
  undefined4 *puVar8;
  
  param_1[4] = param_3;
  param_1[5] = param_4;
  param_1[6] = 0;
  if (param_7 < 0x460) {
    FUN_002815bc(DAT_0033a460,param_2,param_3,0);
  }
  *param_1 = param_6;
  param_1[1] = param_7;
  param_1[2] = param_6;
  param_1[3] = param_6 + param_7;
  FUN_0033c158((int)(param_1 + 7),param_1,param_2);
  FUN_00339528((int)(param_1 + 0x13),*DAT_0033a464,DAT_0033a468,param_1 + 0x13);
  param_1[0x1d] = (int)param_1;
  param_1[0xe] = 4;
  param_1[0xf] = 1;
  param_1[0x10] = 0;
  param_1[0x1e] = 0;
  param_1[0x1f] = 0;
  piVar2 = DAT_0033a46c;
  *DAT_0033a46c = *DAT_0033a46c + 1;
  puVar6 = DAT_0033a470;
  uVar1 = *DAT_0033a470;
  uVar5 = (uint)uVar1;
  *DAT_0033a470 = uVar1 + 1;
  *(ushort *)(param_1 + 0x12) = uVar1;
  iVar4 = *piVar2 + -1;
  if (iVar4 == 0) {
    FUN_0033bf08(0,uVar5,(int *)puVar6);
  }
  else {
    if (iVar4 == 0) {
      FUN_002815bc(DAT_0033a474,uVar5,puVar6,*piVar2);
      uVar5 = extraout_r1;
      puVar6 = extraout_r2;
    }
    if (*DAT_0033a46c == 0) {
      FUN_002815bc(DAT_0033a478,uVar5,puVar6,0);
    }
    *DAT_0033a46c = iVar4;
  }
  iVar4 = 0;
  do {
    param_1[iVar4 + 0x20] = 0;
    iVar4 = iVar4 + 1;
  } while (iVar4 < 6);
  param_1[0x26] = param_5;
  FUN_0033bcc4();
  uVar5 = param_1[3] & 0xfffffff0;
  puVar8 = (undefined4 *)(uVar5 - 0x50);
  uVar7 = 0;
  do {
    puVar8[uVar7] = uVar7 | 0x11110000;
    uVar7 = uVar7 + 1;
  } while ((int)uVar7 < 0xb);
  *puVar8 = param_1;
  *(uint *)(uVar5 - 0x1c) = uVar5;
  uVar3 = DAT_0033a480;
  *(undefined4 *)(uVar5 - 0x18) = DAT_0033a480;
  *(undefined4 *)(uVar5 - 0x14) = uVar3;
  *(undefined4 *)(uVar5 - 0x10) = 0x13;
  param_1[3] = (int)puVar8;
  return;
}

