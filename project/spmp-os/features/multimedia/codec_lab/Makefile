CC ?= cc
CLANG ?= clang
AR ?= llvm-ar
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror -Iinclude
ARM_FLAGS = --target=armv5te-none-eabi -std=c11 -O3 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -march=armv5te -marm -mfloat-abi=soft -Iinclude
SRC = src/spmp_flac_lite.c src/spmp_media_probe.c
.PHONY: all check armv5 clean
all: build/probe_test build/decoder_cli build/probe_file build/mmpi_inspect
build:
	mkdir -p build
build/probe_test: $(SRC) tests/probe_test.c | build
	$(CC) $(CFLAGS) $^ -o $@
build/decoder_cli: src/spmp_flac_lite.c tests/decoder_cli.c | build
	$(CC) $(CFLAGS) $^ -o $@
build/probe_file: src/spmp_media_probe.c tests/probe_file.c | build
	$(CC) $(CFLAGS) $^ -o $@
build/mmpi_inspect: src/spmp_media_probe.c tests/mmpi_inspect.c | build
	$(CC) $(CFLAGS) $^ -o $@
check: all
	./build/probe_test
	python3 tools/verify_decoder.py
armv5: build/spmp_flac_armv5.o build/spmp_probe_armv5.o
	$(AR) rcs build/libspmp_codec_armv5.a $^
build/spmp_flac_armv5.o: src/spmp_flac_lite.c | build
	$(CLANG) $(ARM_FLAGS) -c $< -o $@
build/spmp_probe_armv5.o: src/spmp_media_probe.c | build
	$(CLANG) $(ARM_FLAGS) -c $< -o $@
clean:
	rm -rf build
