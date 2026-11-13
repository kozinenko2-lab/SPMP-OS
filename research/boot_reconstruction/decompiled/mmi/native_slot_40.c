/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de944; original_symbol=native_slot_40 */

undefined4 native_slot_40(undefined2 *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_002deaac;
  *DAT_002deaac = *param_1;
  puVar1[1] = param_1[1];
  switch(param_2) {
  case 0:
    FUN_00312dbc(DAT_002deab0);
    FUN_00312d98(DAT_002deab0);
    *DAT_002deab4 = 0;
    break;
  case 1:
    FUN_00312da8(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,2);
    *DAT_002deab4 = 2;
    break;
  case 2:
    FUN_00312da8(DAT_002deab0);
    FUN_00312d98(DAT_002deab0);
    *DAT_002deab4 = 0;
    break;
  case 3:
    FUN_00312dbc(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,2);
    *DAT_002deab4 = 2;
    break;
  case 4:
    FUN_00312da8(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,3);
    *DAT_002deab4 = 3;
    break;
  case 5:
    FUN_00312dbc(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,1);
    *DAT_002deab4 = 1;
    break;
  case 6:
    FUN_00312dbc(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,3);
    *DAT_002deab4 = 3;
    break;
  case 7:
    FUN_00312da8(DAT_002deab0);
    FUN_00312d80(DAT_002deab0,1);
    *DAT_002deab4 = 1;
  }
  return 0;
}

