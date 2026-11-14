/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00299a14; original_symbol=ecos_errno_location */

int ecos_errno_location(undefined4 param_1,uint param_2,undefined4 param_3)

{
  if ((*DAT_00299a58 & 4) != 0) {
    FUN_002815bc(DAT_00299a5c,param_2,param_3,*DAT_00299a58);
  }
  return *DAT_00299a54 + 0x88;
}

