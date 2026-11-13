/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002df000; original_symbol=native_init_graph */

undefined4 native_init_graph(void)

{
  undefined2 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  ushort *puVar5;
  
  iVar2 = native_fixed_width();
  iVar4 = DAT_002df1c8;
  iVar3 = (**(code **)(DAT_002df1c8 + 8))();
  if (iVar2 == iVar3) {
    iVar2 = native_fixed_height();
    iVar3 = (**(code **)(iVar4 + 0xc))();
    iVar4 = DAT_002df1c8;
    if (iVar2 == iVar3) {
      *DAT_002df1e0 = *(undefined4 *)(DAT_002df1c8 + 0x10);
      *DAT_002df1e8 = *(undefined4 *)(iVar4 + 0x24);
      *DAT_002df1f0 = *(undefined4 *)(iVar4 + 8);
      *DAT_002df1f8 = *(undefined4 *)(iVar4 + 0xc);
      *DAT_002df200 = *(undefined4 *)(iVar4 + 0x18);
      goto LAB_002df14c;
    }
  }
  iVar4 = FUN_0031458c();
  if (iVar4 == 1) {
    *DAT_002df1d0 = *DAT_002df1cc;
    *DAT_002df1d8 = *DAT_002df1d4;
    *DAT_002df1e0 = DAT_002df1dc;
    *DAT_002df1e8 = DAT_002df1e4;
    *DAT_002df1f0 = DAT_002df1ec;
    *DAT_002df1f8 = DAT_002df1f4;
    *DAT_002df200 = DAT_002df1fc;
  }
  else {
    *DAT_002df1d0 = *DAT_002df1cc;
    *DAT_002df1d8 = *DAT_002df1d4;
    *DAT_002df1e0 = DAT_002df1dc;
    *DAT_002df1e8 = DAT_002df1e4;
    *DAT_002df1f0 = DAT_002df1ec;
    *DAT_002df1f8 = DAT_002df1f4;
    *DAT_002df200 = DAT_002df1fc;
  }
LAB_002df14c:
  FUN_002de1ac();
  puVar5 = (ushort *)FUN_00313430();
  FUN_002dd110(puVar5);
  FUN_00312fb0(DAT_002df204);
  FUN_00312fb0(DAT_002df208);
  puVar1 = DAT_002df20c;
  *DAT_002df20c = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  puVar1 = DAT_002df210;
  *DAT_002df210 = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  puVar1 = DAT_002df214;
  *DAT_002df214 = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  puVar1[4] = 0;
  puVar1[5] = 0;
  FUN_00312ec8(DAT_002df204);
  FUN_00312ec8(DAT_002df208);
  return 0;
}

