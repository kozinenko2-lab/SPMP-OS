/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00033ad0; original_symbol=rb_isp_status_bit */

uint rb_isp_status_bit(void)

{
  return *(uint *)(DAT_00033ae0 + 0xc) >> 0x1f;
}

