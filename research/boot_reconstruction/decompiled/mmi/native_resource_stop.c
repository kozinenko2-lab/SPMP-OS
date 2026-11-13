/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004624d4; original_symbol=native_resource_stop */

void native_resource_stop(byte param_1)

{
  *DAT_00462590 = *DAT_00462590 & ~param_1;
  MMI_diag_stub_00298B64();
  if (((param_1 & 2) != 0) && ((*DAT_00462598 & 2) != 0)) {
    if (*DAT_0046259c == '\x03') {
      return;
    }
    *DAT_0046259c = '\x03';
    *DAT_004625a0 = 0;
    *DAT_004625a4 = 0;
  }
  if ((param_1 & 1) == 0) {
    return;
  }
  if ((*DAT_00462598 & 1) == 0) {
    return;
  }
  MMI_diag_stub_00298B64();
  if (*DAT_004625ac == '\r') {
    return;
  }
  *DAT_004625ac = '\r';
  *DAT_004625b0 = 0;
  *DAT_004625b4 = 0;
  return;
}

