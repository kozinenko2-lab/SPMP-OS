# NES synthetic functional tests

1. `nes_cpu_ppu_smoke.nes`: original NROM with 16 KiB PRG, CHR-RAM; CPU 6502 writes `0xA5` to $6000, sets palette, fills pattern 0, enables background, JMP loop. Test expects 256×240 RGB565 with distinct pixel0 and pixel1, save callback once per dirty battery game.
2. `nes_uxrom_smoke.nes`: UxROM fixed $C000–$FFFF bank executes LDA/STA to select lower PRG bank, reads marker `0x42` from $8000 and saves to $6001.
3. `nes_cnrom_smoke.nes`: bank switch to second 8 KiB CHR bank and verify pattern render at top-left.
4. Rejects truncated files, unknown mappers, NES2 format and invalid header before frame execution.
5. `nes_harness.c`: verifies screenshot callback, SRAM/PRG bank values, save load and dirty save behavior. Run separately under ASan and UBSan.

This is *not* a generic NES CPU/PPU accuracy suite, and does not measure commercial game compatibility, frame pacing or audio.
