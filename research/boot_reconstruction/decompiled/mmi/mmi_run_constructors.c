/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00299a98; original_symbol=mmi_run_constructors */

void mmi_run_constructors(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_00299ad0;
  puVar2 = DAT_00299acc;
  if (DAT_00299acc < DAT_00299ad0) {
    return;
  }
  do {
    puVar3 = puVar2 + -1;
    (*(code *)*puVar2)();
    puVar2 = puVar3;
  } while (puVar1 <= puVar3);
  return;
}

