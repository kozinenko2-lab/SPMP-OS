/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e6004; original_symbol=blitter_transfer */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4
blitter_transfer(uint param_1,uint param_2,uint param_3,undefined4 param_4,uint param_5,uint param_6
                ,ushort *param_7,uint param_8,int param_9,int param_10)

{
  undefined2 *puVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  ushort *local_40;
  uint local_3c;
  uint local_38;
  
  puVar1 = DAT_002e65c0;
  local_38 = 0;
  local_3c = 0;
  local_40 = (ushort *)0x0;
  *DAT_002e65c0 = 1;
  if ((param_2 & 1) != 0) {
    MMI_diag_stub_00298B64();
    *puVar1 = 0;
    return 0;
  }
  if ((param_8 & 3) != 0) {
    MMI_diag_stub_00298B64();
    *DAT_002e65c0 = 0;
    return 0;
  }
  if (param_7 != (ushort *)0x0) {
    if ((*param_7 & 1) != 0) {
      MMI_diag_stub_00298B64();
      return 0;
    }
    if ((param_7[2] & 1) != 0) {
      MMI_diag_stub_00298B64();
      *DAT_002e65c0 = 0;
      return 0;
    }
  }
  if (800 < param_5) {
    *DAT_002e65c0 = 0;
    MMI_diag_stub_00298B64();
    return 0;
  }
  if (param_7 == (ushort *)0x0) {
    if (param_6 < param_3 && param_2 < param_5) {
      MMI_diag_stub_00298B64();
      param_3 = param_6;
    }
    if (param_3 < param_6 && param_5 < param_2) {
      MMI_diag_stub_00298B64();
      param_6 = param_3;
    }
  }
  else {
    if ((param_7[2] < param_5) && (param_6 < param_7[3])) {
      MMI_diag_stub_00298B64();
      param_7[3] = (ushort)param_6;
    }
    if ((param_5 < param_7[2]) && (param_7[3] < param_6)) {
      MMI_diag_stub_00298B64();
      param_6 = (uint)param_7[3];
    }
  }
  if (param_7 == (ushort *)0x0) {
    _DAT_92007024 = param_8;
    _DAT_9200703c = param_9;
    _DAT_92007040 = param_10;
    _DAT_9200701c = param_7;
    _DAT_92007020 = param_7;
    goto LAB_002e6384;
  }
  if ((*param_7 & 1) == 0) {
    if ((int)(short)param_7[1] + (uint)param_7[3] <= param_3) {
      local_38 = param_2 * (int)(short)param_7[1];
      local_3c = local_38 >> 2;
      param_3 = (uint)param_7[3];
    }
    iVar3 = (int)(short)*param_7;
    uVar2 = (uint)param_7[2];
    if (iVar3 + uVar2 <= param_2) {
      local_38 = local_38 + iVar3;
      local_3c = local_3c + iVar3 / 2;
      local_40 = (ushort *)(param_2 - uVar2);
      param_2 = uVar2;
    }
  }
  if (param_1 == 6) {
LAB_002e62f0:
    _DAT_92007018 = param_2;
  }
  else {
    if (param_1 < 7) {
      if (param_1 == 3) {
        local_38 = local_38 * 3;
        local_40 = (ushort *)((int)local_40 * 3);
        _DAT_92007018 = param_2 * 3;
        goto LAB_002e62f4;
      }
    }
    else if (param_1 == 9) goto LAB_002e62f0;
    local_38 = local_38 << 1;
    local_40 = (ushort *)((int)local_40 << 1);
    _DAT_92007018 = param_2 << 1;
  }
LAB_002e62f4:
  _DAT_9200701c = local_40;
  _DAT_92007020 = (ushort *)((uint)local_40 >> 1);
  *DAT_002e65e0 = local_38;
  _DAT_92007024 = param_8 + local_38;
  _DAT_9200703c = param_9 + local_3c;
  _DAT_92007040 = param_10 + local_3c;
LAB_002e6384:
  uVar4 = _DAT_92007004 & 0xffffee88;
  uVar2 = FUN_002e5c94(param_1);
  iVar3 = FUN_002e5c94(param_4);
  uVar2 = uVar2 | iVar3 << 4;
  _DAT_92007004 = uVar4 | uVar2;
  if (param_2 != (param_2 & 0x1fff)) {
    param_2 = 0x1fff;
  }
  uVar2 = uVar2 & 0x107;
  if (uVar2 == 2) {
    _DAT_92007018 = param_2 * 3;
  }
  else if (uVar2 == 0x106 || uVar2 == 6) {
    _DAT_92007018 = param_2;
  }
  else {
    _DAT_92007018 = param_2 << 1;
  }
  if (param_3 != (param_3 & 0x1fff)) {
    param_3 = 0x1fff;
  }
  if (param_5 != (param_5 & 0x7ff)) {
    param_5 = 0x7ff;
  }
  if (param_6 != (param_6 & 0x7ff)) {
    param_6 = 0x7ff;
  }
  _DAT_92007008 = param_2;
  _DAT_9200700c = param_3;
  _DAT_92007010 = param_5;
  _DAT_92007014 = param_6;
  if (param_2 == param_5) {
    _DAT_9200702c = 0;
  }
  else if (param_2 < param_5) {
    _DAT_9200702c = FUN_00333f98(param_2 << 0x10,param_5);
    _DAT_9200702c = _DAT_9200702c | 0x10000;
  }
  else {
    _DAT_9200702c = FUN_00333f98((param_2 + param_5 * 0x1000) - 1,param_2);
    _DAT_9200702c = _DAT_9200702c | 0x100000;
  }
  if (param_3 == param_6) {
    _DAT_92007030 = 0;
  }
  else if (param_3 < param_6) {
    _DAT_92007030 = FUN_00333f98(param_3 << 0x10,param_6);
    _DAT_92007030 = _DAT_92007030 | 0x10000;
  }
  else {
    uVar2 = FUN_00333f98((param_6 * 0x1000 + param_3) - 1,param_3);
    _DAT_9200700c = FUN_00333f98((param_6 * 0x1000 + uVar2) - 1,uVar2);
    _DAT_92007030 = uVar2 | 0x100000;
  }
  return 1;
}

