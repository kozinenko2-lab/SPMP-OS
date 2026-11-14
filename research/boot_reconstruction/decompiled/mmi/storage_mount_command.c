/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004382b4; original_symbol=storage_mount_command */

void storage_mount_command(int param_1,undefined4 param_2,undefined4 param_3)

{
  uint local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  local_2c = 0;
  local_28 = 0;
  local_24 = 0;
  FUN_0033e2e0(DAT_00438374,DAT_00438378,*(uint *)(DAT_0043837c + param_1 * 8),param_2);
  local_2c = DAT_00438374;
  if (param_1 == 1) {
    FUN_0033e2e0(DAT_00438374,DAT_00438388,*(uint *)(DAT_0043837c + 8),param_3);
    local_28 = DAT_0043838c;
  }
  else if (param_1 == 0) {
    local_28 = DAT_00438380;
  }
  else if (param_1 == 2) {
    local_28 = DAT_00438384;
  }
  local_24 = DAT_00438390;
  FUN_002d6d60(0x601,&local_2c,0xc);
  return;
}

