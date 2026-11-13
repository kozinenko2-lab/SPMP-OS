/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00041264; original_symbol=rb_module_file_open */

int * rb_module_file_open(undefined4 param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = FUN_0003f860(param_1,1);
  piVar2 = (int *)(*DAT_000412fc)(0x30);
  if (piVar2 == (int *)0x0) {
    *DAT_00041304 = DAT_00041300;
    FUN_0003fbe4(iVar1);
    return (int *)0x0;
  }
  piVar2[2] = 0;
  piVar2[3] = 0;
  piVar2[4] = 0;
  piVar2[5] = 0;
  piVar2[6] = 0;
  piVar2[7] = 0;
  piVar2[8] = 0;
  piVar2[9] = 0;
  piVar2[10] = 0;
  piVar2[0xb] = 0;
  *piVar2 = iVar1;
  piVar2[1] = 0;
  piVar2[4] = DAT_00041308;
  piVar2[5] = DAT_0004130c;
  piVar2[6] = DAT_00041310;
  return piVar2;
}

