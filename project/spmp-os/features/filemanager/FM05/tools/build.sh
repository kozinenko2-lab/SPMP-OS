#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FM01=$(CDPATH= cd -- "$HERE/../FM01" && pwd)
FM02=$(CDPATH= cd -- "$HERE/../FM02" && pwd)
FM03=$(CDPATH= cd -- "$HERE/../FM03" && pwd)
FM04=$(CDPATH= cd -- "$HERE/../FM04" && pwd)
cd "$HERE"
mkdir -p build
HOSTCC=${HOSTCC:-clang}; CLANG=${CLANG:-clang}
INC="-Iinclude -I$FM01/include -I$FM02/include -I$FM03/include -I$FM04/include"
FLAGS="-std=c11 -Wall -Wextra -Wpedantic -Werror -O2 $INC"
$HOSTCC $FLAGS src/fm05_core.c src/fm05_render.c src/fm05_session.c tests/test_core.c "$FM04/src/fm04_core.c" "$FM04/src/fm04_render.c" "$FM02/src/fm02_core.c" "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" -o build/test_core
./build/test_core
$HOSTCC $FLAGS -Ihost src/fm05_core.c host/fm05_posix.c tests/test_posix.c "$FM04/src/fm04_core.c" "$FM04/src/fm04_render.c" "$FM02/src/fm02_core.c" "$FM01/src/fm01_core.c" -o build/test_posix
./build/test_posix
$HOSTCC $FLAGS -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer src/fm05_core.c src/fm05_session.c src/fm05_render.c tests/test_core.c "$FM04/src/fm04_core.c" "$FM04/src/fm04_render.c" "$FM02/src/fm02_core.c" "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" -o build/test_core_asan
ASAN_OPTIONS=detect_leaks=1 ./build/test_core_asan
$HOSTCC $FLAGS -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Ihost src/fm05_core.c host/fm05_posix.c tests/test_posix.c "$FM04/src/fm04_core.c" "$FM04/src/fm04_render.c" "$FM02/src/fm02_core.c" "$FM01/src/fm01_core.c" -o build/test_posix_asan
ASAN_OPTIONS=detect_leaks=1 ./build/test_posix_asan
$HOSTCC $FLAGS -shared -fPIC src/fm05_core.c src/fm05_render.c src/fm05_session.c host/fm05_demo_bridge.c "$FM04/src/fm04_core.c" "$FM04/src/fm04_render.c" "$FM02/src/fm02_core.c" "$FM01/src/fm01_core.c" "$FM01/src/fm01_render.c" -o build/libfm05_host.so
ARM="--target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -fno-stack-protector -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror $INC"
$CLANG $ARM -c src/fm05_core.c -o build/fm05_core_armv5.o
$CLANG $ARM -c src/fm05_render.c -o build/fm05_render_armv5.o
$CLANG $ARM -c src/fm05_session.c -o build/fm05_session_armv5.o
llvm-ar rcs build/libspmp_fm05_armv5.a build/fm05_core_armv5.o build/fm05_render_armv5.o build/fm05_session_armv5.o
for STEP in 01 02 03 04; do
    SRC=$(eval echo "\$FM$STEP")
    $CLANG $ARM -c "$SRC/src/fm${STEP}_core.c" -o "build/fm${STEP}_core_armv5.o"
    $CLANG $ARM -c "$SRC/src/fm${STEP}_render.c" -o "build/fm${STEP}_render_armv5.o"
done
ld.lld -m armelf -r build/fm0*_core_armv5.o build/fm0*_render_armv5.o build/fm05_session_armv5.o -o build/FM01_to_FM05_combined_armv5.o
if [ -n "$(nm -u build/FM01_to_FM05_combined_armv5.o)" ];then nm -u build/FM01_to_FM05_combined_armv5.o;exit 1;fi
file build/FM01_to_FM05_combined_armv5.o
printf 'FM05 host tests + sanitizers + ARMv5 full integration PASS\n'
