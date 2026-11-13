HOSTCC ?= clang
ARMCC ?= clang
HOSTFLAGS = -std=c11 -O2 -g -Wall -Wextra -Werror -Iinclude
ifneq ($(wildcard ../redboot_sd_stage43/include/spmp_bootlog.h),)
S43 ?= ../redboot_sd_stage43
else
S43 ?= ../SPMP-OS_Stage43_SDTrace
endif
ifneq ($(wildcard ../redboot_sd_stage44/src/spmp_stage44_flush.c),)
S44 ?= ../redboot_sd_stage44
else
S44 ?= ../SPMP-OS_Stage44_SDMMCRestore
endif
ARMFLAGS = --target=armv5te-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -Iinclude
.PHONY: all test sanitize arm integrate clean
all: test arm
build:
	mkdir -p build
build/test_stage45: tests/test_stage45.c src/spmp_stage45_sd_poll.c | build
	$(HOSTCC) $(HOSTFLAGS) $^ -o $@
build/test_stage45_trace: tests/test_stage45_trace.c src/spmp_stage45_trace.c $(S43)/src/spmp_bootlog.c | build
	$(HOSTCC) $(HOSTFLAGS) -I$(S43)/include $^ -o $@
test: build/test_stage45 build/test_stage45_trace
	./build/test_stage45
	./build/test_stage45_trace
sanitize: | build
	$(HOSTCC) $(HOSTFLAGS) -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -I$(S43)/include tests/test_stage45_trace.c src/spmp_stage45_trace.c $(S43)/src/spmp_bootlog.c -o build/test_stage45_trace_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage45_trace_asan
	$(HOSTCC) $(HOSTFLAGS) -O1 -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage45.c src/spmp_stage45_sd_poll.c -o build/test_stage45_asan
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage45_asan
arm: | build
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage45_sd_poll.c -o build/spmp_stage45_sd_poll_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -I$(S43)/include -c src/spmp_stage45_trace.c -o build/spmp_stage45_trace_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -I$(S43)/include -c $(S43)/src/spmp_bootlog.c -o build/spmp_stage43_bootlog_ARMv5.o
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage45_trace_bundle_ARMv5.o build/spmp_stage45_trace_ARMv5.o build/spmp_stage45_sd_poll_ARMv5.o build/spmp_stage43_bootlog_ARMv5.o
	@if nm -u build/spmp_stage45_trace_bundle_ARMv5.o | grep -q ' U '; then echo 'unexpected ARM undefined symbols'; exit 1; fi
integrate: arm
	$(MAKE) -C $(S44) arm
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage45_stage44_integrated_ARMv5.o $(S44)/build/spmp_stage44_with_logger_armv5.o build/spmp_stage45_trace_ARMv5.o build/spmp_stage45_sd_poll_ARMv5.o
	@if nm -u build/spmp_stage45_stage44_integrated_ARMv5.o | grep -q ' U '; then echo 'unexpected unresolved references'; exit 1; fi
clean:
	rm -rf build
