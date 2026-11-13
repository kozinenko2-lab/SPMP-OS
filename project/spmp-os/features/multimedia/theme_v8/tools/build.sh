#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mkdir -p "$HERE/build"
CC="${CC:-clang}"
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -I"$HERE/include" "$HERE/src/spmp_ui_theme_v7.c" "$HERE/src/spmp_ui_builtin_glyphs.c" "$HERE/src/spmp_theme_catalog_v8.c" "$HERE/src/spmp_theme_config_v8.c" "$HERE/src/theme_preview_host.c" -o "$HERE/build/theme_preview_host"
clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-pic -O2 -std=c11 -Wall -Wextra -Werror -I"$HERE/include" -c "$HERE/src/spmp_ui_theme_v7.c" -o "$HERE/build/theme_armv5.o"
clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-pic -O2 -std=c11 -Wall -Wextra -Werror -I"$HERE/include" -I"${SPMP_STAGE10:-$HERE/stage10_compat/include}" -c "$HERE/src/spmp_ui_stage10_bridge.c" -o "$HERE/build/bridge_armv5.o"
clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-pic -O2 -std=c11 -Wall -Wextra -Werror -I"$HERE/include" -c "$HERE/src/spmp_ui_builtin_glyphs.c" -o "$HERE/build/glyphs_armv5.o"

clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-pic -O2 -std=c11 -Wall -Wextra -Werror -I"$HERE/include" -c "$HERE/src/spmp_theme_catalog_v8.c" -o "$HERE/build/catalog_armv5.o"
clang --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-pic -O2 -std=c11 -Wall -Wextra -Werror -I"$HERE/include" -c "$HERE/src/spmp_theme_config_v8.c" -o "$HERE/build/config_armv5.o"
llvm-ar rcs "$HERE/build/libspmp_ui_themes_v8_armv5.a" "$HERE/build/theme_armv5.o" "$HERE/build/bridge_armv5.o" "$HERE/build/glyphs_armv5.o" "$HERE/build/catalog_armv5.o" "$HERE/build/config_armv5.o"
clang -std=c11 -O2 -Wall -Wextra -Werror -I"$HERE/include" "$HERE/src/spmp_theme_catalog_v8.c" "$HERE/tests/test_catalog_v8.c" -o "$HERE/build/test_catalog_v8"
"$HERE/build/test_catalog_v8"
clang -std=c11 -O2 -Wall -Wextra -Werror -I"$HERE/include" "$HERE/src/spmp_theme_catalog_v8.c" "$HERE/src/spmp_theme_config_v8.c" "$HERE/tests/test_config_v8.c" -o "$HERE/build/test_config_v8"
"$HERE/build/test_config_v8"
