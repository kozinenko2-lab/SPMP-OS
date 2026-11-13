/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003be5b0; original_symbol=mmi_demo_gui */

void mmi_demo_gui(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = FUN_00339ba8();
  FUN_00339bb8(uVar1,10);
  iVar2 = mmi_system_coordinator();
  if (iVar2 < 0) {
    MMI_diag_stub_00298B64();
                    /* WARNING: Subroutine does not return */
    FUN_004be95c(1);
  }
  iVar3 = FUN_003c6044();
  iVar2 = DAT_003be690;
  if (iVar3 < 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004be95c(1);
  }
  iVar3 = (**(code **)(DAT_003be694 + 0x5c))(DAT_003be690);
  if (((iVar3 != 0) && ((*(uint *)(iVar2 + 4) & 0x10) != 0)) &&
     ((*(uint *)(iVar2 + 0x18) & 0x8000000) != 0)) {
    FUN_002a1488(0);
    FUN_002a0dc8();
    MMI_diag_stub_00298B64();
  }
  iVar2 = mmi_gui_phase2();
  if (iVar2 < 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004be95c(1);
  }
  if ((code *)*DAT_003be69c != (code *)0x0) {
    (*(code *)*DAT_003be69c)(0,0,0,5);
  }
  mmi_gui_dispatch();
  return;
}

