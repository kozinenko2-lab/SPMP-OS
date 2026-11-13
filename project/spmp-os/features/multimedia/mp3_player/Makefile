CC ?= cc
CLANG ?= clang
AR ?= llvm-ar
LD_LLD ?= ld.lld
FEATURES := -std=gnu11 -DSPMP_MAD_STATIC -DOPT_SPEED -DNDEBUG -DHAVE_STRING_H -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8
HOST = $(FEATURES) -DFPM_64BIT -O2 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Iinclude -Isrc -Ivendor/libmad
ARM = $(FEATURES) -DFPM_ARM -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-strict-aliasing -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables --target=armv5te-none-eabi -march=armv5te -marm -mfloat-abi=soft -Icompat/arm -Iinclude -Isrc -Ivendor/libmad
MAD = bit stream frame synth layer12 layer3 huffman timer version fixed
SDK = spmp_mp3_player spmp_mp3_id3 spmp_m3u_playlist
HOSTOBJ = $(patsubst %,build/host/%.o,$(MAD)) $(patsubst %,build/host/%.o,$(SDK))
ARMOBJ = $(patsubst %,build/arm/%.o,$(MAD)) $(patsubst %,build/arm/%.o,$(SDK))
.PHONY: all armv5 check clean
all: build/mp3_to_wav
build/host build/arm:
	mkdir -p $@
build/host/%.o: vendor/libmad/%.c | build/host
	$(CC) $(HOST) -MMD -MP -c $< -o $@
build/host/%.o: src/%.c | build/host
	$(CC) $(HOST) -MMD -MP -c $< -o $@
build/arm/%.o: vendor/libmad/%.c | build/arm
	$(CLANG) $(ARM) -MMD -MP -c $< -o $@
build/arm/%.o: src/%.c | build/arm
	$(CLANG) $(ARM) -MMD -MP -c $< -o $@
build/libspmp_mp3_host.a: $(HOSTOBJ)
	$(AR) rcs $@ $^
build/libspmp_mp3_armv5.a: $(ARMOBJ)
	$(AR) rcs $@ $^
armv5: build/libspmp_mp3_armv5.a
build/mp3_to_wav: tests/mp3_to_wav.c build/libspmp_mp3_host.a
	$(CC) $(HOST) $< build/libspmp_mp3_host.a -o $@
check: all
	python3 tests/verify_mp3.py
clean:
	rm -rf build

# Compile against the actual Stage10 HAL, without copying/modifying it.
STAGE10 ?= ../../..
build/stage10_adapter_test: tests/test_stage10_adapter.c src/spmp_mp3_stage10.c
	$(CC) $(HOST) -I$(STAGE10)/include $^ -o $@
build/arm/spmp_mp3_stage10.o: src/spmp_mp3_stage10.c | build/arm
	$(CLANG) $(ARM) -MMD -MP -I$(STAGE10)/include -c $< -o $@
armv5_stage10: armv5 build/arm/spmp_mp3_stage10.o
	$(AR) rcs build/libspmp_mp3_stage10_armv5.a build/arm/spmp_mp3_stage10.o

build/arm/spmp_mp3_aeabi.o: src/spmp_mp3_aeabi.c | build/arm
	$(CLANG) $(ARM) -MMD -MP -c $< -o $@
build/arm/spmp_mp3_aeabi_arm.o: src/spmp_mp3_aeabi_arm.S | build/arm
	$(CLANG) $(ARM) -MMD -MP -c $< -o $@
armv5_freestanding: armv5 build/arm/spmp_mp3_aeabi.o build/arm/spmp_mp3_aeabi_arm.o
	$(AR) rcs build/libspmp_mp3_freestanding_armv5.a build/arm/spmp_mp3_aeabi.o build/arm/spmp_mp3_aeabi_arm.o
	$(LD_LLD) -m armelf -nostdlib --whole-archive -Ttext=0x02000000 -e spmp_mp3_player_size -o build/spmp_mp3_linkcheck_armv5.elf build/libspmp_mp3_armv5.a build/libspmp_mp3_freestanding_armv5.a


build/test_m3u: tests/test_m3u.c build/libspmp_mp3_host.a
	$(CC) $(HOST) $< build/libspmp_mp3_host.a -o $@

-include $(HOSTOBJ:.o=.d) $(ARMOBJ:.o=.d)
build/arm/spmp_mp3_app.o: src/spmp_mp3_app.c | build/arm
	$(CLANG) $(ARM) -MMD -MP -I$(STAGE10)/include -c $< -o $@
armv5_app: armv5_stage10 build/arm/spmp_mp3_app.o
	$(AR) rcs build/libspmp_mp3_app_armv5.a build/arm/spmp_mp3_app.o
build/test_division: tests/test_aeabi_division.c src/spmp_mp3_aeabi.c
	$(CC) -O2 -fno-builtin -D__arm__ $^ -o $@
