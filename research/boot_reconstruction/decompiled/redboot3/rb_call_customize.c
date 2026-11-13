/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00036844; original_symbol=rb_call_customize */

void rb_call_customize(void)

{
  if ((code *)*DAT_00036860 == (code *)0x0) {
    return;
  }
  (*(code *)*DAT_00036860)();
  return;
}

