/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=0003e48c; original_symbol=rb_mmpi_checksum */

undefined4 rb_mmpi_checksum(int param_1,int *param_2,uint param_3)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  uint uVar6;
  
  if ((param_2 == (int *)0x0 || param_1 == 0) || ((param_3 & 3) != 0)) {
    return 0xffffffff;
  }
  uVar6 = param_3 >> 4;
  iVar2 = 0;
  iVar3 = 0;
  uVar4 = 0;
  do {
    if ((uVar4 & 1) == 0) {
      if (uVar6 != 0) {
        uVar1 = 0;
        piVar5 = param_2;
        do {
          iVar2 = iVar2 + *piVar5;
          uVar1 = uVar1 + 1;
          piVar5 = piVar5 + 1;
        } while (uVar1 != uVar6);
        param_2 = param_2 + uVar6;
      }
    }
    else {
      if ((uVar4 & 2) != 0) {
        uVar6 = uVar6 + ((param_3 & 0xf) >> 2);
      }
      if (uVar6 != 0) {
        uVar1 = 0;
        piVar5 = param_2;
        do {
          iVar3 = iVar3 + *piVar5;
          uVar1 = uVar1 + 1;
          piVar5 = piVar5 + 1;
        } while (uVar1 != uVar6);
        param_2 = param_2 + uVar6;
      }
    }
    uVar4 = uVar4 + 1;
  } while (uVar4 != 4);
  *(int *)(param_1 + 8) = iVar2;
  *(int *)(param_1 + 0xc) = iVar3;
  return 1;
}

