/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461e7c; original_symbol=native_slot_53 */

void native_slot_53(undefined4 param_1,int param_2)

{
  int *piVar1;
  undefined4 *puVar2;
  int iVar3;
  
  if (*DAT_00461f80 != '\0') {
    MMI_diag_stub_00298B64();
    return;
  }
  MMI_diag_stub_00298B64();
  if (param_2 == 0) {
    MMI_diag_stub_00298B64();
    return;
  }
  iVar3 = FUN_00461e14();
  *DAT_00461f90 = iVar3;
  if (iVar3 == 0) {
    MMI_diag_stub_00298B64();
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  FUN_00298d34((undefined4 *)*DAT_00461f90,0,0x100000);
  FUN_00462c94();
  piVar1 = DAT_00461f98;
  *DAT_00461f98 = param_2;
  MMI_diag_stub_00298B64();
  puVar2 = DAT_00461fa0;
  if (*piVar1 != 0) {
    *DAT_00461fa0 = 0;
    *(undefined1 *)(puVar2 + 2) = 0;
    puVar2[3] = 0;
    puVar2[1] = 0;
    *DAT_00461fa4 = 0;
    puVar2 = DAT_00461fa8;
    *DAT_00461fa8 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    *DAT_00461fac = 3;
    *DAT_00461fb0 = 0xd;
    *DAT_00461f80 = '\x01';
    return;
  }
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

