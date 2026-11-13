/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00461510; original_symbol=native_wave_parse */

int native_wave_parse(undefined4 *param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  byte bVar4;
  byte *pbVar5;
  byte local_60 [8];
  byte abStack_58 [4];
  byte abStack_54 [4];
  int local_50;
  short local_4c;
  undefined1 local_4a;
  int local_48;
  undefined1 local_3e;
  
  FUN_00298d34((undefined4 *)local_60,0,0x3c);
  FUN_00298c6c((undefined4 *)local_60,param_1,0x3c);
  iVar1 = FUN_00345c68(local_60,DAT_0046172c,4);
  if (iVar1 == 0) {
    iVar1 = FUN_00345c68(abStack_58,DAT_00461738,4);
    if (iVar1 == 0) {
      iVar1 = FUN_00345c68(abStack_54,DAT_00461740,4);
      if (iVar1 == 0) {
        if (local_4c == 1) {
          *(undefined1 *)(param_2 + 4) = 3;
          param_2[7] = 0x1000;
        }
        else {
          if (local_4c != 0x11) {
            MMI_diag_stub_00298B64();
            return 1;
          }
          *(undefined1 *)(param_2 + 4) = 4;
          param_2[7] = (uint)*(ushort *)(param_1 + 8);
        }
        *param_2 = local_48;
        *(undefined1 *)(param_2 + 3) = local_4a;
        *(undefined1 *)((int)param_2 + 0xd) = local_3e;
        pbVar5 = (byte *)((int)param_1 + local_50 + 0x14);
        do {
          iVar2 = FUN_00345c68(pbVar5,DAT_0046174c,4);
          iVar1 = 0;
          if (iVar2 == 0) goto LAB_004616a8;
          iVar1 = native_read_le32(pbVar5 + 4);
          pbVar5 = pbVar5 + 4 + iVar1 + 4;
        } while ((int)pbVar5 - (int)param_1 < 0x2001);
        iVar1 = 1;
LAB_004616a8:
        iVar2 = native_read_le32(pbVar5 + 4);
        param_2[5] = iVar2;
        if (iVar1 != 1) {
          param_2[6] = (int)(pbVar5 + (8 - (int)param_1));
        }
        if ((char)param_2[4] == '\x03') {
          uVar3 = (uint)(*(byte *)((int)param_2 + 0xd) >> 3) *
                  *param_2 * (uint)*(byte *)(param_2 + 3);
        }
        else {
          if ((char)param_2[4] != '\x04') {
            MMI_diag_stub_00298B64();
            return 1;
          }
          uVar3 = *param_2 * (uint)*(byte *)(param_2 + 3) >> 1;
        }
        uVar3 = FUN_00333f98(param_2[5],uVar3);
        param_2[2] = uVar3 + 1;
      }
      else {
        MMI_diag_stub_00298B64();
        iVar1 = 1;
      }
    }
    else {
      MMI_diag_stub_00298B64();
      iVar1 = 1;
    }
  }
  else {
    bVar4 = 0;
    do {
      MMI_diag_stub_00298B64();
      bVar4 = bVar4 + 1;
    } while (bVar4 < 4);
    MMI_diag_stub_00298B64();
    iVar1 = 1;
  }
  return iVar1;
}

