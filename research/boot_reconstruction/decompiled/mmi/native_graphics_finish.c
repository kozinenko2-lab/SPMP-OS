/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e479c; original_symbol=native_graphics_finish */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 native_graphics_finish(void)

{
  uint uVar1;
  
  uVar1 = 0;
  do {
    if ((_DAT_92006010 & 0x3b) == 0x10) {
      return 0;
    }
    FUN_00299d80(100);
    uVar1 = uVar1 + 1;
  } while (uVar1 < 300);
  return 1;
}

