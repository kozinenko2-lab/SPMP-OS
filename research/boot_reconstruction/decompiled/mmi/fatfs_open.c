/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00283788; original_symbol=fatfs_open */

undefined4
fatfs_open(undefined4 param_1,undefined4 param_2,uint *param_3,uint param_4,uint *param_5)

{
  uint uVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  int iStack_1c;
  
  if ((param_4 & 8) != 0) {
    param_4 = param_4 | 10;
  }
  iStack_1c = DAT_00283848;
  FUN_002834dc(&iStack_1c,param_3);
  uVar1 = FUN_002a8264(iStack_1c,param_4);
  if (-1 < (int)uVar1) {
    *param_5 = param_4 & 0x283 | *param_5;
    *(undefined2 *)((int)param_5 + 6) = 1;
    param_5[3] = DAT_00283854;
    param_5[4] = 0;
    param_5[5] = uVar1;
    param_5[6] = 0;
    return 0;
  }
  iVar2 = FUN_002ab170(uVar1,extraout_r1);
  if (iVar2 == -8) {
    return 0x15;
  }
  FUN_002ab170(iVar2,extraout_r1_00);
  MMI_diag_stub_00298B64();
  return 5;
}

