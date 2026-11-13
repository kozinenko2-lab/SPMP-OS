#!/usr/bin/env bash
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/vendor/psx4all-dingoo-master/src"
H="$ROOT/tools/armv5_headers"
OUT="$ROOT/build/armv5/core"
mkdir -p "$OUT"
: > "$OUT/RESULTS.tsv"
pass=0; fail=0
for mod in cdrom decode_xa mdec misc bios dma hle hw components pad sio spu r3000a_new counters_new gte_pcsx gte_mame mem \
 gpuAPI/gpuAPI gpuAPI/newGPU/newGPU gpuAPI/newGPU/fixed gpuAPI/newGPU/core_Command gpuAPI/newGPU/core_Dma \
 gpuAPI/newGPU/core_Draw gpuAPI/newGPU/core_Misc gpuAPI/newGPU/raster_Sprite gpuAPI/newGPU/raster_Poly \
 gpuAPI/newGPU/raster_Line gpuAPI/newGPU/raster_Image gpuAPI/newGPU/inner \
 interpreter/interpreter recompiler3/arm/recompiler recompiler3/arm/disasm recompiler3/arm/mips_disasm; do
  name="${mod//\//_}"
  flags=()
  if [ "$mod" = recompiler3/arm/recompiler ]; then flags+=(-DDYNAREC); fi
  if clang++ --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -std=gnu++11 \
    -ffreestanding -fno-exceptions -fno-rtti -fno-strict-aliasing -fsigned-char \
    -O2 -DGP2X -DPANDORA -DSPMP_PSX4ALL -DLSB_FIRST "${flags[@]}" \
    -I"$H" -I"$SRC" -I"$SRC/pandora" \
    -c "$SRC/$mod.cpp" -o "$OUT/$name.o" >"$OUT/$name.out" 2>"$OUT/$name.err"; then
    echo -e "$mod\tpass" >> "$OUT/RESULTS.tsv"
    pass=$((pass+1))
  else
    first="$(grep -m1 'error:' "$OUT/$name.err" | head -c 200)"
    echo -e "$mod\tfail\t$first" >> "$OUT/RESULTS.tsv"
    fail=$((fail+1))
  fi
done
printf 'ARMv5 source compile audit: pass=%s fail=%s\n' "$pass" "$fail"
cat "$OUT/RESULTS.tsv"
[ "$fail" = 0 ]
