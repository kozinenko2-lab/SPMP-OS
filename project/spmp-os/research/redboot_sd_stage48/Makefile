HOSTCC ?= clang
ARMCC ?= clang
STAGE43 ?= ../redboot_sd_stage43
STAGE47 ?= ../redboot_sd_stage47
RAP ?= /mnt/data/8000_MMI.RAP
INCLUDES = -Iinclude -I$(STAGE43)/include
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES)
ARMFLAGS = --target=armv5te-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables $(INCLUDES)
.PHONY: all test static sanitize arm integrate clean
all: test static arm
build:
	mkdir -p build
build/test_stage48_monitor: tests/test_stage48_monitor.c src/spmp_stage48_sd_monitor.c $(STAGE43)/src/spmp_bootlog.c | build
	$(HOSTCC) $(CFLAGS) $^ -o $@
test: build/test_stage48_monitor
	./build/test_stage48_monitor
	SPMP48_RAP=$(RAP) python3 tests/test_stage48_static.py
static:
	python3 tools/audit_stage48_init.py $(RAP) --out evidence/verified_sd_commands_30.json
sanitize: | build
	$(HOSTCC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage48_monitor.c src/spmp_stage48_sd_monitor.c $(STAGE43)/src/spmp_bootlog.c -o build/test_stage48_monitor_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage48_monitor_asan
arm: | build
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage48_sd_monitor.c -o build/spmp_stage48_sd_monitor_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c $(STAGE43)/src/spmp_bootlog.c -o build/spmp_stage48_bootlog_ARMv5.o
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage48_sd_trace_ARMv5.o build/spmp_stage48_sd_monitor_ARMv5.o build/spmp_stage48_bootlog_ARMv5.o
	@if nm -u build/spmp_stage48_sd_trace_ARMv5.o | grep -q ' U '; then echo 'unresolved ARM symbols'; exit 1; fi
integrate: arm
	$(MAKE) -C $(STAGE47) arm S42=$(abspath $(STAGE47)/../redboot_sd_stage42) S43=$(abspath $(STAGE43)) S45=$(abspath $(STAGE47)/../redboot_sd_stage45) S46=$(abspath $(STAGE47)/../redboot_sd_stage46)
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage48_stage47_integrated_ARMv5.o $(STAGE47)/build/spmp_stage47_fat_guard_bridge_ARMv5.o build/spmp_stage48_sd_monitor_ARMv5.o
	@if nm -u build/spmp_stage48_stage47_integrated_ARMv5.o | grep -q ' U '; then echo 'unresolved ARM symbols'; exit 1; fi
clean:
	rm -f build/*
