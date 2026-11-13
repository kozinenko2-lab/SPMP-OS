#!/usr/bin/env bash
set -eu
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
bash tools/audit_armv5_core.sh
SRC="$ROOT/vendor/psx4all-dingoo-master/src"
H="$ROOT/tools/armv5_headers"
OUT="$ROOT/build/armv5/spmp_port"
mkdir -p "$OUT"
for mod in spmp_psx_bios spmp_psx_opt spmp_psx_jit_store spmp_psx_cache_guard spmp_psx_budget spmp_psx_arm926_priv; do
  clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -std=c11 \
    -ffreestanding -fno-builtin -fsigned-char -O2 -DSPMP_PSX4ALL \
    -I"$H" -I"$SRC" -I"$SRC/pandora" \
    -c "$SRC/spmp_port/$mod.c" -o "$OUT/$mod.o"
done
ld.lld -r -m armelf_linux_eabi \
  build/armv5/core/*.o build/armv5/spmp_port/*.o \
  -o build/armv5/psx4all_armv5_core_bridge.o
readelf -h build/armv5/psx4all_armv5_core_bridge.o > build/armv5/ELF_HEADER.txt
nm -u build/armv5/psx4all_armv5_core_bridge.o | awk '{print $NF}' | sort -u > build/armv5/unresolved_symbols_bridge.txt
printf 'ARMv5 bridge relocatable ELF generated; unresolved external runtime symbols: %s\n' "$(wc -l < build/armv5/unresolved_symbols_bridge.txt)"
