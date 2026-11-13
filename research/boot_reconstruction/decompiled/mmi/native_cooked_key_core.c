/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d2ffc; original_symbol=native_cooked_key_core */

int native_cooked_key_core(int param_1)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = native_key_event_collector();
  iVar3 = FUN_0045ffbc();
  if (iVar3 != 6) {
    if (iVar3 == 8) {
      if ((uVar2 & 1) == 0) {
        if ((uVar2 & 2) == 0) {
          if ((uVar2 & 4) == 0) {
            if ((uVar2 & 8) == 0) {
              if ((uVar2 & 0x800) == 0) {
                if ((uVar2 & 0x20) == 0) {
                  if ((uVar2 & 0x10) == 0) {
                    *(undefined4 *)(param_1 + 4) = 0;
                    return 0;
                  }
                  *(undefined4 *)(param_1 + 4) = 0x4000;
                }
                else {
                  *(undefined4 *)(param_1 + 4) = 0x8000;
                }
              }
              else {
                *(undefined4 *)(param_1 + 4) = 0x1a00;
              }
            }
            else {
              *(undefined4 *)(param_1 + 4) = 0x400;
            }
          }
          else {
            *(undefined4 *)(param_1 + 4) = 0x200;
          }
        }
        else {
          *(undefined4 *)(param_1 + 4) = 0x1e00;
        }
      }
      else {
        *(undefined4 *)(param_1 + 4) = 0x1c00;
      }
    }
    return iVar3;
  }
  *(undefined4 *)(param_1 + 4) = 0;
  if (uVar2 == 0) {
    return 0;
  }
  if ((uVar2 & 1) != 0) {
    *(undefined4 *)(param_1 + 4) = 1;
  }
  if ((uVar2 & 2) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 2;
  }
  if ((uVar2 & 4) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 4;
  }
  if ((uVar2 & 8) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 8;
  }
  if ((uVar2 & 0x10) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x10000;
  }
  if ((uVar2 & 0x20) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x20000;
  }
  if ((uVar2 & 0x80000) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x80000;
  }
  if ((uVar2 & 0x1000) != 0) {
    *(undefined4 *)(param_1 + 4) = 0;
    FUN_002d6d60(0xd07,(uint *)0x0,0);
    FUN_003c4c5c();
    FUN_002d8b88(0x569,0,0);
    native_pause_callback();
    ecos_thread_delay_wrapper(0x1e,0);
    return 0;
  }
  MMI_diag_stub_00298B64();
  if (*DAT_002d3244 == '\0') {
    if ((uVar2 & 0x200) != 0) {
      MMI_diag_stub_00298B64();
      puVar1 = DAT_002d324c;
      uVar2 = *DAT_002d324c;
      *DAT_002d324c = uVar2 + 1;
      if (0x1f < (int)(uVar2 + 1)) {
        *puVar1 = 0x1f;
      }
      puVar1 = DAT_002d324c;
      FUN_003e30cc((char)*DAT_002d324c);
      FUN_002c4de8(*puVar1);
      uVar2 = 0;
    }
    if ((uVar2 & 0x100) != 0) {
      MMI_diag_stub_00298B64();
      puVar1 = DAT_002d324c;
      uVar2 = *DAT_002d324c;
      *DAT_002d324c = uVar2 - 1;
      if ((int)(uVar2 - 1) < 0) {
        *puVar1 = 0;
      }
      puVar1 = DAT_002d324c;
      FUN_003e30cc((char)*DAT_002d324c);
      FUN_002c4de8(*puVar1);
      uVar2 = 0;
    }
  }
  if ((uVar2 & 0x800) != 0) {
    *(uint *)(param_1 + 4) = *(uint *)(param_1 + 4) | 0x2000;
  }
  return 1;
}

