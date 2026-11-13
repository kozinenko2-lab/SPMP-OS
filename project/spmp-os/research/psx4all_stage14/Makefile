CC ?= cc
CLANG ?= clang
CFLAGS = -O2 -g -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror
PORT = vendor/psx4all-dingoo-master/src/spmp_port
BUILD = build
.PHONY: check bench armv5 clean asan
$(BUILD)/psx_opt_test: tests/test_optimizations.c $(PORT)/spmp_psx_opt.c $(PORT)/spmp_psx_opt.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $< $(PORT)/spmp_psx_opt.c -o $@
check: $(BUILD)/psx_opt_test $(BUILD)/psx_cd_test $(BUILD)/psx_bios_test
	./$(BUILD)/psx_opt_test
	./$(BUILD)/psx_cd_test
	./$(BUILD)/psx_bios_test
bench: $(BUILD)/psx_opt_test
	./$(BUILD)/psx_opt_test --bench
armv5:
	$(MAKE) -C $(PORT) -f Makefile.spmp-psx-armv5
asan:
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -O1 -fno-omit-frame-pointer -fsanitize=address,undefined tests/test_optimizations.c $(PORT)/spmp_psx_opt.c -o $(BUILD)/psx_opt_test_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_opt_test_san
clean:
	rm -rf $(BUILD)
$(BUILD)/psx_cd_test: tests/test_cdr.cpp vendor/psx4all-dingoo-master/src/cdr/oldversion/cdriso.cpp
	mkdir -p $(BUILD)
	$(CXX) -std=c++11 -O2 -g -DSPMP_PSX4ALL -DGP2X -Ivendor/psx4all-dingoo-master/src -Ivendor/psx4all-dingoo-master/src/pandora $^ -lz -o $@
check-cd: $(BUILD)/psx_cd_test
	./$(BUILD)/psx_cd_test

$(BUILD)/psx_cd_test_san: tests/test_cdr.cpp vendor/psx4all-dingoo-master/src/cdr/oldversion/cdriso.cpp
	mkdir -p $(BUILD)
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -DSPMP_PSX4ALL -DGP2X -Ivendor/psx4all-dingoo-master/src -Ivendor/psx4all-dingoo-master/src/pandora $^ -lz -o $@
asan-cd: $(BUILD)/psx_cd_test_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_cd_test_san

$(BUILD)/psx_bios_test: tests/test_bios.c $(PORT)/spmp_psx_bios.c $(PORT)/spmp_psx_bios.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $< $(PORT)/spmp_psx_bios.c -o $@
.PHONY: bios-check full-check
bios-check: $(BUILD)/psx_bios_test
	./$(BUILD)/psx_bios_test
full-check: check check-cd bios-check armv5

PSX_SRC = vendor/psx4all-dingoo-master/src
$(BUILD)/psx_mem_reset_test: tests/test_mem_reset.cpp $(PSX_SRC)/mem.cpp $(PORT)/spmp_psx_bios.c
	mkdir -p $(BUILD)
	$(CXX) -std=c++11 -g -O1 -ffunction-sections -fdata-sections -Wl,--gc-sections \
		-DSPMP_PSX4ALL -DGP2X -DPANDORA -DLSB_FIRST -I$(PSX_SRC) -I$(PSX_SRC)/pandora $^ -o $@
.PHONY: mem-bios-check
mem-bios-check: $(BUILD)/psx_mem_reset_test
	@test -n "$(BIOS_FILE)" || (echo 'BIOS_FILE must be an existing user-supplied file' && exit 1)
	./$(BUILD)/psx_mem_reset_test "$(BIOS_FILE)"


# Stage12 integration. The ARM build remains relocatable object code, not a
# linkable firmware, because the SPMP8000 sysroot and HAL are unverified.
.PHONY: stage12-check stage12-arm
stage12-check: check $(BUILD)/psx_stage12_test
	./$(BUILD)/psx_stage12_test
$(BUILD)/psx_stage12_test: tests/test_stage12.c $(PORT)/spmp_psx_cache_guard.c $(PORT)/spmp_psx_budget.c $(PORT)/spmp_psx_arm926_priv.c $(PORT)/spmp_psx_opt.c $(PORT)/spmp_memory.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(PORT) $^ -o $@
stage12-arm:
	mkdir -p $(BUILD)/armv5
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -O2 -ffreestanding -fno-builtin -c $(PORT)/spmp_psx_cache_guard.c -o $(BUILD)/armv5/spmp_psx_cache_guard.o
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -O2 -ffreestanding -fno-builtin -c $(PORT)/spmp_psx_budget.c -o $(BUILD)/armv5/spmp_psx_budget.o
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -O2 -ffreestanding -fno-builtin -DSPMP_PSX_ARM926_PRIVILEGED_CACHE -c $(PORT)/spmp_psx_arm926_priv.c -o $(BUILD)/armv5/spmp_psx_arm926_priv.o
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -O2 -ffreestanding -fno-builtin -Itools/arm_shim -c $(PORT)/spmp_memory.c -o $(BUILD)/armv5/spmp_memory.o

.PHONY: stage12-asan stage12-bundle
stage12-asan:
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -I$(PORT) tests/test_stage12.c $(PORT)/spmp_psx_cache_guard.c $(PORT)/spmp_psx_budget.c $(PORT)/spmp_psx_arm926_priv.c $(PORT)/spmp_psx_opt.c $(PORT)/spmp_memory.c -o $(BUILD)/psx_stage12_test_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_stage12_test_san
stage12-bundle: stage12-arm armv5
	ld.lld -r -m armelf_linux_eabi $(BUILD)/armv5/spmp_psx_opt.o $(BUILD)/armv5/psx_jit_cache_bridge.o $(BUILD)/armv5/spmp_psx_cache_guard.o $(BUILD)/armv5/spmp_psx_budget.o $(BUILD)/armv5/spmp_psx_arm926_priv.o $(BUILD)/armv5/spmp_memory.o -o $(BUILD)/armv5/psx_stage12_spmp_bridge.o
	readelf -h $(BUILD)/armv5/psx_stage12_spmp_bridge.o | grep -E 'Class:|Machine:|Type:'

