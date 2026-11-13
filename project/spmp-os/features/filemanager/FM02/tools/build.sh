#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FM01=$(CDPATH= cd -- "$HERE/../FM01" && pwd)
cd "$HERE"
mkdir -p build
HOSTCC=${HOSTCC:-cc};CLANG=${CLANG:-clang}
COMMON="-std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude -I$FM01/include -Ihost"
$HOSTCC $COMMON src/fm02_core.c src/fm02_render.c host/fm02_posix.c host/fm02_cli.c "$FM01/src/fm01_core.c" -o build/fm02_ops
$HOSTCC $COMMON -fsanitize=address,undefined -fno-omit-frame-pointer -O1 src/fm02_core.c src/fm02_render.c host/fm02_posix.c host/fm02_cli.c "$FM01/src/fm01_core.c" -o build/fm02_ops_asan
$HOSTCC $COMMON src/fm02_core.c src/fm02_render.c "$FM01/src/fm01_core.c" tests/test_fm02.c -o build/test_fm02
$HOSTCC $COMMON -shared -fPIC src/fm02_core.c src/fm02_render.c host/fm02_posix.c host/fm02_demo_bridge.c "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" "$FM01/host/fm01_posix.c" -I"$FM01/host" -o build/libfm02_host.so
./build/test_fm02
python3 tests/test_backend.py
FLAGS="--target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-stack-protector -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -I$FM01/include"
$CLANG $FLAGS -c src/fm02_core.c -o build/fm02_core_armv5.o
$CLANG $FLAGS -c src/fm02_render.c -o build/fm02_render_armv5.o
llvm-ar rcs build/libspmp_fm02_armv5.a build/fm02_core_armv5.o build/fm02_render_armv5.o
ld.lld -m armelf -r build/fm02_core_armv5.o build/fm02_render_armv5.o "$FM01/build/fm01_core_armv5.o" "$FM01/build/fm01_render_armv5.o" -o build/fm01_fm02_combined_armv5.o
if [ -n "$(nm -u build/fm01_fm02_combined_armv5.o)" ]; then nm -u build/fm01_fm02_combined_armv5.o;exit 1;fi
file build/fm01_fm02_combined_armv5.o
