/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00462448; original_symbol=native_resource_resume */

void native_resource_resume(uint param_1)

{
  MMI_diag_stub_00298B64();
  if (((param_1 & 2) != 0) && ((*DAT_004624c0 & 2) != 0)) {
    MMI_diag_stub_00298B64();
    *DAT_004624c8 = 1;
  }
  if ((param_1 & 1) == 0) {
    return;
  }
  if ((*DAT_004624c0 & 1) == 0) {
    return;
  }
  MMI_diag_stub_00298B64();
  *DAT_004624d0 = 0xb;
  return;
}

