/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00292720; original_symbol=pcm_driver_configure */

undefined4 pcm_driver_configure(undefined4 *param_1,undefined4 *param_2)

{
  uint *puVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint extraout_r1;
  uint extraout_r1_00;
  uint extraout_r1_01;
  uint extraout_r1_02;
  undefined4 extraout_r1_03;
  uint extraout_r1_04;
  int *extraout_r1_05;
  int *extraout_r1_06;
  int *piVar5;
  uint extraout_r1_07;
  uint extraout_r1_08;
  undefined4 extraout_r2;
  int extraout_r2_00;
  int extraout_r2_01;
  undefined8 uVar6;
  int local_24;
  int local_20;
  
  local_20 = 0;
  local_24 = 0;
  if ((*(uint *)*DAT_00292aa8 & 1) != 0) {
    MMI_diag_stub_00298B64();
  }
  if (param_2 == (undefined4 *)0x0 || param_1 == (undefined4 *)0x0) {
    uVar2 = 0x10000000;
  }
  else {
    FUN_00292320(DAT_00292ab0);
    iVar3 = FUN_00292378(DAT_00292ab0,&local_20);
    if (iVar3 == 0) {
      iVar3 = FUN_002924b0(DAT_00292ab0,&local_24);
      piVar5 = DAT_00292ab4;
      if (iVar3 == 0) {
        if ((local_20 == 0) && (local_24 != 0)) {
          MMI_diag_stub_00298B64();
          FUN_002922ac(param_1);
          uVar2 = FUN_0029224c(param_2);
          FUN_002922f4(uVar2,extraout_r1_07);
          *DAT_00292af0 = 0;
          *DAT_00292af4 = 0;
          *DAT_00292afc = 0;
          *DAT_00292b00 = 0;
          FUN_0029251c(DAT_00292ab0,extraout_r1_08);
        }
        else {
          MMI_diag_stub_00298B64();
          FUN_00292584(DAT_00292ab0,1);
          FUN_002926bc(DAT_00292ab0,0);
          FUN_0029244c(DAT_00292ab0,0);
          uVar4 = extraout_r1;
          if (*piVar5 == 0) {
            if ((*(uint *)*DAT_00292aa8 & 1) != 0) {
              MMI_diag_stub_00298B64();
              uVar4 = extraout_r1_00;
            }
            ecos_thread_delete_wrapper(*DAT_00292ac4,uVar4);
            uVar4 = extraout_r1_01;
          }
          piVar5 = DAT_00292ac8;
          if (*DAT_00292ac8 == 1) {
            ecos_thread_delete_wrapper(*DAT_00292acc,uVar4);
            *piVar5 = 0;
          }
          if (2 < *DAT_00292ab4 + 1U) {
            FUN_00293308();
          }
          FUN_0033a020(DAT_00292ad0);
          ecos_flag_init(DAT_00292ad4);
          ecos_flag_init(DAT_00292ad8);
          *DAT_00292adc = 0;
          uVar2 = FUN_002922ac(param_1);
          FUN_002922f4(uVar2,extraout_r1_02);
          *DAT_00292ae0 = 0;
          *DAT_00292ae4 = 0;
          *DAT_00292ae8 = 0;
          *DAT_00292aec = 0;
          *DAT_00292af0 = 0;
          *DAT_00292af4 = 0;
          *DAT_00292af8 = 0;
          *DAT_00292afc = 0;
          *DAT_00292b00 = 0;
          *DAT_00292b04 = 0;
          *DAT_00292b08 = 0;
          *DAT_00292b0c = 1;
          FUN_00298d34(DAT_00292b10,0,0x4000);
          *DAT_00292b14 = 0;
          *DAT_00292b18 = 0;
          uVar2 = FUN_0029224c(param_2);
          uVar6 = CONCAT44(extraout_r1_03,uVar2);
          if (*DAT_00292b1c < 1) {
            uVar6 = pcm_stage_292198();
            *DAT_00292b1c = 0;
          }
          else {
            *DAT_00292b1c = 0;
          }
          uVar4 = pcm_dma_channel_allocate((int)uVar6,(uint)((ulonglong)uVar6 >> 0x20));
          *DAT_00292b20 = uVar4;
          if ((*(uint *)*DAT_00292aa8 & 1) != 0) {
            MMI_diag_stub_00298B64();
          }
          pcm_dma_attach_handlers(*DAT_00292b20,DAT_00292b28,DAT_00292b2c);
          iVar3 = *DAT_00292ac8;
          piVar5 = DAT_00292ac8;
          if (iVar3 == 0) {
            *DAT_00292ac8 = 1;
            puVar1 = DAT_00292acc;
            ecos_thread_create_wrapper
                      (0x16,DAT_00292b38,0,DAT_00292b3c,DAT_00292b30,0x1000,DAT_00292acc,
                       DAT_00292b34);
            ecos_thread_resume_wrapper((int *)*puVar1,extraout_r1_04,extraout_r2);
            piVar5 = extraout_r1_05;
            iVar3 = extraout_r2_00;
          }
          if (*DAT_00292b40 == 0) {
            if ((*(uint *)*DAT_00292aa8 & 1) != 0) {
              MMI_diag_stub_00298B64();
            }
            *DAT_00292b40 = 1;
            ecos_thread_create_wrapper
                      (5,DAT_00292b50,0,DAT_00292b54,DAT_00292b48,0x2000,DAT_00292ac4,DAT_00292b4c);
            piVar5 = extraout_r1_06;
            iVar3 = extraout_r2_01;
          }
          ecos_thread_resume_wrapper((int *)*DAT_00292ac4,(uint)piVar5,iVar3);
          *DAT_00292ab4 = 1;
        }
        uVar2 = 1;
      }
      else {
        uVar2 = 0x10000000;
      }
    }
    else {
      uVar2 = 0x10000000;
    }
  }
  return uVar2;
}

