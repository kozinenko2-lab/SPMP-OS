/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e37c0; original_symbol=mmi_game_pool_info */

int * mmi_game_pool_info(int *param_1)

{
  undefined4 local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  undefined4 local_2c;
  int local_28;
  undefined1 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined1 local_18;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  local_40 = 0xffffffff;
  local_3c = -1;
  local_38 = -1;
  local_34 = -1;
  local_30 = -1;
  local_2c = 0xffffffff;
  local_28 = -1;
  local_24 = 0xff;
  local_20 = 0xffffffff;
  local_1c = 0xffffffff;
  local_18 = 0xff;
  ecos_pool_info_wrapper(DAT_002e3894,0x5e,&local_40);
  if (0 < local_3c) {
    *param_1 = local_3c;
  }
  if (0 < local_38) {
    param_1[1] = local_38;
  }
  if (0 < local_34) {
    param_1[7] = local_34;
  }
  if (0 < local_30) {
    param_1[8] = local_30;
  }
  if (0 < local_28) {
    param_1[10] = local_28;
  }
  return param_1;
}

