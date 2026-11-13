/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004628b8; original_symbol=native_sound_start */

undefined4 native_sound_start(void)

{
  uint *puVar1;
  undefined4 *puVar2;
  int iVar3;
  uint extraout_r1;
  undefined4 extraout_r2;
  
  MMI_diag_stub_00298B64();
  if (*DAT_004629a4 == 0) {
    FUN_002c554c(8000,1);
    puVar1 = DAT_004629ac;
    iVar3 = FUN_003e30bc();
    *puVar1 = (int)(char)iVar3;
    if ((int)(char)iVar3 != 0) {
      *DAT_004629ac = *DAT_004629ac & 0x1f;
    }
    FUN_002c4de8(*DAT_004629ac);
    MMI_diag_stub_00298B64();
    puVar2 = DAT_004629b8;
    ecos_thread_create_wrapper
              (0x17,DAT_004629c0,0,DAT_004629c4,DAT_004629b4,0x4000,DAT_004629b8,DAT_004629bc);
    MMI_diag_stub_00298B64();
    ecos_thread_resume_wrapper((int *)*puVar2,extraout_r1,extraout_r2);
    *DAT_004629a4 = 1;
  }
  else {
    MMI_diag_stub_00298B64();
  }
  return 0;
}

