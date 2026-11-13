/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00313f40; original_symbol=gfx_blit_dispatch */

int gfx_blit_dispatch(uint *param_1,ushort param_2,ushort param_3,ushort param_4,ushort param_5,
                     uint *param_6,ushort param_7,ushort param_8,ushort param_9)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_00312b78();
  if (iVar1 == 0) {
    return 1;
  }
  iVar1 = FUN_00312bac(param_1,(int)param_6);
  if (iVar1 != 0) {
    return iVar1;
  }
  uVar2 = FUN_0029e450();
  FUN_00312b38((int)uVar2,(uint)((ulonglong)uVar2 >> 0x20));
  FUN_00312ff8(0,(ushort)param_6[1]);
  FUN_00312ff8(1,*(ushort *)((int)param_6 + 6));
  FUN_00312ff8(2,*(ushort *)((int)param_6 + 0x2a));
  FUN_00312ff8(3,param_7);
  FUN_00312ff8(4,param_8);
  FUN_00312ff8(5,(ushort)param_1[1]);
  FUN_00312ff8(6,*(ushort *)((int)param_1 + 6));
  FUN_00312ff8(7,*(ushort *)((int)param_1 + 0x2a));
  FUN_00312ff8(8,*(ushort *)((int)param_1 + 10));
  FUN_00312ff8(9,param_2);
  FUN_00312ff8(10,param_3);
  FUN_00312ff8(0xb,param_4);
  FUN_00312ff8(0xc,param_5);
  FUN_00312ff8(0xe,(ushort)param_1[9]);
  uVar2 = FUN_0031308c(param_1,param_6,0,param_9);
  FUN_00312b58((int)uVar2,(uint)((ulonglong)uVar2 >> 0x20));
  if (param_1[0xb] == 1) {
    native_graphics_finish();
  }
  return 0;
}

