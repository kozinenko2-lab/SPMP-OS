/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004617e4; original_symbol=native_load_ngame */

undefined4 native_load_ngame(undefined4 *param_1,int param_2,uint param_3,int param_4)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 local_2c;
  uint local_28 [3];
  
  local_28[2] = 0;
  local_28[0] = 0;
  local_2c = 0;
  local_28[1] = 0;
  if ((param_2 == 0 || param_1 == (undefined4 *)0x0) || (param_4 == 0)) {
    uVar2 = 2;
  }
  else {
    iVar3 = native_read_file((uint)param_1,param_2,param_3,local_28 + 2);
    if (iVar3 == 0) {
      iVar3 = native_parse_ngame_header
                        (param_1,local_28[2],(int *)local_28,&local_2c,(int *)(local_28 + 1));
      if (iVar3 == 0) {
        MMI_diag_stub_00298B64();
        MMI_diag_stub_00298B64();
        uVar1 = local_28[0];
        iVar3 = native_decode_prefix(param_1,local_28[0]);
        if (iVar3 == 0) {
          MMI_diag_stub_00298B64();
          iVar3 = FUN_00345f54((uint *)((int)param_1 + (uVar1 - 0x10)),DAT_00461904);
          uVar2 = 0;
          if (iVar3 != 0) {
            MMI_diag_stub_00298B64();
            uVar2 = 5;
          }
        }
        else {
          MMI_diag_stub_00298B64();
          uVar2 = 5;
        }
      }
      else {
        MMI_diag_stub_00298B64();
        uVar2 = 5;
      }
    }
    else {
      uVar2 = 2;
    }
  }
  return uVar2;
}

