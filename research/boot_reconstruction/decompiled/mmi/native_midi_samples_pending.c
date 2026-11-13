/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00464d9c; original_symbol=native_midi_samples_pending */

int native_midi_samples_pending(void)

{
  return (int)*DAT_00464d64;
}

