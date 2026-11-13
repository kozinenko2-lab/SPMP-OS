/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000407c0; original_symbol=rb_module_find_symbol */

int rb_module_find_symbol(int param_1,uint *param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *(int *)(*(int *)(param_1 + 0x2c) + *(int *)(param_1 + 0x28) * 4);
  iVar4 = *(int *)(*(int *)(param_1 + 0x2c) + *(int *)(param_1 + 0x24) * 4);
  iVar2 = *(int *)(param_1 + 0x24) * 0x28 + *(int *)(param_1 + 0x20);
  uVar1 = FUN_0004bc08(*(uint *)(iVar2 + 0x14),*(uint *)(iVar2 + 0x24));
  if (0 < (int)uVar1) {
    uVar3 = 0;
    do {
      iVar2 = FUN_0004caf4((uint *)(iVar5 + *(int *)(iVar4 + uVar3 * 0x10)),param_2);
      if (iVar2 == 0) {
        iVar2 = rb_module_resolve_symbol(param_1,uVar3);
        return iVar2;
      }
      uVar3 = uVar3 + 1;
    } while (uVar1 != uVar3);
  }
  *DAT_00040858 = DAT_00040854;
  return 0;
}

