CC ?= cc
CLANG ?= clang
AR ?= llvm-ar
V5_DIR ?= ../audio_v5
STAGE10 ?= ../../..
BASE = src/spmp_mixer_v6.c src/spmp_wma_asf_v6.c src/spmp_mix_mp3_bridge_v6.c
HOST_CFLAGS = -O2 -std=c11 -Wall -Wextra -Werror -Iinclude
ARM_CFLAGS = --target=armv5te-none-eabi -mfloat-abi=soft -ffreestanding -fno-builtin -Os -std=c11 -Wall -Wextra -Werror -Ifreestanding/include -Iinclude
HOSTOBJ = $(patsubst src/%.c,build/host/%.o,$(BASE))
ARMOBJ = $(patsubst src/%.c,build/arm/%.o,$(BASE))
all: host
build/host/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(HOST_CFLAGS) -c $< -o $@
build/arm/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CLANG) $(ARM_CFLAGS) -c $< -o $@
artifacts:
	@mkdir -p artifacts
host: artifacts $(HOSTOBJ)
	ar rcs artifacts/libspmp_audio_v6_host.a $(HOSTOBJ)
arm: artifacts $(ARMOBJ)
	$(AR) rcs artifacts/libspmp_audio_v6_armv5.a $(ARMOBJ)
v5-host: host
	$(CC) $(HOST_CFLAGS) -I$(V5_DIR)/include -c src/spmp_mix_audio_v5_bridge_v6.c -o artifacts/spmp_mix_v5_adapter_host.o
v5-arm: arm
	$(CLANG) $(ARM_CFLAGS) -I$(V5_DIR)/include -c src/spmp_mix_audio_v5_bridge_v6.c -o artifacts/spmp_mix_v5_adapter_armv5.o
stage10-arm: arm
	$(CLANG) $(ARM_CFLAGS) -I$(STAGE10)/include -c src/spmp_mix_stage10_v6.c -o artifacts/spmp_mix_stage10_armv5.o
stage10-host: host
	$(CC) $(HOST_CFLAGS) -I$(STAGE10)/include -c src/spmp_mix_stage10_v6.c -o artifacts/spmp_mix_stage10_host.o
check: host
	$(CC) $(HOST_CFLAGS) tests/test_mixer.c artifacts/libspmp_audio_v6_host.a -o build/test_mixer
	./build/test_mixer
	$(CC) $(HOST_CFLAGS) tests/test_asf.c artifacts/libspmp_audio_v6_host.a -o build/test_asf
	./build/test_asf tests/fixtures/sample.wma
clean:
	rm -rf build artifacts/*
.PHONY: all host arm check clean artifacts v5-host v5-arm stage10-arm stage10-host
benchmark: host
	$(CC) $(HOST_CFLAGS) tests/benchmark_mixer.c artifacts/libspmp_audio_v6_host.a -o build/benchmark_mixer
	./build/benchmark_mixer
.PHONY: benchmark

stage10-check: host
	$(CC) $(HOST_CFLAGS) -I$(STAGE10)/include tests/test_stage10_sink.c src/spmp_mix_stage10_v6.c artifacts/libspmp_audio_v6_host.a -o build/test_stage10_sink
	./build/test_stage10_sink

check-sanitizers:
	$(CLANG) -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -std=c11 -Iinclude tests/test_mixer.c src/spmp_mixer_v6.c src/spmp_mix_mp3_bridge_v6.c -o build/test_mixer_sanitized
	ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./build/test_mixer_sanitized
	$(CLANG) -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -std=c11 -Iinclude tests/test_asf.c src/spmp_wma_asf_v6.c -o build/test_asf_sanitized
	ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./build/test_asf_sanitized tests/fixtures/sample.wma
.PHONY: stage10-check check-sanitizers
