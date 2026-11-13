/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00040f9c; original_symbol=rb_module_resolve_symbol */

int rb_module_resolve_symbol(int param_1,int param_2)

{
  int iVar1;
  byte bVar2;
  
  iVar1 = rb_module_get_section(param_1,*(int *)(param_1 + 0x24));
  iVar1 = param_2 * 0x10 + iVar1;
  bVar2 = *(byte *)(iVar1 + 0xc) & 0xf;
  if (bVar2 < 3) {
    bVar2 = *(byte *)(iVar1 + 0xc) >> 4;
    if (bVar2 < 2) {
      if (*(short *)(iVar1 + 0xe) == 0) {
        iVar1 = rb_module_resolve_external(param_1,param_2);
        return iVar1;
      }
      iVar1 = rb_module_resolve_defined(param_1,param_2);
      return iVar1;
    }
    if (bVar2 == 2) {
      iVar1 = rb_module_resolve_external(param_1,param_2);
      if (iVar1 == 0) {
        iVar1 = rb_module_resolve_defined(param_1,param_2);
        return iVar1;
      }
      return iVar1;
    }
  }
  else if (bVar2 == 3) {
    iVar1 = rb_module_get_section(param_1,(uint)*(ushort *)(iVar1 + 0xe));
    return iVar1;
  }
  return 0;
}

