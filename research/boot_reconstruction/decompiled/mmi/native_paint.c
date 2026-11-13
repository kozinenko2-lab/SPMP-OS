/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002dd20c; original_symbol=native_paint */

undefined4 native_paint(void)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  undefined2 auStack_28 [2];
  ushort local_24;
  ushort local_22;
  
  puVar5 = DAT_002dd2ac;
  iVar2 = (*(code *)*DAT_002dd2ac)();
  puVar1 = DAT_002dd2b0;
  iVar3 = (*(code *)*DAT_002dd2b0)();
  if (iVar2 != iVar3) {
    (*(code *)*DAT_002dd2b4)();
    native_get_display_rect(auStack_28);
    puVar4 = (undefined4 *)(*(code *)*puVar1)();
    puVar5 = (undefined4 *)(*(code *)*puVar5)();
    FUN_00298c6c(puVar4,puVar5,(uint)local_22 * (uint)local_24 * 2);
    FUN_0029e450();
    uVar6 = (*(code *)*puVar1)();
    *(undefined4 *)(DAT_002dd2b8 + 4) = uVar6;
  }
  return 0;
}

