/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de878; original_symbol=native_slot_27 */

undefined4 native_slot_27(undefined2 *param_1)

{
  undefined2 *puVar1;
  undefined4 *puVar2;
  
  puVar1 = DAT_002de8b8;
  *DAT_002de8b8 = *param_1;
  puVar1[1] = param_1[1];
  puVar1[2] = param_1[2];
  puVar1[3] = param_1[3];
  puVar2 = DAT_002de8bc;
  *DAT_002de8bc = puVar1;
  *(undefined2 *)(puVar2 + 1) = 8;
  *(undefined2 *)((int)puVar2 + 6) = 8;
  return 0;
}

