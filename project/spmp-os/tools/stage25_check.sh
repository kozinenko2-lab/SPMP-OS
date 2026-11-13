#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
make -j4 stage24-gpsp build/stage24_gba_harness
./build/stage24_gba_harness tests/roms/stage24_gba_test.gba
python3 -m unittest discover -s tests -p 'test_stage25_gba_cpu.py' -v
(cd third_party/gpsp && clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -c bios_data.S -o ../../build/stage25_open_bios_armv5.o)
python3 - <<'PYARM'
from pathlib import Path
b=Path('build/stage25_open_bios_armv5.o').read_bytes()
assert b[:4] == b'\x7fELF' and b[4] == 1 and int.from_bytes(b[18:20], 'little') == 40
print('ARMv5 open BIOS data object: ELF32 EM_ARM OK')
PYARM
