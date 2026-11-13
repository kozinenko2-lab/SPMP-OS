#!/bin/sh
# Reproducible exact-firmware static audit. No writes to firmware / console.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
RAP=${1:-"$HERE/reference/8000_MMI.RAP"}
OUT=${2:-"$HERE/evidence"}
if [ ! -f "$RAP" ]; then
  echo "Provide original 8000_MMI.RAP as argument: sh tools/run_stage55.sh /path/to/8000_MMI.RAP" >&2
  exit 2
fi
python3 "$HERE/tools/analyze_gui_boot.py" --rap "$RAP" --outdir "$OUT"
SPMP_MMI_RAP="$RAP" python3 -m unittest discover -s "$HERE/tests" -v
