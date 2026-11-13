/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002805f8; original_symbol=mmi_application_main */

undefined4 mmi_application_main(undefined4 param_1,uint param_2)

{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  uint extraout_r1;
  uint uStack_190;
  undefined1 uStack_18c;
  undefined4 auStack_90 [16];
  int aiStack_50 [4];
  undefined4 uStack_40;
  
  mmi_register_demo_class(param_1,param_2);
  piVar2 = FUN_003bf6cc(0,DAT_002807b8,DAT_002807bc,0x10040000,0,0,0x140,0xf0,0,0,0,0);
  piVar3 = (int *)FUN_003c0dac((int)piVar2);
  FUN_0041e844(piVar3,0x140,0xf0);
  FUN_003c0de4(piVar2,piVar3);
  FUN_003dbe78();
  FUN_003dc0a8();
  FUN_003d9f04();
  FUN_003d9fcc();
  FUN_003dbe4c(1,extraout_r1);
  uStack_190 = *DAT_002807c0;
  uStack_18c = (undefined1)DAT_002807c0[1];
  FUN_00345dec(&uStack_190,DAT_002807c4);
  piVar3 = FUN_003c26a4(0,(int)&uStack_190,0,0,0,10);
  piVar4 = FUN_003c0f18(piVar2,auStack_90);
  piVar5 = FUN_003c21d0(piVar4);
  piVar6 = (int *)FUN_003c1c48(piVar5,piVar3);
  FUN_003c22f4(piVar4,0,0,0x140,0xf0,piVar5,0,0,0);
  FUN_003c2860(1);
  FUN_003c1058((int)piVar2,auStack_90);
  FUN_003c2860(0);
  FUN_003c1c48(piVar5,piVar6);
  FUN_003c1d4c(piVar3);
  FUN_003c0e90(piVar5);
  FUN_00447398();
  mmi_launch_shell(param_1,(int)piVar2);
  while (bVar1 = gui_get_message(aiStack_50,0,0,0), bVar1) {
    gui_translate_message((int)aiStack_50);
    gui_dispatch_message((int)aiStack_50);
  }
  return uStack_40;
}

