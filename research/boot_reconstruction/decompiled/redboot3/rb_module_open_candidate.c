/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00040a14; original_symbol=rb_module_open_candidate */

int * rb_module_open_candidate(int param_1,uint param_2,undefined4 param_3)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  code *pcVar6;
  undefined4 *puVar7;
  int *piVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  int *unaff_r6;
  int iVar13;
  uint uVar14;
  
  uVar9 = param_2 & 0xff;
  if (uVar9 == 0) {
    unaff_r6 = rb_module_file_open(param_1);
  }
  else if (uVar9 == 3) {
    unaff_r6 = rb_module_memory_open(param_1,(int)param_2 >> 8);
  }
  else {
    FUN_0004563c(DAT_00040eac,param_2,param_3,uVar9);
  }
  if (unaff_r6 == (int *)0x0) {
    return (int *)0x0;
  }
  iVar2 = (*DAT_00040eb0)(0x34);
  unaff_r6[7] = iVar2;
  if (iVar2 != 0) {
    (*(code *)unaff_r6[5])(unaff_r6,0);
    (*(code *)unaff_r6[4])(unaff_r6,1,0x34,unaff_r6[7]);
    piVar8 = (int *)unaff_r6[7];
    if ((((*piVar8 == DAT_00040eb4) && ((char)piVar8[1] == '\x01')) && ((short)piVar8[4] == 1)) &&
       ((*(short *)((int)piVar8 + 0x12) == 0x28 && (*(char *)((int)piVar8 + 5) == '\x01')))) {
      puVar7 = (undefined4 *)(*DAT_00040eb0)((*(ushort *)(piVar8 + 0xc) + 1) * 4);
      unaff_r6[0xb] = (int)puVar7;
      if (puVar7 == (undefined4 *)0x0) {
        *DAT_00040ebc = DAT_00040ec0;
      }
      else {
        FUN_000459a4(puVar7,0,(*(ushort *)(unaff_r6[7] + 0x30) + 1) * 4);
        iVar2 = (*DAT_00040eb0)((uint)*(ushort *)(unaff_r6[7] + 0x30) *
                                (uint)*(ushort *)(unaff_r6[7] + 0x2e));
        unaff_r6[8] = iVar2;
        if (iVar2 != 0) {
          (*(code *)unaff_r6[5])(unaff_r6,*(undefined4 *)(unaff_r6[7] + 0x20));
          (*(code *)unaff_r6[4])
                    (unaff_r6,*(undefined2 *)(unaff_r6[7] + 0x2e),
                     *(undefined2 *)(unaff_r6[7] + 0x30),unaff_r6[8]);
          uVar1 = *(ushort *)(unaff_r6[7] + 0x32);
          iVar12 = unaff_r6[0xb];
          iVar2 = rb_module_load_section((int)unaff_r6,(uint)uVar1);
          *(int *)((uint)uVar1 * 4 + iVar12) = iVar2;
          iVar2 = rb_module_get_section((int)unaff_r6,(uint)*(ushort *)(unaff_r6[7] + 0x32));
          if (iVar2 == 0) {
            FUN_00040898((int)unaff_r6);
            return (int *)0x0;
          }
          uVar9 = (uint)*(ushort *)(unaff_r6[7] + 0x30);
          if (1 < uVar9) {
            iVar12 = 1;
            iVar13 = 0x28;
            do {
              iVar3 = FUN_0004caf4((uint *)(iVar2 + *(int *)(unaff_r6[8] + iVar13)),DAT_00040ec4);
              if (iVar3 == 0) {
                unaff_r6[9] = iVar12;
                iVar11 = unaff_r6[0xb];
                iVar3 = rb_module_load_section((int)unaff_r6,iVar12);
                *(int *)(iVar12 * 4 + iVar11) = iVar3;
                if (*(int *)(unaff_r6[0xb] + iVar12 * 4) == 0) {
                  FUN_00040898((int)unaff_r6);
                  return (int *)0x0;
                }
              }
              iVar3 = FUN_0004caf4((uint *)(iVar2 + *(int *)(unaff_r6[8] + iVar13)),DAT_00040ec8);
              if (iVar3 == 0) {
                unaff_r6[10] = iVar12;
                iVar11 = unaff_r6[0xb];
                iVar3 = rb_module_load_section((int)unaff_r6,iVar12);
                *(int *)(iVar12 * 4 + iVar11) = iVar3;
                if (*(int *)(unaff_r6[0xb] + iVar12 * 4) == 0) {
                  FUN_00040898((int)unaff_r6);
                  return (int *)0x0;
                }
              }
              iVar12 = iVar12 + 1;
              uVar9 = (uint)*(ushort *)(unaff_r6[7] + 0x30);
              iVar13 = iVar13 + 0x28;
            } while (iVar12 < (int)uVar9);
          }
          iVar13 = unaff_r6[0xb];
          iVar2 = *(int *)(iVar13 + unaff_r6[9] * 4);
          iVar12 = unaff_r6[9] * 0x28 + unaff_r6[8];
          uVar4 = FUN_0004bc08(*(uint *)(iVar12 + 0x14),*(uint *)(iVar12 + 0x24));
          if (1 < (int)uVar4) {
            uVar14 = 1;
            iVar12 = 0;
            do {
              if (*(ushort *)(iVar2 + 0x1e) == DAT_00040ecc) {
                uVar10 = *(int *)(iVar2 + 0x14) - 1;
                iVar12 = (iVar12 + uVar10 & ~uVar10) + *(int *)(iVar2 + 0x18);
              }
              uVar14 = uVar14 + 1;
              iVar2 = iVar2 + 0x10;
            } while (uVar4 != uVar14);
            if (iVar12 != 0) {
              uVar5 = (*DAT_00040eb0)();
              *(undefined4 *)(iVar13 + uVar9 * 4) = uVar5;
              iVar2 = unaff_r6[0xb];
              if (*(int *)(uVar9 * 4 + iVar2) == 0) {
                FUN_00040898((int)unaff_r6);
                return (int *)0x0;
              }
              iVar13 = unaff_r6[9];
              iVar12 = iVar13 * 0x28 + unaff_r6[8];
              uVar4 = FUN_0004bc08(*(uint *)(iVar12 + 0x14),*(uint *)(iVar12 + 0x24));
              iVar2 = *(int *)(iVar2 + iVar13 * 4);
              if (1 < (int)uVar4) {
                uVar14 = 1;
                iVar12 = 0;
                do {
                  if (*(ushort *)(iVar2 + 0x1e) == DAT_00040ecc) {
                    uVar10 = *(int *)(iVar2 + 0x14) - 1;
                    uVar10 = iVar12 + uVar10 & ~uVar10;
                    *(short *)(iVar2 + 0x1e) = (short)uVar9;
                    *(uint *)(iVar2 + 0x14) = uVar10;
                    iVar12 = uVar10 + *(int *)(iVar2 + 0x18);
                  }
                  uVar14 = uVar14 + 1;
                  iVar2 = iVar2 + 0x10;
                } while (uVar4 != uVar14);
              }
            }
          }
          if (1 < *(ushort *)(unaff_r6[7] + 0x30)) {
            iVar2 = 1;
            iVar12 = 0x28;
            do {
              iVar13 = *(int *)(unaff_r6[8] + iVar12 + 4);
              if ((iVar13 == 4 || iVar13 == 9) &&
                 (iVar13 = rb_module_relocation_section((int)unaff_r6,iVar2), iVar13 < 0)) {
                FUN_00040898((int)unaff_r6);
                return (int *)0x0;
              }
              iVar2 = iVar2 + 1;
              iVar12 = iVar12 + 0x28;
            } while (iVar2 < (int)(uint)*(ushort *)(unaff_r6[7] + 0x30));
          }
          rb_module_cache_sync();
          pcVar6 = (code *)rb_module_find_symbol((int)unaff_r6,DAT_00040ed0);
          if (pcVar6 == (code *)0x0) {
            return unaff_r6;
          }
          (*pcVar6)();
          return unaff_r6;
        }
        *DAT_00040ebc = DAT_00040ec0;
      }
    }
    else {
      *DAT_00040ebc = DAT_00040eb8;
    }
  }
  FUN_00040898((int)unaff_r6);
  return (int *)0x0;
}

