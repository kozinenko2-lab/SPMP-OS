/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462118; original_symbol=native_slot_55 */

undefined4 native_slot_55(byte param_1,uint param_2,int *param_3)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int local_848 [3];
  char cStack_83b;
  undefined4 uStack_83c;
  undefined4 uStack_838;
  undefined4 uStack_834;
  int local_830;
  undefined4 local_82c;
  undefined4 auStack_828 [512];
  
  uVar5 = param_2 & 0xff;
  *DAT_00462378 = param_1 | *DAT_00462378;
  piVar1 = DAT_00462388;
  if ((param_3 == (int *)0x0) || (*param_3 == 0)) {
    MMI_diag_stub_00298B64();
    MMI_diag_stub_00298B64();
    uVar2 = 0;
  }
  else {
    if (((param_1 & 2) != 0) && ((*DAT_00462384 & 2) != 0)) {
      if (*DAT_00462388 == *param_3) {
        uVar4 = uVar5;
        if (uVar5 == 0) {
          uVar4 = 0xfffffff;
        }
        *DAT_0046238c = uVar4;
      }
      else {
        *DAT_00462388 = *param_3;
        piVar1[1] = param_3[1];
        uVar4 = uVar5;
        if (uVar5 == 0) {
          uVar4 = 0xfffffff;
        }
        *DAT_0046238c = uVar4;
        FUN_0046438c();
        piVar1 = DAT_00462390;
        *DAT_00462390 = 0;
        do {
          iVar3 = FUN_00464d9c();
          if (iVar3 * 0x10000 < 1) break;
          uVar4 = FUN_00464da8((int)auStack_828,0x800);
          FUN_00298c6c((undefined4 *)(*DAT_00462394 + *piVar1),auStack_828,uVar4);
          iVar3 = *piVar1;
          *piVar1 = uVar4 + iVar3;
        } while (uVar4 + iVar3 + 0x800 < 0x100001);
      }
      *DAT_00462398 = *DAT_00462390;
      *DAT_0046239c = *DAT_00462394;
      *DAT_004623a0 = 1;
    }
    if (((param_1 & 1) != 0) && ((*DAT_00462384 & 1) != 0)) {
      local_848[0] = 0;
      local_848[1] = 0;
      local_848[2] = 0;
      uStack_83c = 0;
      uStack_838 = 0;
      uStack_834 = 0;
      local_830 = 0;
      local_82c = 0;
      uVar4 = FUN_00461510((undefined4 *)*param_3,local_848);
      if ((uVar4 & 0xff) != 0) {
        MMI_diag_stub_00298B64();
      }
      if ((local_848[0] != 8000) || (cStack_83b != '\b')) {
        return 0xffffffff;
      }
      local_830 = *param_3 + local_830;
      uVar2 = *(undefined4 *)(local_830 + -4);
      *DAT_004623a8 = local_830;
      *DAT_004623ac = local_830;
      *DAT_004623b0 = uVar2;
      if (uVar5 == 0) {
        uVar5 = 0xfffffff;
      }
      *DAT_004623b4 = uVar5;
      *DAT_004623b8 = 0xb;
    }
    uVar2 = 1;
  }
  return uVar2;
}

