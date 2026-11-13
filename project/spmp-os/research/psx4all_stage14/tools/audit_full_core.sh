#!/usr/bin/env bash
set -eu
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PSX="$ROOT/vendor/psx4all-dingoo-master/src"
OUT="$ROOT/build/host-core-audit"
mkdir -p "$OUT"
pass=0; fail=0
: > "$OUT/RESULTS.tsv"
for f in cdrom decode_xa mdec misc bios dma hle hw components pad sio spu r3000a_new counters_new gte_pcsx gte_mame mem \
 gpuAPI/gpuAPI gpuAPI/newGPU/newGPU gpuAPI/newGPU/fixed gpuAPI/newGPU/core_Command gpuAPI/newGPU/core_Dma \
 gpuAPI/newGPU/core_Draw gpuAPI/newGPU/core_Misc gpuAPI/newGPU/raster_Sprite gpuAPI/newGPU/raster_Poly \
 gpuAPI/newGPU/raster_Line gpuAPI/newGPU/raster_Image gpuAPI/newGPU/inner \
 interpreter/interpreter recompiler3/arm/recompiler recompiler3/arm/disasm recompiler3/arm/mips_disasm; do
  name="${f//\//_}"
  file="$PSX/$f.cpp"
  if [ ! -f "$file" ]; then echo -e "$f\tmissing\tfile not found" >> "$OUT/RESULTS.tsv"; continue; fi
  if g++ -std=gnu++11 -c -O2 -fno-strict-aliasing -fpermissive -Wno-unused-result \
       -o "$OUT/$name.o" \
       -DGP2X -DPANDORA -DSPMP_PSX4ALL -DDYNAREC -DLSB_FIRST -I"$PSX" -I"$PSX/pandora" "$file" \
       >"$OUT/$name.out" 2>"$OUT/$name.err"; then
    pass=$((pass+1));echo -e "$f\tpass\t" >> "$OUT/RESULTS.tsv"
  else
    fail=$((fail+1));printf '%s\tfail\t%s\n' "$f" "$(head -1 "$OUT/$name.err" | cut -c 1-160)" >> "$OUT/RESULTS.tsv"
  fi
done
printf 'pass=%s fail=%s\n' "$pass" "$fail"
cat "$OUT/RESULTS.tsv"
