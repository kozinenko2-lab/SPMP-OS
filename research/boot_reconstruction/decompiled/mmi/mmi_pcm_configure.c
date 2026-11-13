/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002c554c; original_symbol=mmi_pcm_configure */

undefined4 mmi_pcm_configure(uint param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 uStack_30;
  uint local_2c;
  
  if ((1 < param_2 - 1U) && ((*(uint *)*DAT_002c5658 & 0x10) != 0)) {
    MMI_diag_stub_00298B64();
  }
  uStack_30._2_2_ = (undefined2)param_2;
  local_44 = DAT_002c5660;
  local_40 = DAT_002c5664;
  local_3c = 0;
  local_2c = param_1;
  FUN_002948fc();
  FUN_00292198();
  FUN_00292720(&uStack_30,&local_44);
  FUN_00294878(param_1 >> 2);
  uVar3 = FUN_00292058(0x400);
  FUN_0029440c((int)uVar3,(uint)((ulonglong)uVar3 >> 0x20));
  *DAT_002c5668 = 0;
  *DAT_002c566c = param_2;
  *DAT_002c5670 = param_1;
  puVar1 = (undefined4 *)FUN_00294ab0(param_1 >> 2,0);
  if (puVar1 == (undefined4 *)0x0) {
    if ((*(uint *)*DAT_002c5658 & 0x10) != 0) {
      MMI_diag_stub_00298B64();
    }
  }
  else {
    FUN_00298d34(puVar1,0,param_1 >> 2);
  }
  uVar2 = FUN_00294f30(param_1 >> 2,0);
  if ((*(uint *)*DAT_002c5658 & 1) != 0) {
    uVar2 = MMI_diag_stub_00298B64();
  }
  return uVar2;
}

