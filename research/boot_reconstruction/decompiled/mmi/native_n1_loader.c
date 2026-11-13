/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00460120; original_symbol=native_n1_loader */

undefined4 native_n1_loader(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uStack_80;
  uint local_7c;
  uint local_78;
  uint uStack_74;
  undefined2 local_70;
  
  local_7c = *DAT_00460260;
  local_78 = DAT_00460260[1];
  uStack_74 = DAT_00460260[2];
  local_70 = (undefined2)DAT_00460260[3];
  FUN_00345dec(&local_7c,DAT_00460264);
  native_cache_prepare();
  iVar1 = native_read_file(0xa00000,(int)&local_7c,0x34000,&uStack_80);
  if (iVar1 != 0) {
    local_7c = *DAT_00460268;
    local_78 = CONCAT31(local_78._1_3_,(char)DAT_00460268[1]);
    FUN_00345dec(&local_7c,DAT_00460264);
    iVar1 = native_read_file(0xa00000,(int)&local_7c,0x34000,&uStack_80);
    if (iVar1 != 0) {
      return 0xfffffffe;
    }
  }
  iVar2 = native_read_file(0xa34000,param_1,*DAT_0046026c - 0x34000,&uStack_80);
  iVar1 = DAT_00460270;
  if (iVar2 == 0) {
    *(undefined4 *)(DAT_00460270 + 8) = 0xa34000;
    *(undefined4 *)(iVar1 + 0x10) = 0xb34000;
    *(int *)(iVar1 + 0x14) = *DAT_0046026c + -0x134000;
    *(uint *)(iVar1 + 0xc) = uStack_80;
    *(int *)(iVar1 + 4) = param_1;
    *(undefined4 *)(iVar1 + 0x18) = 1;
    uVar4 = FUN_00464158(*DAT_00460274,&SUB_00a00000,&local_7c,0x34000,uStack_80);
    *(char *)(iVar1 + 1) = (char)uVar4;
    uVar3 = (*(code *)&SUB_00a00000)(iVar1,DAT_00460278,DAT_0046027c);
  }
  else {
    uVar3 = 0xfffffffe;
  }
  return uVar3;
}

