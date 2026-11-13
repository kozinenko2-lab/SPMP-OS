#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FM01=$(CDPATH= cd -- "$HERE/../FM01" && pwd)
FM02=$(CDPATH= cd -- "$HERE/../FM02" && pwd)
FM03=$(CDPATH= cd -- "$HERE/../FM03" && pwd)
cd "$HERE"
mkdir -p build
HOSTCC=${HOSTCC:-cc};CLANG=${CLANG:-clang}
FLAGS="-std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude -I$FM01/include -I$FM02/include"
$HOSTCC $FLAGS src/fm04_core.c src/fm04_render.c tests/test_core.c "$FM01/src/fm01_core.c" "$FM02/src/fm02_core.c" -o build/test_fm04
$HOSTCC $FLAGS -I"$FM01/host" -I"$FM02/host" src/fm04_core.c tests/test_rename_posix.c "$FM01/src/fm01_core.c" "$FM01/host/fm01_posix.c" "$FM02/src/fm02_core.c" "$FM02/host/fm02_posix.c" -o build/test_rename_posix
./build/test_fm04
./build/test_rename_posix
$HOSTCC $FLAGS -O1 -fsanitize=address,undefined -fno-omit-frame-pointer src/fm04_core.c src/fm04_render.c tests/test_core.c "$FM01/src/fm01_core.c" "$FM02/src/fm02_core.c" -o build/test_fm04_asan
ASAN_OPTIONS=detect_leaks=1 ./build/test_fm04_asan
$HOSTCC $FLAGS -shared -fPIC src/fm04_core.c src/fm04_render.c "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" "$FM02/src/fm02_core.c" host/fm04_demo_bridge.c -o build/libfm04_host.so
ARM="--target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-stack-protector -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -Iinclude -I$FM01/include -I$FM02/include"
$CLANG $ARM -c src/fm04_core.c -o build/fm04_core_armv5.o
$CLANG $ARM -c src/fm04_render.c -o build/fm04_render_armv5.o
llvm-ar rcs build/libspmp_fm04_armv5.a build/fm04_core_armv5.o build/fm04_render_armv5.o
$CLANG $ARM -c "$FM01/src/fm01_core.c" -o build/fm01_core_armv5.o
$CLANG $ARM -c "$FM01/src/fm01_render.c" -o build/fm01_render_armv5.o
$CLANG $ARM -c "$FM02/src/fm02_core.c" -o build/fm02_core_armv5.o
$CLANG $ARM -c "$FM02/src/fm02_render.c" -o build/fm02_render_armv5.o
$CLANG $ARM -c "$FM03/src/fm03_core.c" -I"$FM03/include" -o build/fm03_core_armv5.o
$CLANG $ARM -c "$FM03/src/fm03_render.c" -I"$FM03/include" -o build/fm03_render_armv5.o
ld.lld -m armelf -r build/fm01_core_armv5.o build/fm01_render_armv5.o build/fm02_core_armv5.o build/fm02_render_armv5.o build/fm03_core_armv5.o build/fm03_render_armv5.o build/fm04_core_armv5.o build/fm04_render_armv5.o -o build/FM01_to_FM04_combined_armv5.o
if [ -n "$(nm -u build/FM01_to_FM04_combined_armv5.o)" ]; then nm -u build/FM01_to_FM04_combined_armv5.o;exit 1;fi
file build/FM01_to_FM04_combined_armv5.o
