#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$HERE"
mkdir -p build
HOSTCC=${HOSTCC:-cc}
CLANG=${CLANG:-clang}
COMMON='-std=c11 -Wall -Wextra -Werror -Wpedantic -O2 -Iinclude'
$HOSTCC $COMMON src/fm01_core.c src/fm01_render.c host/fm01_posix.c host/fm01_cli.c -Ihost -o build/fm01_host
$HOSTCC $COMMON -fPIC -shared src/fm01_core.c src/fm01_render.c host/fm01_posix.c host/fm01_demo_bridge.c -Ihost -o build/libfm01_host.so
$HOSTCC $COMMON src/fm01_core.c src/fm01_render.c tests/test_fm01.c -o build/test_fm01
./build/test_fm01
FLAGS='--target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-stack-protector -std=c11 -O2 -Wall -Wextra -Werror -Iinclude'
$CLANG $FLAGS -c src/fm01_core.c -o build/fm01_core_armv5.o
$CLANG $FLAGS -c src/fm01_render.c -o build/fm01_render_armv5.o
llvm-ar rcs build/libspmp_fm01_armv5.a build/fm01_core_armv5.o build/fm01_render_armv5.o
ld.lld -m armelf -r build/fm01_core_armv5.o build/fm01_render_armv5.o -o build/fm01_combined_armv5.o
if [ -n "$(nm -u build/fm01_combined_armv5.o)" ]; then
  echo 'Unresolved symbols in ARMv5 module' >&2
  nm -u build/fm01_combined_armv5.o >&2
  exit 1
fi
printf 'Built host + ARMv5 modules\n'
