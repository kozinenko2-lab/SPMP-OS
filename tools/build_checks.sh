#!/bin/sh
# Safe host/ARM test builds; no device flashing.
set -eu
cd "$(dirname "$0")/.."
python3 tools/verify_integrity.py --quick
make -C project/spmp-os host
(cd project/spmp-os && ./build/spmp-os-host --cores)
make -C project/spmp-os arm-core arm-native-core arm-chip8
for n in 01 02 03 04 05; do (cd "project/spmp-os/features/filemanager/FM$n" && sh tools/build.sh); done
SPMP_MMI_RAP=firmware/originals/8000_MMI.RAP python3 -m unittest discover -s research/Stage53_to_55/SPMP-OS_Stage55/tests -v
