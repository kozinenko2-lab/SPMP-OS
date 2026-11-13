/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00040f08; original_symbol=rb_module_resolve_external */

undefined4 rb_module_resolve_external(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 uVar3;
  uint *puVar4;
  uint uVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint *puVar8;
  
  iVar1 = rb_module_get_section(param_1,*(int *)(param_1 + 0x24));
  iVar2 = rb_module_get_section(param_1,*(int *)(param_1 + 0x28));
  puVar8 = (uint *)(iVar2 + *(int *)(iVar1 + param_2 * 0x10));
  uVar7 = *DAT_00040f90;
  uVar3 = extraout_r2;
  puVar4 = DAT_00040f90;
  if (uVar7 != 0) {
    uVar5 = 0;
    puVar6 = DAT_00040f94;
    while( true ) {
      iVar1 = FUN_0004caf4(puVar8,(uint *)*puVar6);
      if (iVar1 == 0) {
        return puVar6[1];
      }
      puVar4 = (uint *)(uVar5 + 1);
      uVar5 = (uint)puVar4 & 0xff;
      uVar3 = extraout_r2_00;
      if (uVar7 <= uVar5) break;
      puVar6 = puVar6 + 2;
    }
  }
  FUN_0004563c(DAT_00040f98,(uint)puVar8,uVar3,puVar4);
  return 0;
}

