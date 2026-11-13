/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4be8; original_symbol=lcd_bind_pool_buffers */

void lcd_bind_pool_buffers(void)

{
  int iVar1;
  ushort uVar2;
  ushort uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  
  lcd_pool_buffer_get(1,0);
  lcd_pool_buffer_get(1,1);
  lcd_pool_buffer_get(0,0);
  lcd_pool_buffer_get(0,1);
  MMI_diag_stub_00298B64();
  iVar1 = DAT_002a4d48;
  uVar2 = (**(code **)(DAT_002a4d48 + 8))();
  uVar3 = (**(code **)(iVar1 + 0xc))();
  iVar6 = lcd_pool_buffer_get(1,0);
  iVar7 = lcd_pool_buffer_get(1,1);
  iVar8 = lcd_pool_buffer_get(0,0);
  iVar9 = lcd_pool_buffer_get(0,1);
  lcd_initialize(uVar2,uVar3,iVar6,iVar7,iVar8,iVar9,1,0);
  MMI_diag_stub_00298B64();
  MMI_diag_stub_00298B64();
  uVar4 = (**(code **)(iVar1 + 8))();
  uVar5 = (**(code **)(iVar1 + 0xc))();
  uVar10 = lcd_pool_buffer_get(0,0);
  uVar11 = lcd_pool_buffer_get(0,1);
  lcd_secondary_initialize(uVar4,uVar5,100,uVar10,uVar11,0x10000000,0);
  MMI_diag_stub_00298B64();
  return;
}

