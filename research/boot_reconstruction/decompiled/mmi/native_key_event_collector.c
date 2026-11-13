/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002d2e6c; original_symbol=native_key_event_collector */

uint native_key_event_collector(void)

{
  ushort uVar1;
  uint *puVar2;
  ushort *puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  
  iVar4 = FUN_0045ffbc();
  if (iVar4 == 5) {
    iVar4 = DAT_002d2f44;
  }
  else {
    iVar4 = DAT_002d2f48;
  }
  uVar7 = 0;
  iVar5 = FUN_002d8b78();
  puVar3 = DAT_002d2f54;
  puVar2 = DAT_002d2f4c;
  if (iVar5 != 0) {
    *DAT_002d2f4c = 0;
    *DAT_002d2f50 = 0;
    return 0;
  }
  uVar6 = *DAT_002d2f4c;
  if (uVar6 != 0) {
    uVar1 = *DAT_002d2f54;
    *DAT_002d2f54 = uVar1 + 1;
    if (2 < uVar1) {
      *puVar2 = 0;
      *puVar3 = 0;
    }
    return uVar6;
  }
  if (*DAT_002d2f50 != 0) {
    uVar6 = 0;
    do {
      if ((*DAT_002d2f50 & 1 << (uVar6 & 0xff)) != 0) {
        uVar7 = uVar7 | *(uint *)(iVar4 + uVar6 * 4 + 4);
      }
      uVar6 = uVar6 + 1 & 0xffff;
    } while (uVar6 < 0x10);
    uVar7 = uVar7 & 0xfffff7ff;
  }
  return uVar7;
}

