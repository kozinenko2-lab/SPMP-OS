/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004ae52c; original_symbol=ecos_context_restore */

void ecos_context_restore(undefined4 *param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)*param_1;
                    /* WARNING: Could not recover jumptable at 0x004ae548. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)puVar1[0xe])(*puVar1,puVar1[1],puVar1[2],puVar1[3]);
  return;
}

