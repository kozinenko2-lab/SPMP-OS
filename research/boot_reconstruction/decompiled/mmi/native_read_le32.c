/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004614ec; original_symbol=native_read_le32 */

int native_read_le32(byte *param_1)

{
  return (uint)param_1[2] * 0x10000 + (uint)param_1[3] * 0x1000000 + (uint)param_1[1] * 0x100 +
         (uint)*param_1;
}

