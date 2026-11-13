/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d3254; original_symbol=native_slot_73 */

int native_slot_73(void)

{
  undefined8 uVar1;
  
  uVar1 = ecos_current_ticks();
  return ((int)uVar1 - *DAT_002d3298) * 10;
}

