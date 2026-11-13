/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=003bf470; original_symbol=gui_get_message */

bool gui_get_message(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  bool bVar1;
  
  do {
    bVar1 = FUN_003bf198(param_1,param_2,param_3,param_4,1);
  } while (!bVar1);
  return param_1[3] != 0x12;
}

