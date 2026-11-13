/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=000409e4; original_symbol=rb_module_get_section */

undefined4 rb_module_get_section(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = param_2 * 4;
  iVar3 = *(int *)(param_1 + 0x2c);
  if (*(int *)(iVar1 + iVar3) == 0) {
    iVar2 = rb_module_load_section(param_1,param_2);
    *(int *)(iVar1 + iVar3) = iVar2;
  }
  return *(undefined4 *)(iVar1 + *(int *)(param_1 + 0x2c));
}

