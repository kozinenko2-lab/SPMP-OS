HOSTCC ?= clang
ARMCC ?= clang
S42 ?= ../redboot_sd_stage42
S43 ?= ../redboot_sd_stage43
S44 ?= ../redboot_sd_stage44
S45 ?= ../redboot_sd_stage45
S46 ?= ../redboot_sd_stage46
RAP ?= /mnt/data/8000_MMI.RAP
INCLUDE = -Iinclude -I$(S42)/include -I$(S43)/include -I$(S45)/include -I$(S46)/include
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDE)
ARMFLAGS = --target=armv5te-none-eabi -march=armv5te -mfloat-abi=soft \
 -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin \
 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables $(INCLUDE)
.PHONY: test sanitize static arm all clean
all: test static arm
build:
	mkdir -p build
build/test_stage47_unit: tests/test_stage47_unit.c src/spmp_stage47_guarded_fat.c $(S46)/src/spmp_stage46_sector_guard.c $(S45)/src/spmp_stage45_sd_poll.c $(S42)/src/spmp_fat_bridge.c $(S42)/src/spmp_fat_reader.c $(S43)/src/spmp_bootlog.c | build
	$(HOSTCC) $(CFLAGS) $^ -o $@
build/test_stage47_disk: tests/test_stage47_disk.c src/spmp_stage47_guarded_fat.c $(S46)/src/spmp_stage46_sector_guard.c $(S45)/src/spmp_stage45_sd_poll.c $(S42)/src/spmp_fat_bridge.c $(S42)/src/spmp_fat_reader.c $(S42)/src/spmp_dualboot.c $(S43)/src/spmp_bootlog.c | build
	$(HOSTCC) $(CFLAGS) $^ -o $@
test: build/test_stage47_unit build/test_stage47_disk
	./build/test_stage47_unit
	python3 tests/test_stage47_static.py
	python3 tests/test_stage47_disk.py
sanitize: | build
	$(HOSTCC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage47_unit.c src/spmp_stage47_guarded_fat.c $(S46)/src/spmp_stage46_sector_guard.c $(S45)/src/spmp_stage45_sd_poll.c $(S42)/src/spmp_fat_bridge.c $(S42)/src/spmp_fat_reader.c $(S43)/src/spmp_bootlog.c -o build/test_stage47_unit_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage47_unit_asan
static:
	python3 tools/audit_stage47_sd.py $(RAP) --out evidence/stage47_command_map.json
arm: | build
	$(MAKE) -C $(S46) integrate S43=$(abspath $(S43)) S44=$(abspath $(S44)) S45=$(abspath $(S45))
	$(ARMCC) $(ARMFLAGS) -c $(S42)/runtime_from_stage40/spmp_numeric.c -o build/spmp_stage47_numeric_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c $(S42)/runtime_from_stage40/spmp_minilibc.c -o build/spmp_stage47_minilibc_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage47_guarded_fat.c -o build/spmp_stage47_guarded_fat_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c $(S42)/src/spmp_fat_reader.c -o build/spmp_stage47_fat_reader_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c $(S42)/src/spmp_fat_bridge.c -o build/spmp_stage47_fat_bridge_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c $(S42)/src/spmp_dualboot.c -o build/spmp_stage47_dualboot_ARMv5.o
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage47_fat_guard_bridge_ARMv5.o \
	 $(S46)/build/spmp_stage46_stage45_integrated_ARMv5.o \
	 build/spmp_stage47_guarded_fat_ARMv5.o \
	 build/spmp_stage47_fat_reader_ARMv5.o \
	 build/spmp_stage47_fat_bridge_ARMv5.o \
	 build/spmp_stage47_dualboot_ARMv5.o \
	 build/spmp_stage47_numeric_ARMv5.o \
	 build/spmp_stage47_minilibc_ARMv5.o
	@if nm -u build/spmp_stage47_fat_guard_bridge_ARMv5.o | grep -q ' U '; then echo 'unresolved ARM symbols'; exit 1; fi
clean:
	rm -f build/*
