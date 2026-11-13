HOSTCC ?= clang
ARMCC ?= clang
ifneq ($(wildcard ../redboot_sd_stage45/include/spmp_stage45_sd_poll.h),)
S45 ?= ../redboot_sd_stage45
S43 ?= ../redboot_sd_stage43
S44 ?= ../redboot_sd_stage44
else
S45 ?= ../SPMP-OS_Stage45_SDRead_Research
S43 ?= ../SPMP-OS_Stage43_SDTrace
S44 ?= ../SPMP-OS_Stage44_SDMMCRestore
endif
RAP ?= ../8000_MMI.RAP
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -I$(S45)/include -I$(S43)/include
ARMFLAGS = --target=armv5te-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Iinclude -I$(S45)/include -I$(S43)/include
.PHONY: all test sanitize static arm integrate clean
all: test static arm
build:
	mkdir -p build
build/test_stage46: tests/test_stage46.c src/spmp_stage46_sector_guard.c $(S45)/src/spmp_stage45_sd_poll.c | build
	$(HOSTCC) $(CFLAGS) $^ -o $@
build/test_stage46_trace: tests/test_stage46_trace.c src/spmp_stage46_trace.c $(S43)/src/spmp_bootlog.c | build
	$(HOSTCC) $(CFLAGS) $^ -o $@
test: build/test_stage46 build/test_stage46_trace
	./build/test_stage46
	./build/test_stage46_trace
sanitize: | build
	$(HOSTCC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage46.c src/spmp_stage46_sector_guard.c $(S45)/src/spmp_stage45_sd_poll.c -o build/test_stage46_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage46_asan
	$(HOSTCC) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage46_trace.c src/spmp_stage46_trace.c $(S43)/src/spmp_bootlog.c -o build/test_stage46_trace_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage46_trace_asan
static:
	python3 tools/audit_cmd17_path.py $(RAP) --output evidence/stage46_sd_path.json
	python3 tests/test_stage46_static.py $(RAP)
arm: | build
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage46_sector_guard.c -o build/spmp_stage46_sector_guard_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage46_trace.c -o build/spmp_stage46_trace_ARMv5.o
integrate: arm
	$(MAKE) -C $(S45) integrate S43=$(abspath $(S43)) S44=$(abspath $(S44))
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage46_stage45_integrated_ARMv5.o $(S45)/build/spmp_stage45_stage44_integrated_ARMv5.o build/spmp_stage46_sector_guard_ARMv5.o build/spmp_stage46_trace_ARMv5.o
	@if nm -u build/spmp_stage46_stage45_integrated_ARMv5.o | grep -q ' U '; then echo 'unexpected ARM undefined symbols'; exit 1; fi
clean:
	rm -rf build/*
