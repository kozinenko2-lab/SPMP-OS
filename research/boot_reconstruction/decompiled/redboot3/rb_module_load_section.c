/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00040950; original_symbol=rb_module_load_section */

int rb_module_load_section(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_2 * 0x28;
  iVar2 = (*DAT_000409d8)(*(undefined4 *)(*(int *)(param_1 + 0x20) + iVar1 + 0x14));
  if (iVar2 == 0) {
    *DAT_000409e0 = DAT_000409dc;
  }
  else {
    (**(code **)(param_1 + 0x14))(param_1,*(undefined4 *)(iVar1 + *(int *)(param_1 + 0x20) + 0x10));
    (**(code **)(param_1 + 0x10))
              (param_1,1,*(undefined4 *)(iVar1 + *(int *)(param_1 + 0x20) + 0x14),iVar2);
  }
  return iVar2;
}

