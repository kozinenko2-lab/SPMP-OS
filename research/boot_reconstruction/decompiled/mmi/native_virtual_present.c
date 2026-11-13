/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002def60; original_symbol=native_virtual_present */

void native_virtual_present(void)

{
  uint *puVar1;
  uint *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined8 uVar6;
  
  puVar2 = DAT_002deff8;
  puVar1 = DAT_002deff4;
  uVar5 = *DAT_002deff4;
  *DAT_002deff4 = *DAT_002deff8;
  *puVar2 = uVar5;
  iVar3 = DAT_002deffc;
  uVar6 = (**(code **)(DAT_002deffc + 0x10))();
  blitter_command_reset((int)uVar6,(uint)((ulonglong)uVar6 >> 0x20));
  uVar5 = (**(code **)(iVar3 + 8))();
  uVar4 = (**(code **)(iVar3 + 0xc))();
  blitter_transfer(0,0x140,0xf0,0,uVar5,uVar4,(ushort *)0x0,*puVar1,0,0);
  blitter_control(1);
  (**(code **)(iVar3 + 0x18))();
  return;
}

