#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT/tools/build.sh"
clang -fsanitize=address,undefined -g -std=c11 -O1 -Wall -Wextra -Werror -I"$ROOT/include" "$ROOT/tests/test_theme.c" "$ROOT/src/spmp_ui_theme_v7.c" "$ROOT/src/spmp_ui_builtin_glyphs.c" -o "$ROOT/build/test_theme_sanitized"
"$ROOT/build/test_theme_sanitized" "$ROOT/themes"
clang -fsanitize=address,undefined -g -std=c11 -O1 -Wall -Wextra -Werror -I"$ROOT/include" -I"$ROOT/stage10_compat/include" "$ROOT/tests/test_bridge.c" "$ROOT/src/spmp_ui_stage10_bridge.c" "$ROOT/src/spmp_ui_theme_v7.c" "$ROOT/src/spmp_ui_builtin_glyphs.c" -o "$ROOT/build/test_bridge_sanitized"
"$ROOT/build/test_bridge_sanitized"
python3 "$ROOT/tools/render_collection.py"
python3 "$ROOT/tests/test_assets_v8.py"
file "$ROOT/build/theme_armv5.o"
