/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=00280550; original_symbol=mmi_demo_window_proc */

int mmi_demo_window_proc(int *param_1,uint param_2,int param_3,int *param_4)

{
  int iVar1;
  
  if (param_2 == 2) {
    FUN_002804f8();
    return 0;
  }
  if (param_2 == 0xf) {
    FUN_0028051c(param_1);
    return 0;
  }
  iVar1 = FUN_003c4038(param_1,param_2,param_3,param_4);
  return iVar1;
}

