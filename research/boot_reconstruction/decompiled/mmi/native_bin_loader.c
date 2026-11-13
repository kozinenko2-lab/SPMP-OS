/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00460800; original_symbol=native_bin_loader */

undefined4 native_bin_loader(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  uint local_20 [2];
  
  puVar1 = DAT_00460924;
  local_20[1] = 0;
  *DAT_00460924 = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  puVar1[4] = 0;
  puVar1[5] = 0;
  puVar1[6] = 0;
  FUN_004600b0();
  iVar2 = native_load_ngame((undefined4 *)&SUB_00a00000,param_1,*DAT_00460928,(int)(local_20 + 1));
  MMI_diag_stub_00298B64();
  if (iVar2 == 0) {
    native_api_init();
    native_sound_start();
    *DAT_00460930 = 0;
    puVar1 = DAT_00460924;
    DAT_00460924[3] = DAT_00460934;
    puVar1[1] = DAT_00460938;
    puVar1[2] = DAT_0046093c;
    FUN_002d6d60(0xd06,(uint *)0x0,0);
    local_20[0] = 3;
    FUN_002d6d60(0xd08,local_20,4);
    (*(code *)&SUB_00a00000)(DAT_00460940);
    native_exit_control(-0x80000000);
    FUN_002d6d60(0xd07,(uint *)0x0,0);
    FUN_002d8b88(0x560,0,0);
    uVar3 = 0;
  }
  else {
    uVar3 = 0xfffffffe;
  }
  return uVar3;
}

