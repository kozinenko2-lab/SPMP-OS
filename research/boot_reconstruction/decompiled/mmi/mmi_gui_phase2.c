/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003be6a0; original_symbol=mmi_gui_phase2 */

undefined4 mmi_gui_phase2(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = mmi_gui_init_impl();
  if (iVar1 < 0) {
    uVar2 = 0xffffffff;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

