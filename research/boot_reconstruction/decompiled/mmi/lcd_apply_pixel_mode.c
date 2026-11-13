/* Ghidra pseudocode; not recovered buildable OEM source. */
/* entry=002e796c; original_symbol=lcd_apply_pixel_mode */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void lcd_apply_pixel_mode(void)

{
  if ((*DAT_002e7b3c & 1) == 0) {
    if ((*DAT_002e7b3c & 2) == 0) {
      if ((*DAT_002e7b3c & 4) == 0) {
        if ((*DAT_002e7b3c & 0x10) == 0) {
          if ((*DAT_002e7b3c & 0x20) == 0) {
            if ((*DAT_002e7b3c & 0x40) == 0) {
              if ((*DAT_002e7b3c & 0x80) != 0) {
                _DAT_93000028 = 3;
                _DAT_93000068 = 0x80108010;
                _DAT_9300007c = 0x80108010;
              }
            }
            else {
              _DAT_93000028 = 2;
              _DAT_93000068 = 0x80108010;
              _DAT_9300007c = 0x80108010;
            }
          }
          else {
            _DAT_93000028 = 0x10003;
            _DAT_93000068 = 0x100010;
            _DAT_9300007c = 0x100010;
          }
        }
        else {
          _DAT_93000028 = 0x10002;
          _DAT_93000068 = 0x100010;
          _DAT_9300007c = 0x100010;
        }
      }
      else {
        _DAT_93000028 = 4;
        _DAT_93000068 = 0;
        _DAT_9300007c = 0;
      }
    }
    else {
      _DAT_93000028 = 1;
      _DAT_93000068 = 0;
      _DAT_9300007c = 0;
    }
  }
  else {
    _DAT_93000028 = 0;
    _DAT_93000068 = 0;
    _DAT_9300007c = 0;
  }
  if (*DAT_002e7b40 != 0) {
    _DAT_93000078 = 1;
  }
  return;
}

