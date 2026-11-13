/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00041154; original_symbol=rb_module_memory_open */

int * rb_module_memory_open(int param_1,int param_2)

{
  int *piVar1;
  
  if (param_1 == 0) {
    *DAT_00041200 = DAT_000411fc;
    return (int *)0x0;
  }
  piVar1 = (int *)(*DAT_00041204)(0x30);
  if (piVar1 == (int *)0x0) {
    *DAT_00041200 = DAT_00041208;
    return (int *)0x0;
  }
  piVar1[2] = 0;
  piVar1[3] = 0;
  piVar1[4] = 0;
  piVar1[5] = 0;
  piVar1[6] = 0;
  piVar1[7] = 0;
  piVar1[8] = 0;
  piVar1[9] = 0;
  piVar1[10] = 0;
  piVar1[0xb] = 0;
  *piVar1 = param_1;
  piVar1[1] = 3;
  piVar1[2] = 0;
  piVar1[3] = param_2;
  piVar1[4] = DAT_0004120c;
  piVar1[5] = DAT_00041210;
  piVar1[6] = DAT_00041214;
  return piVar1;
}

