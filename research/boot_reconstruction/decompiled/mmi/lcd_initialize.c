/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002a4288; original_symbol=lcd_initialize */

void lcd_initialize(ushort param_1,ushort param_2,int param_3,int param_4,int param_5,int param_6,
                   uint param_7,undefined4 param_8)

{
  int *piVar1;
  undefined4 *puVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r3;
  uint uVar3;
  
  piVar1 = DAT_002a43d4;
  if (*DAT_002a43d4 == 0) {
    FUN_0033a020(DAT_002a43d8);
    FUN_0033a020(DAT_002a43dc);
    ecos_flag_init(DAT_002a43e0);
    puVar2 = DAT_002a43e8;
    FUN_00339c10(0x17,10,0,DAT_002a43f0,DAT_002a43e4,DAT_002a43e8,DAT_002a43ec);
    FUN_00339c74((uint *)*puVar2,extraout_r1,extraout_r2);
    FUN_00339cd8(0x17,extraout_r1_00,extraout_r2_00,extraout_r3);
    *piVar1 = 1;
  }
  uVar3 = param_7 & 0xffffff00;
  switch(param_8) {
  case 1:
    uVar3 = uVar3 | 0x10002;
    break;
  default:
    uVar3 = uVar3 | 0x10001;
    break;
  case 3:
    uVar3 = uVar3 | 0x10004;
    break;
  case 4:
    uVar3 = uVar3 | 0x10010;
    break;
  case 5:
    uVar3 = uVar3 | 0x10020;
    break;
  case 7:
    uVar3 = uVar3 | 0x10040;
    break;
  case 8:
    uVar3 = uVar3 | 0x10080;
  }
  lcd_state_initialize(param_1,param_2,param_3,param_5,uVar3);
  if (param_6 == 0) {
    param_6 = param_5;
  }
  if (param_4 == 0) {
    param_4 = param_3;
  }
  lcd_set_render_buffer(param_4);
  lcd_secondary_set_render_buffer(param_6);
  lcd_select_main_span_ops();
  return;
}

