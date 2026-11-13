/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00040ed4; original_symbol=rb_module_resolve_defined */

int rb_module_resolve_defined(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = rb_module_get_section(param_1,*(int *)(param_1 + 0x24));
  iVar1 = param_2 * 0x10 + iVar1;
  iVar2 = rb_module_get_section(param_1,(uint)*(ushort *)(iVar1 + 0xe));
  return iVar2 + *(int *)(iVar1 + 4);
}

