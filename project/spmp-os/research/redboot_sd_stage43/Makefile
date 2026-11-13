CC ?= clang
# Works both in the full Stage43 tree (sibling redboot_sd_stage42)
# and as a standalone patch beside the Stage42 working directory.
ifneq ($(wildcard ../redboot_sd_stage42/include/spmp_dualboot.h),)
S42 ?= ../redboot_sd_stage42
else
S42 ?= ../SPMP-OS_Stage42_RedBoot_SD_Prototype
endif
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -I$(S42)/include
ARMCC ?= clang
ARMFLAGS = -target armv5te-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -Os -Wall -Wextra -Werror -ffreestanding -fno-builtin -nostdlib -fno-unwind-tables -fno-asynchronous-unwind-tables -Iinclude -I$(S42)/include
.PHONY: test arm clean audit
build:
	mkdir -p build
build/test_bootlog: src/spmp_bootlog.c src/spmp_trace_bridge.c tests/test_bootlog.c | build
	$(CC) $(CFLAGS) $^ $(S42)/src/spmp_dualboot.c -o $@
build/test_mmi_store: src/spmp_bootlog.c src/spmp_mmi_trace_store.c tests/test_mmi_store.c | build
	$(CC) $(CFLAGS) $^ -o $@
build/test_sd_trace: src/spmp_bootlog.c src/spmp_sd_trace.c tests/test_sd_trace.c | build
	$(CC) $(CFLAGS) $^ -o $@
test: build/test_bootlog build/test_mmi_store build/test_sd_trace
	./build/test_bootlog
	./build/test_mmi_store
	./build/test_sd_trace
arm: | build
	$(ARMCC) $(ARMFLAGS) -c src/spmp_bootlog.c -o build/spmp_bootlog_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_trace_bridge.c -o build/spmp_trace_bridge_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_mmi_trace_store.c -o build/spmp_mmi_trace_store_ARMv5.o
	$(ARMCC) $(ARMFLAGS) -c src/spmp_sd_trace.c -o build/spmp_sd_trace_ARMv5.o
	$(ARMCC) -target armv5te-none-eabi -nostdlib -Wl,-r -o build/SPMP_Stage43_BootTrace_ARMv5.o $(S42)/build/SPMP_Stage42_SDReader_BootBridge_ARMv5.o build/spmp_bootlog_ARMv5.o build/spmp_trace_bridge_ARMv5.o build/spmp_mmi_trace_store_ARMv5.o build/spmp_sd_trace_ARMv5.o
	@if nm -u build/SPMP_Stage43_BootTrace_ARMv5.o | grep -q ' U '; then echo 'UNRESOLVED SYMBOLS!'; exit 1; fi
audit:
	python3 tools/analyze_oem_sd.py $(RAP) --outdir evidence
clean:
	rm -f build/test_* build/*.o
