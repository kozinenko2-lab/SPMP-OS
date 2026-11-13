/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00339d18; original_symbol=ecos_current_ticks */

undefined8 ecos_current_ticks(void)

{
  return *(undefined8 *)(*DAT_00339d2c + 0x28);
}