.PHONY: stage12-core-audit stage12-full-check
stage12-core-audit:
	bash tools/audit_full_core.sh
	ar rcs $(BUILD)/libpsx4all_host_core_audit.a $(BUILD)/host-core-audit/*.o
stage12-full-check: stage12-check stage12-bundle stage12-core-audit stage12-asan

# Stage 13: SPMP-OS overlay-owned JIT allocations. Does not execute guest code.
.PHONY: stage13-check stage13-asan stage13-arm stage13-full-check stage13-core-audit
$(BUILD)/psx_stage13_test: tests/test_stage13.c $(PORT)/spmp_psx_jit_store.c $(PORT)/spmp_psx_jit_store.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(PORT) tests/test_stage13.c $(PORT)/spmp_psx_jit_store.c -o $@
stage13-check: $(BUILD)/psx_stage13_test
	./$(BUILD)/psx_stage13_test
stage13-asan:
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -I$(PORT) tests/test_stage13.c $(PORT)/spmp_psx_jit_store.c -o $(BUILD)/psx_stage13_test_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_stage13_test_san
stage13-arm:
	mkdir -p $(BUILD)/armv5
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -O2 -ffreestanding -fno-builtin -Itools/arm_shim -I$(PORT) -c $(PORT)/spmp_psx_jit_store.c -o $(BUILD)/armv5/spmp_psx_jit_store.o
	readelf -h $(BUILD)/armv5/spmp_psx_jit_store.o | grep -E 'Class:|Machine:|Type:'
stage13-core-audit:
	bash tools/audit_full_core.sh
	@test "$$(grep -c "pass" $(BUILD)/host-core-audit/RESULTS.tsv)" -eq 33
stage13-full-check: stage12-check stage13-check stage13-arm stage13-core-audit

# Stage14: actual emulated PS1 RAM reads/writes, mirrors, lifetime and cache isolation
.PHONY: stage14-memory-check stage14-memory-asan stage14-full-check
$(BUILD)/psx_stage14_mem: tests/test_mem_lifecycle.cpp $(PSX_SRC)/mem.cpp $(PORT)/spmp_psx_bios.c
	mkdir -p $(BUILD)
	$(CXX) -std=gnu++11 -O2 -g -fno-strict-aliasing -ffunction-sections -fdata-sections -Wl,--gc-sections -DGP2X -DPANDORA -DSPMP_PSX4ALL -DLSB_FIRST -I$(PSX_SRC) -I$(PSX_SRC)/pandora $^ -o $@
stage14-memory-check: $(BUILD)/psx_stage14_mem
	./$(BUILD)/psx_stage14_mem
stage14-memory-asan:
	mkdir -p $(BUILD)
	$(CXX) -std=gnu++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-strict-aliasing -ffunction-sections -fdata-sections -Wl,--gc-sections -DGP2X -DPANDORA -DSPMP_PSX4ALL -DLSB_FIRST -I$(PSX_SRC) -I$(PSX_SRC)/pandora tests/test_mem_lifecycle.cpp $(PSX_SRC)/mem.cpp $(PORT)/spmp_psx_bios.c -o $(BUILD)/psx_stage14_mem_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_stage14_mem_san
stage14-full-check: stage13-full-check stage14-memory-check stage14-memory-asan

$(BUILD)/psx_stage14_interpreter: tests/test_interpreter_cpu.cpp $(PSX_SRC)/interpreter/interpreter.cpp $(PSX_SRC)/mem.cpp $(PORT)/spmp_psx_bios.c
	mkdir -p $(BUILD)
	$(CXX) -std=gnu++11 -O2 -g -fno-strict-aliasing -ffunction-sections -fdata-sections -Wl,--gc-sections -DGP2X -DPANDORA -DINTERP -DSPMP_PSX4ALL -DLSB_FIRST -I$(PSX_SRC) -I$(PSX_SRC)/pandora $^ -o $@
.PHONY: stage14-cpu-check
stage14-cpu-check: $(BUILD)/psx_stage14_interpreter
	./$(BUILD)/psx_stage14_interpreter
stage14-full-check: stage14-cpu-check

.PHONY: stage14-armv5-core stage14-cpu-asan stage14-stress stage14-release-check
stage14-armv5-core:
	bash tools/build_stage14_armv5.sh
stage14-cpu-asan:
	mkdir -p $(BUILD)
	$(CXX) -std=gnu++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-strict-aliasing -ffunction-sections -fdata-sections -Wl,--gc-sections -DGP2X -DPANDORA -DINTERP -DSPMP_PSX4ALL -DLSB_FIRST -I$(PSX_SRC) -I$(PSX_SRC)/pandora tests/test_interpreter_cpu.cpp $(PSX_SRC)/interpreter/interpreter.cpp $(PSX_SRC)/mem.cpp $(PORT)/spmp_psx_bios.c -o $(BUILD)/psx_stage14_interpreter_san
	ASAN_OPTIONS=detect_leaks=1 ./$(BUILD)/psx_stage14_interpreter_san
stage14-stress: $(BUILD)/psx_stage14_interpreter
	PSX_CPU_STRESS=1 ./$(BUILD)/psx_stage14_interpreter
stage14-release-check: stage14-full-check stage14-cpu-asan stage14-stress stage14-armv5-core
