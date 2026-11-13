CC ?= cc
CLANG ?= clang
ARM_LD ?= ld.lld
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS ?= -Iinclude
CORE = src/inspect.c src/wav.c src/bmp.c src/skin.c src/ui.c
HOST_LIBS := $(shell pkg-config --libs libpng libjpeg libgif 2>/dev/null)
ARMFLAGS = --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -O2 -std=c11 -Wall -Wextra -Werror -fno-pic -fno-pie
ARMOBJS = $(CORE:src/%.c=build/arm/%.o) build/arm/arm_compat.o
.PHONY: all host test arm-core stage10-bridge clean
all: host
host: build/mm-host
build:
	mkdir -p build
build/mm-host: $(CORE) src/host_image.c src/host.c include/spmp_mm.h src/common.h | build
	@if ! pkg-config --exists libpng libjpeg libgif; then echo 'Host requires libpng, libjpeg, giflib development packages'; exit 1; fi
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CORE) src/host_image.c src/host.c $(HOST_LIBS) -o $@
# Optional ARMv5 ET_REL compile, independent of any emulator or system Makefile.
# Exports are not yet bound to hardware syscalls or memory addresses.
build/arm/%.o: src/%.c include/spmp_mm.h src/common.h
	@mkdir -p build/arm
	$(CLANG) $(ARMFLAGS) $(CPPFLAGS) -c $< -o $@
arm-core: $(ARMOBJS)
	$(ARM_LD) -m armelf -r $(ARMOBJS) -o build/spmp-mm-armv5.o

# This adapter depends only on the Stage10 public defos_hal.h; no other
# Stage10 or emulator object is rebuilt/modified.
stage10-bridge: | build
	$(CLANG) $(ARMFLAGS) $(CPPFLAGS) -I../../include -c src/stage10_bridge.c -o build/stage10-bridge-armv5.o

test: host
	python3 -m unittest discover -s tests -v
clean:
	rm -rf build
