/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0003e734; original_symbol=rb_mmp_parse */

int rb_mmp_parse(undefined4 *param_1,uint param_2)

{
  int iVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined1 local_55;
  undefined4 auStack_54 [4];
  uint local_44;
  int local_40;
  int local_3c;
  int iStack_30;
  int local_2c;
  int local_28;
  int local_24;
  
  FUN_0003e3c0(&iStack_30,param_1,0x10);
  iVar3 = FUN_0003e3e4(&iStack_30,DAT_0003e8ac);
  iVar6 = local_2c;
  if (iVar3 == 0) {
    uVar4 = FUN_0003ee5c((int)(param_1 + 4),local_2c + -0x10);
    if (uVar4 != 0) {
      puVar7 = (undefined4 *)(local_2c * 2 + (int)param_1);
      iVar3 = 0x10;
      while (bVar2 = FUN_0003ef70(&local_55), bVar2) {
        *(undefined1 *)((int)puVar7 + iVar3) = local_55;
        iVar3 = iVar3 + 1;
      }
      if (uVar4 + 0x10 == iVar3) {
        FUN_0003e3c0(puVar7,param_1,0x10);
        param_1 = puVar7;
        iVar6 = uVar4 + 0x10;
      }
    }
  }
  else {
    iVar3 = FUN_0003e3e4(&iStack_30,DAT_0003e8b0);
    if (iVar3 != 0) {
      return -1;
    }
  }
  iVar1 = local_24;
  iVar3 = local_28;
  if (((param_2 & 1) == 0) ||
     ((rb_mmpi_checksum((int)&iStack_30,param_1 + 4,iVar6 - 0x10), local_28 == iVar3 &&
      (local_24 == iVar1)))) {
    if (iVar6 < 0x11) {
      iVar3 = 0;
    }
    else {
      uVar4 = 0x10;
      iVar3 = 0;
      do {
        FUN_0003e3c0(auStack_54,(undefined4 *)((uVar4 & 0xfffffffc) + (int)param_1),0x24);
        if (((local_44 & 0xf0000) == 0) && (local_3c != -1)) {
          iVar3 = local_3c;
        }
        uVar5 = rb_mmp_copy_segments((uVar4 + 0x24 & 0xfffffffc) + (int)param_1,local_40);
        if ((int)uVar5 < 0) {
          return -3;
        }
        uVar4 = uVar5 + uVar4 + 0x24;
      } while ((int)uVar4 < iVar6);
    }
  }
  else {
    iVar3 = -2;
  }
  return iVar3;
}

