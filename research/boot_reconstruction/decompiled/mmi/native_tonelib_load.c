/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462c94; original_symbol=native_tonelib_load */

void native_tonelib_load(void)

{
  int iVar1;
  uint uStack_18;
  
  iVar1 = native_audio_workspace();
  native_read_file((uint)(iVar1 + 0x100000),DAT_00462ccc,0x32000,&uStack_18);
  native_tonelib_setup((uint *)(iVar1 + 0x100000));
  return;
}

