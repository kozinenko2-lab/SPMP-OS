/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004623bc; original_symbol=native_slot_56 */

void native_slot_56(uint param_1)

{
  MMI_diag_stub_00298B64();
  if (((param_1 & 2) != 0) && ((*DAT_00462434 & 2) != 0)) {
    MMI_diag_stub_00298B64();
    *DAT_0046243c = 2;
  }
  if ((param_1 & 1) == 0) {
    return;
  }
  if ((*DAT_00462434 & 1) == 0) {
    return;
  }
  MMI_diag_stub_00298B64();
  *DAT_00462444 = 0xc;
  return;
}

