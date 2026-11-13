/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00036ab4; original_symbol=rb_fast_load */

int rb_fast_load(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  undefined4 extraout_r2;
  undefined4 extraout_r2_00;
  undefined4 extraout_r3;
  undefined4 extraout_r3_00;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iStack_68;
  int local_64;
  undefined4 local_60;
  undefined1 auStack_50 [16];
  uint local_40;
  uint local_3c;
  int local_38;
  int iStack_2c;
  int local_28;
  
  iVar1 = FUN_0003f860(param_1,1);
  if (iVar1 < 0) {
    FUN_0004563c(DAT_00036be4,extraout_r1,extraout_r2,extraout_r3);
    iVar5 = -1;
  }
  else {
    FUN_0003fbc0(iVar1,&iStack_2c,0x10);
    iVar5 = FUN_00036838(&iStack_2c,DAT_00036be8);
    if (iVar5 == 0) {
      if (local_28 < 0x11) {
        iVar5 = 0;
      }
      else {
        iVar3 = 0x10;
        iVar5 = 0;
        do {
          FUN_0003fbc0(iVar1,auStack_50,0x24);
          iVar3 = iVar3 + 0x24;
          if (((local_40 & 0xf0000) == 0) && (local_38 != -1)) {
            iVar5 = local_38;
          }
          if (local_3c != 0) {
            uVar4 = 0;
            do {
              FUN_0003fbc0(iVar1,&iStack_68,0x18);
              iVar2 = FUN_00036838(&iStack_68,DAT_00036bf0);
              if (iVar2 == 0) {
                iVar2 = local_64 + -0x18;
                FUN_0003fbc0(iVar1,local_60,iVar2);
                iVar3 = iVar3 + 0x18 + iVar2;
              }
              uVar4 = uVar4 + 1;
            } while (uVar4 < local_3c);
          }
        } while (iVar3 < local_28);
      }
      FUN_0003fbe4(iVar1);
    }
    else {
      FUN_0004563c(DAT_00036bec,extraout_r1_00,extraout_r2_00,extraout_r3_00);
      iVar5 = -1;
    }
  }
  return iVar5;
}

