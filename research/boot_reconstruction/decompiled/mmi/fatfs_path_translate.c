/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002834dc; original_symbol=fatfs_path_translate */

void fatfs_path_translate(int *param_1,uint *param_2)

{
  if ((*DAT_0028358c != (char)*param_2) || (DAT_0028358c[1] != *(char *)((int)param_2 + 1))) {
    *param_1 = (int)param_2;
    return;
  }
  if ((((*(byte *)((int)param_2 + 2) < (byte)DAT_0028358c[2]) ||
       ((byte)DAT_0028358c[5] < *(byte *)((int)param_2 + 2))) ||
      (*(byte *)((int)param_2 + 3) < (byte)DAT_0028358c[3])) ||
     ((byte)DAT_0028358c[6] < *(byte *)((int)param_2 + 3))) {
    *param_1 = (int)param_2;
    return;
  }
  if (((char)param_2[1] == '\0') || (DAT_0028358c[4] == (char)param_2[1])) {
    *(undefined1 *)*param_1 = 0x2f;
    FUN_00346058((uint *)(*param_1 + 1),param_2);
    return;
  }
  *param_1 = (int)param_2;
  return;
}

