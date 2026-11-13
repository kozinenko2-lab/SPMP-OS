/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002de06c; original_symbol=native_set_framebuffer */

undefined4 native_set_framebuffer(ushort param_1,ushort param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  
  if (0x3ff < param_1) {
    return 0x70000401;
  }
  if (0x3ff < param_2) {
    return 0x70000401;
  }
  iVar1 = (*(code *)*DAT_002de114)((uint)param_1 << 0x10);
  iVar2 = (*(code *)*DAT_002de118)();
  iVar3 = (*(code *)*DAT_002de11c)();
  puVar5 = DAT_002de120;
  iVar4 = (*(code *)*DAT_002de120)();
  if (iVar3 != iVar4) {
    puVar5 = (undefined4 *)(*(code *)*puVar5)();
    FUN_00298d34(puVar5,0,iVar2 * iVar1 * 2);
  }
  FUN_0029e450();
  return 0;
}

