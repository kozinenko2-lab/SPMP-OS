/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00041054; original_symbol=rb_module_relocation_section */

undefined4 rb_module_relocation_section(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 extraout_r3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  
  iVar1 = rb_module_load_section(param_1,param_2);
  *(int *)(*(int *)(param_1 + 0x2c) + param_2 * 4) = iVar1;
  iVar2 = rb_module_get_section(param_1,*(int *)(*(int *)(param_1 + 0x20) + param_2 * 0x28 + 0x1c));
  iVar4 = param_2 * 0x28 + *(int *)(param_1 + 0x20);
  uVar3 = FUN_0004bc08(*(uint *)(iVar4 + 0x14),*(uint *)(iVar4 + 0x24));
  if (uVar3 != 0) {
    uVar7 = 0;
    do {
      iVar6 = *(int *)(uVar7 * 8 + iVar1);
      uVar5 = *(uint *)(uVar7 * 8 + iVar1 + 4);
      iVar4 = rb_module_resolve_symbol(param_1,uVar5 >> 8);
      if ((iVar4 == 0) ||
         (iVar4 = rb_module_relocate_word(uVar5 & 0xff,(uint *)(iVar2 + iVar6),iVar4,extraout_r3),
         iVar4 != 0)) {
        return 0xffffffff;
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 != uVar3);
  }
  FUN_00040868(param_1,param_2);
  return 0;
}

