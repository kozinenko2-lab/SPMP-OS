/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=004600b0; original_symbol=native_cache_prepare */

undefined8 native_cache_prepare(void)

{
  char in_ZR;
  char in_CY;
  char in_OV;
  byte in_Q;
  undefined4 in_cr5;
  undefined4 in_cr8;
  
  do {
    coproc_movefrom_Data_cache_operations(3);
  } while ((((byte)(in_ZR << 3 | in_CY << 2 | in_OV << 1 | in_Q) & 0x1f) >> 3 & 1) == 0);
  coproc_moveto_Data_Synchronization(0);
  coproc_moveto_Invalidate_Entire_Data_cache(0);
  coproc_moveto_Invalidate_unified_TLB_unlocked(0);
  coproc_moveto_Invalidate_Entire_Instruction(0);
  coprocessor_moveto(0xf,0,0,0,in_cr8,in_cr5);
  return 0;
}

