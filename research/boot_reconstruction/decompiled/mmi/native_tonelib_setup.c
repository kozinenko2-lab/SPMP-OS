/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00464f78; original_symbol=native_tonelib_setup */

undefined4 native_tonelib_setup(uint *param_1)

{
  int *piVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  uint uVar9;
  
  iVar6 = DAT_00465118;
  uVar7 = 0;
  puVar2 = param_1;
  do {
    if (*puVar2 < 0x80) {
      *(undefined4 *)(iVar6 + uVar7 * 4) = 0;
    }
    else {
      puVar4 = (uint *)(*puVar2 + (int)param_1);
      piVar8 = (int *)(iVar6 + uVar7 * 4);
      *piVar8 = (int)puVar4;
      uVar9 = *puVar4;
      puVar4[1] = puVar4[1] + (int)puVar4;
      puVar4[2] = puVar4[2] + (int)puVar4;
      puVar4[3] = puVar4[3] + (int)puVar4;
      puVar4[4] = puVar4[4] + (int)puVar4;
      puVar4[5] = puVar4[5] + (int)puVar4;
      uVar3 = 0;
      piVar1 = *(int **)(*piVar8 + 8);
      if (uVar9 != 0) {
        do {
          uVar3 = uVar3 + 1;
          *piVar1 = *piVar1 + (int)puVar4;
          piVar1 = piVar1 + 1;
        } while (uVar3 < uVar9);
      }
      uVar3 = 0;
      piVar1 = *(int **)(*piVar8 + 0x10);
      if (uVar9 != 0) {
        do {
          uVar3 = uVar3 + 1;
          *piVar1 = *piVar1 + (int)puVar4;
          piVar1 = piVar1 + 1;
        } while (uVar3 < uVar9);
      }
    }
    uVar7 = uVar7 + 1;
    puVar2 = puVar2 + 1;
  } while (uVar7 < 0x80);
  uVar7 = 0;
  puVar2 = param_1;
  while ((0x7f < *puVar2 ||
         (iVar5 = *(int *)(iVar6 + *puVar2 * 4), *(int *)(iVar6 + uVar7 * 4) = iVar5, iVar5 != 0)))
  {
    iVar5 = DAT_0046511c;
    uVar7 = uVar7 + 1;
    puVar2 = puVar2 + 1;
    if (0x7f < uVar7) {
      puVar2 = param_1 + 0x80;
      uVar7 = 0;
      do {
        if (*puVar2 < 0x80) {
          *(undefined4 *)(iVar5 + uVar7 * 4) = 0;
        }
        else {
          iVar6 = *puVar2 + (int)param_1;
          *(int *)(iVar5 + uVar7 * 4) = iVar6;
          *(int *)(iVar6 + 4) = *(int *)(iVar6 + 4) + iVar6;
        }
        uVar7 = uVar7 + 1;
        puVar2 = puVar2 + 1;
      } while (uVar7 < 0x80);
      puVar2 = param_1 + 0x80;
      uVar7 = 0;
      while ((0x7f < *puVar2 ||
             (iVar6 = *(int *)(iVar5 + *puVar2 * 4), *(int *)(iVar5 + uVar7 * 4) = iVar6, iVar6 != 0
             ))) {
        uVar7 = uVar7 + 1;
        puVar2 = puVar2 + 1;
        if (0x7f < uVar7) {
          return 0;
        }
      }
      return 0xffffffff;
    }
  }
  return 0xffffffff;
}

