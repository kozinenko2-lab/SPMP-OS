/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000427a8; original_symbol=rb_mmp_parse */

int rb_mmp_parse(undefined4 *param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  undefined1 local_61;
  undefined4 auStack_60 [4];
  uint local_50;
  uint local_4c;
  int local_48;
  int iStack_3c;
  int local_38;
  int local_34;
  int local_30;
  
  iVar7 = 0;
  FUN_00042620(&iStack_3c,param_1,0x10);
  uVar4 = 0x10;
  iVar1 = FUN_00042644(&iStack_3c,DAT_0004292c);
  iVar6 = local_38;
  if (iVar1 == 0) {
    uVar2 = FUN_000430b4((int)(param_1 + 4),local_38 + -0x10);
    if (uVar2 != 0) {
      iVar1 = 0x10;
      puVar5 = (undefined4 *)((int)param_1 + local_38 * 2);
      while (iVar3 = FUN_000431d0(&local_61), iVar3 != 0) {
        *(undefined1 *)(iVar1 + (int)puVar5) = local_61;
        iVar1 = iVar1 + 1;
      }
      if (uVar2 + 0x10 == iVar1) {
        FUN_00042620(puVar5,param_1,0x10);
        param_1 = puVar5;
        iVar6 = iVar1;
      }
    }
  }
  else {
    iVar1 = FUN_00042644(&iStack_3c,DAT_00042930);
    if (iVar1 != 0) {
      return -1;
    }
  }
  iVar3 = local_30;
  iVar1 = local_34;
  if (((param_2 & 1) == 0) ||
     ((FUN_000426f8((int)&iStack_3c,param_1 + 4,iVar6 - 0x10), local_34 == iVar1 &&
      (local_30 == iVar3)))) {
    if (0x10 < iVar6) {
      do {
        FUN_00042620(auStack_60,(undefined4 *)((uVar4 & 0xfffffffc) + (int)param_1),0x24);
        if (((local_50 & 0xf0000) == 0) && (local_48 != -1)) {
          iVar7 = local_48;
        }
        uVar2 = FUN_00042650((uVar4 + 0x24 & 0xfffffffc) + (int)param_1,local_4c);
        if ((int)uVar2 < 0) {
          return -3;
        }
        uVar4 = uVar4 + 0x24 + uVar2;
      } while ((int)uVar4 < iVar6);
    }
  }
  else {
    iVar7 = -2;
  }
  return iVar7;
}

