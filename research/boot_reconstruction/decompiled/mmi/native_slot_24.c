/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de7d0; original_symbol=native_slot_24 */

undefined4 native_slot_24(undefined2 *param_1)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_002de7fc;
  param_1[1] = DAT_002de7fc[1];
  param_1[3] = puVar1[3];
  *param_1 = *puVar1;
  param_1[2] = puVar1[2];
  return 0;
}

