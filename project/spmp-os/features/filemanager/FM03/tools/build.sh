#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FM01=$(CDPATH= cd -- "$HERE/../FM01" && pwd)
cd "$HERE"
mkdir -p build
HOSTCC=${HOSTCC:-cc}; CLANG=${CLANG:-clang}
FLAGS="-std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude -I$FM01/include -Ihost"
$HOSTCC $FLAGS src/fm03_core.c host/fm03_posix.c host/fm03_cli.c "$FM01/src/fm01_core.c" -o build/fm03_props
$HOSTCC $FLAGS src/fm03_core.c src/fm03_render.c tests/test_core.c "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" -o build/test_core
$HOSTCC $FLAGS -shared -fPIC src/fm03_core.c src/fm03_render.c host/fm03_posix.c host/fm03_demo_bridge.c \
    "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" "$FM01/host/fm01_posix.c" -I"$FM01/host" -o build/libfm03_host.so
./build/test_core
python3 tests/test_host.py
ARM="--target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-stack-protector -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -Iinclude -I$FM01/include"
$CLANG $ARM -c src/fm03_core.c -o build/fm03_core_armv5.o
$CLANG $ARM -c src/fm03_render.c -o build/fm03_render_armv5.o
llvm-ar rcs build/libspmp_fm03_armv5.a build/fm03_core_armv5.o build/fm03_render_armv5.o
ld.lld -m armelf -r build/fm03_core_armv5.o build/fm03_render_armv5.o \
    "$FM01/build/fm01_core_armv5.o" "$FM01/build/fm01_render_armv5.o" \
    -o build/fm01_fm03_combined_armv5.o
if [ -n "$(nm -u build/fm01_fm03_combined_armv5.o)" ]; then nm -u build/fm01_fm03_combined_armv5.o;exit 1;fi
file build/fm01_fm03_combined_armv5.o
