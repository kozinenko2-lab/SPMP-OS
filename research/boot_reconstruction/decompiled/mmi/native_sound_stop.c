/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004629cc; original_symbol=native_sound_stop */

void native_sound_stop(void)

{
  undefined4 *puVar1;
  uint *puVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  uint uVar3;
  
  MMI_diag_stub_00298B64();
  *DAT_00462a60 = 0;
  native_resource_stop(0xf);
  puVar1 = DAT_00462a64;
  uVar3 = extraout_r1;
  if ((int *)*DAT_00462a64 != (int *)0x0) {
    mmi_game_pool_free((int *)*DAT_00462a64);
    *puVar1 = 0;
    uVar3 = extraout_r1_00;
  }
  ecos_thread_delete_wrapper(*DAT_00462a68,uVar3);
  mmi_pcm_stop();
  puVar2 = DAT_00462a6c;
  if (*DAT_00462a6c != 0) {
    mmi_general_free(*DAT_00462a6c);
    ecos_thread_delay_wrapper(5,0);
    *puVar2 = 0;
  }
  *DAT_00462a70 = 0;
  return;
}

