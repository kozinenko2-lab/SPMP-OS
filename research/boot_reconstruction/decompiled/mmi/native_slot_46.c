/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002deb1c; original_symbol=native_slot_46 */

undefined4 native_slot_46(uint param_1,ushort *param_2,ushort *param_3,byte *param_4)

{
  byte bVar1;
  uint *puVar2;
  bool bVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  byte bVar7;
  ushort uStack_34;
  short sStack_32;
  ushort uStack_30;
  short sStack_2e;
  
  param_1 = param_1 & 0xff;
  uVar6 = 0;
  bVar7 = 0;
  if (*DAT_002deef4 <= param_1) {
    return 0x70000406;
  }
  iVar5 = *DAT_002deef8 + param_1 * 0x24;
  if (param_1 == 1) {
    uVar4 = (*(code *)*DAT_002deefc)();
    *(undefined4 *)(iVar5 + -0x18) = uVar4;
  }
  else if (*(char *)(iVar5 + -0x24) == '\0') {
    return 0x70000406;
  }
  puVar2 = DAT_002def00;
  if ((0x3ff < param_2[2]) || (0x3ff < param_2[3])) {
    return 0x70000401;
  }
  if (((short)*param_2 < 0) || ((short)param_2[1] < 0)) {
    return 0x70000401;
  }
  if ((int)(uint)*(ushort *)(iVar5 + -0x1c) < (int)((int)(short)*param_2 + (uint)param_2[2])) {
    return 0x70000401;
  }
  if ((int)(uint)*(ushort *)(iVar5 + -0x1a) < (int)((int)(short)param_2[1] + (uint)param_2[3])) {
    return 0x70000401;
  }
  if (((0x7ff < (ushort)(*param_3 + 0x400)) || ((short)param_3[1] < -0x400)) ||
     (0x3ff < (short)param_3[1])) {
    return 0x70000401;
  }
  uStack_30 = (param_4[5] & 0xfc) * 8 + (param_4[4] & 0xf8) * 0x100 + (ushort)(param_4[6] >> 3);
  sStack_2e = (param_4[1] & 0xfc) * 8 + (*param_4 & 0xf8) * 0x100 + (ushort)(param_4[2] >> 3);
  uStack_34 = (param_4[0xd] & 0xfc) * 8 + (param_4[0xc] & 0xf8) * 0x100 +
              (ushort)(param_4[0xe] >> 3);
  sStack_32 = (param_4[9] & 0xfc) * 8 + (param_4[8] & 0xf8) * 0x100 + (ushort)(param_4[10] >> 3);
  bVar1 = param_4[0x10];
  DAT_002def00[1] = *(uint *)(iVar5 + -0x18);
  gfx_context_dimensions
            ((int)puVar2,(uint)*(ushort *)(iVar5 + -0x1c),*(undefined2 *)(iVar5 + -0x1a));
  uVar4 = native_image_pixel_format(*(undefined4 *)(iVar5 + -0x20));
  FUN_00312e38((int)puVar2,(char)uVar4);
  switch(*(undefined4 *)(iVar5 + -0x20)) {
  case 1:
  case 2:
    FUN_00312e70(DAT_002def00,*(uint *)(iVar5 + -0x10));
    break;
  case 3:
    bVar7 = *(byte *)(iVar5 + -10) >> 6 & 1;
    if (bVar7 == 1) {
      FUN_00312e38((int)DAT_002def00,0);
    }
    else {
      bVar3 = FUN_00312e24(DAT_002def04);
      uVar6 = (uint)bVar3;
      FUN_00312e08(DAT_002def00,1);
    }
    break;
  case 4:
    bVar3 = FUN_00312e24(DAT_002def04);
    uVar6 = (uint)bVar3;
    FUN_00312e08(DAT_002def00,1);
  }
  bVar3 = FUN_00312d70(DAT_002def04);
  if ((bVar3) || ((*DAT_002def04 & 0x100004) == 0x100004)) {
    FUN_00312de8(DAT_002def04);
    FUN_00314068(DAT_002def04,*param_3,param_3[1],param_2[2],param_2[3],DAT_002def00,*DAT_002def08,
                 DAT_002def08[1],&uStack_34,&uStack_30,*DAT_002def0c | (ushort)((bVar1 & 0xf) << 8))
    ;
  }
  else {
    FUN_00314068(DAT_002def04,*param_3,param_3[1],param_2[2],param_2[3],DAT_002def00,*param_2,
                 param_2[1],&uStack_34,&uStack_30,*DAT_002def0c | (ushort)((bVar1 & 0xf) << 8));
  }
  native_graphics_finish();
  if (*(int *)(iVar5 + -0x20) != 4) {
    bVar7 = bVar7 ^ 1;
    if (*(int *)(iVar5 + -0x20) != 3) {
      bVar7 = 0;
    }
    if (bVar7 == 0) {
      return 0;
    }
  }
  FUN_00312e08(DAT_002def00,uVar6);
  return 0;
}

