CC ?= clang
ARMCC ?= clang
# Standalone beside /mnt/data/SPMP-OS_Stage43_SDTrace, or inside full sources.
ifneq ($(wildcard ../redboot_sd_stage43/include/spmp_bootlog.h),)
S43 ?= ../redboot_sd_stage43
else
S43 ?= ../SPMP-OS_Stage43_SDTrace
endif
CFLAGS = -std=c11 -O2 -g -Wall -Wextra -Werror -Iinclude -I$(S43)/include
ARMFLAGS = --target=armv5te-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -Iinclude -I$(S43)/include
.PHONY: all test arm sanitize
all: test arm
build:
	mkdir -p build
build/test_stage44: tests/test_stage44.c src/spmp_stage44_handoff.c src/spmp_stage44_flush.c $(S43)/src/spmp_bootlog.c | build
	$(CC) $(CFLAGS) $^ -o $@
test: build/test_stage44
	./build/test_stage44
arm: | build
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage44_handoff.c -o build/spmp_stage44_handoff_armv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_stage44_flush.c -o build/spmp_stage44_flush_armv5.o
	$(ARMCC) $(ARMFLAGS) -c $(S43)/src/spmp_bootlog.c -o build/spmp_stage43_bootlog_armv5.o
	$(ARMCC) --target=armv5te-none-eabi -nostdlib -fuse-ld=lld -Wl,-r -o build/spmp_stage44_with_logger_armv5.o build/spmp_stage44_handoff_armv5.o build/spmp_stage44_flush_armv5.o build/spmp_stage43_bootlog_armv5.o
	@if nm -u build/spmp_stage44_with_logger_armv5.o | grep -q ' U '; then echo 'Unresolved ARM symbols'; exit 1; fi
sanitize: | build
	clang $(CFLAGS) -O1 -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_stage44.c src/spmp_stage44_handoff.c src/spmp_stage44_flush.c $(S43)/src/spmp_bootlog.c -o build/test_stage44_sanitized
	ASAN_OPTIONS=detect_leaks=1 ./build/test_stage44_sanitized
