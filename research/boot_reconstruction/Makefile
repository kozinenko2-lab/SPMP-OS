CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
.PHONY: all test clean arm-probe
all: build/mmp_inspect
build/mmp_inspect: src/mmp_inspect.c
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@
build/test_boot_policy: src/redboot_boot_policy.c src/redboot_boot_policy.h src/oem_layouts.h src/mmi_native_abi.h src/mmi_native_resources_abi.h tests/test_boot_policy.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/redboot_boot_policy.c tests/test_boot_policy.c -o $@
build/test_reloc_model: src/redboot_reloc_model.c src/redboot_reloc_model.h tests/test_reloc_model.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/redboot_reloc_model.c tests/test_reloc_model.c -o $@
build/test_flag_model: src/mmi_flag_model.c src/mmi_flag_model.h tests/test_flag_model.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/mmi_flag_model.c tests/test_flag_model.c -o $@
build/native_probe_host: examples/native_probe/probe.c examples/native_probe/probe.h examples/native_probe/host.c src/mmi_native_abi.h
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc -Iexamples/native_probe examples/native_probe/probe.c examples/native_probe/host.c -o $@
ARM_CC ?= clang
build/test_native_probe_adapter: examples/native_probe/probe.c examples/native_probe/oem_adapter.c examples/native_probe/oem_adapter.h examples/native_probe/adapter_test.c examples/native_probe/probe.h src/mmi_native_abi.h
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc -Iexamples/native_probe examples/native_probe/probe.c examples/native_probe/oem_adapter.c examples/native_probe/adapter_test.c -o $@
arm-probe:
	mkdir -p build
	$(ARM_CC) --target=arm-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -O2 -Isrc -Iexamples/native_probe -c examples/native_probe/probe.c -o build/native_probe_armv5.o
	$(ARM_CC) --target=arm-none-eabi -march=armv5te -mfloat-abi=soft -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -O2 -Isrc -Iexamples/native_probe -c examples/native_probe/oem_adapter.c -o build/native_probe_adapter_armv5.o
	python3 tools/verify_arm_object.py build/native_probe_armv5.o build/native_probe_adapter_armv5.o
	llvm-nm -u build/native_probe_armv5.o > build/native_probe_undefined.txt
	llvm-nm -u build/native_probe_adapter_armv5.o >> build/native_probe_undefined.txt
	test ! -s build/native_probe_undefined.txt
	llvm-readelf -h -A build/native_probe_armv5.o
test: all build/test_boot_policy build/test_reloc_model build/test_flag_model build/native_probe_host build/test_native_probe_adapter build/test_input_host
	./build/test_reloc_model
	./build/test_flag_model
	./build/test_boot_policy
	./build/native_probe_host --frame build/native_probe_frame.ppm
	./build/test_native_probe_adapter
	./build/test_input_host
	python3 -m unittest discover -s tests -v
clean:
	rm -rf build

# Analysis ELFs only. No target-ready container or original firmware dependency.
ARM_LD ?= ld.lld
ARM_NM ?= llvm-nm
STARTUP_FLAGS = --target=arm-none-eabi -march=armv5te -marm -mfloat-abi=soft -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -Wall -Wextra -Werror -O2
STARTUP_INCLUDES = -Isrc -Iexamples/native_probe -Iexamples/native_startup
STARTUP_DIR = build/native_startup
.PHONY: arm-startup
arm-startup:
	mkdir -p $(STARTUP_DIR)
	$(ARM_CC) --target=arm-none-eabi -march=armv5te -marm -mfloat-abi=soft -c examples/native_startup/entry.S -o $(STARTUP_DIR)/entry.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -c examples/native_startup/application.c -o $(STARTUP_DIR)/application.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -c tests/startup_fixture.c -o $(STARTUP_DIR)/fixture.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -c examples/native_probe/probe.c -o $(STARTUP_DIR)/probe.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -c examples/native_probe/oem_adapter.c -o $(STARTUP_DIR)/adapter.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -Map=$(STARTUP_DIR)/application.map -o $(STARTUP_DIR)/application.elf $(STARTUP_DIR)/entry.o $(STARTUP_DIR)/application.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -Map=$(STARTUP_DIR)/fixture.map -o $(STARTUP_DIR)/fixture.elf $(STARTUP_DIR)/entry.o $(STARTUP_DIR)/fixture.o $(STARTUP_DIR)/probe.o $(STARTUP_DIR)/adapter.o
	$(ARM_NM) -u $(STARTUP_DIR)/application.elf > $(STARTUP_DIR)/undefined.txt
	$(ARM_NM) -u $(STARTUP_DIR)/fixture.elf >> $(STARTUP_DIR)/undefined.txt
	test ! -s $(STARTUP_DIR)/undefined.txt

ARM_VM_PYTHON ?= python3
.PHONY: arm-startup-check
arm-startup-check: arm-startup
	python3 tools/startup_elf.py $(STARTUP_DIR)/application.elf > $(STARTUP_DIR)/application_layout.json
	python3 tools/startup_elf.py $(STARTUP_DIR)/fixture.elf > $(STARTUP_DIR)/fixture_layout.json
	python3 tools/check_startup_link_limits.py --cc $(ARM_CC) --ld $(ARM_LD) --build $(STARTUP_DIR) --output $(STARTUP_DIR)/negative_checks.json
	$(ARM_VM_PYTHON) tools/emulate_startup.py --application $(STARTUP_DIR)/application.elf --fixture $(STARTUP_DIR)/fixture.elf --output $(STARTUP_DIR)/arm_vm.json
	$(ARM_CC) --version > $(STARTUP_DIR)/compiler.txt
	$(ARM_LD) --version > $(STARTUP_DIR)/linker.txt

.PHONY: arm-native-bind
arm-native-bind: arm-startup
	$(ARM_CC) --target=arm-none-eabi -march=armv5te -marm -mfloat-abi=soft -DSPMP_NATIVE_PREFLIGHT -c examples/native_startup/entry.S -o $(STARTUP_DIR)/entry_bound.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Itests -c examples/native_startup/native_binding.c -o $(STARTUP_DIR)/binding.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Itests -DSPMP_BINDING_SYNTHETIC -c examples/native_startup/native_binding.c -o $(STARTUP_DIR)/binding_synthetic.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -c examples/native_startup/native_app.c -o $(STARTUP_DIR)/native_app.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -Map=$(STARTUP_DIR)/native.map -o $(STARTUP_DIR)/native.elf $(STARTUP_DIR)/entry_bound.o $(STARTUP_DIR)/binding.o $(STARTUP_DIR)/native_app.o $(STARTUP_DIR)/probe.o $(STARTUP_DIR)/adapter.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -o $(STARTUP_DIR)/native_synthetic.elf $(STARTUP_DIR)/entry_bound.o $(STARTUP_DIR)/binding_synthetic.o $(STARTUP_DIR)/native_app.o $(STARTUP_DIR)/probe.o $(STARTUP_DIR)/adapter.o
	$(ARM_NM) -u $(STARTUP_DIR)/native.elf > $(STARTUP_DIR)/native_undefined.txt
	test ! -s $(STARTUP_DIR)/native_undefined.txt

.PHONY: arm-native-check
arm-native-check: arm-native-bind
	python3 tools/startup_elf.py $(STARTUP_DIR)/native.elf > $(STARTUP_DIR)/native_layout.json
	python3 tools/ngame_pack.py $(STARTUP_DIR)/native.elf --output $(STARTUP_DIR)/SPMP_PROBE_VM.bin --manifest $(STARTUP_DIR)/native_package.json
	$(ARM_VM_PYTHON) tools/emulate_native_package.py --synthetic-elf $(STARTUP_DIR)/native_synthetic.elf --output $(STARTUP_DIR)/native_binding_vm.json

INPUT_DIR = build/input_test
.PHONY: arm-input
arm-input: arm-native-bind
	mkdir -p $(INPUT_DIR)
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Itests -DSPMP_INPUT_BUILD -c examples/native_startup/native_binding.c -o $(INPUT_DIR)/base_binding.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Itests -DSPMP_INPUT_BUILD -DSPMP_BINDING_SYNTHETIC -c examples/native_startup/native_binding.c -o $(INPUT_DIR)/base_synthetic.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Iexamples/input_test -Itests -c examples/input_test/input_binding.c -o $(INPUT_DIR)/binding.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Iexamples/input_test -Itests -DSPMP_BINDING_SYNTHETIC -c examples/input_test/input_binding.c -o $(INPUT_DIR)/binding_synthetic.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Iexamples/input_test -c examples/input_test/input_test.c -o $(INPUT_DIR)/input.o
	$(ARM_CC) $(STARTUP_FLAGS) $(STARTUP_INCLUDES) -Iexamples/input_test -c examples/input_test/main.c -o $(INPUT_DIR)/main.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -Map=$(INPUT_DIR)/input.map -o $(INPUT_DIR)/input.elf $(STARTUP_DIR)/entry_bound.o $(INPUT_DIR)/base_binding.o $(INPUT_DIR)/binding.o $(INPUT_DIR)/input.o $(INPUT_DIR)/main.o $(STARTUP_DIR)/adapter.o
	$(ARM_LD) -T examples/native_startup/research.ld --no-undefined --fatal-warnings --gc-sections -o $(INPUT_DIR)/input_synthetic.elf $(STARTUP_DIR)/entry_bound.o $(INPUT_DIR)/base_synthetic.o $(INPUT_DIR)/binding_synthetic.o $(INPUT_DIR)/input.o $(INPUT_DIR)/main.o $(STARTUP_DIR)/adapter.o
	python3 tools/ngame_pack.py $(INPUT_DIR)/input.elf --output $(INPUT_DIR)/SPMP_INPUT_SD0.bin --manifest $(INPUT_DIR)/input_package.json --title "SPMP INPUT SD0"

build/test_input_host: examples/input_test/input_test.c examples/input_test/input_test.h examples/input_test/font5x7.h examples/native_probe/oem_adapter.c tests/test_input_host.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc -Iexamples/native_probe -Iexamples/input_test examples/input_test/input_test.c examples/native_probe/oem_adapter.c tests/test_input_host.c -o $@

.PHONY: arm-input-check
arm-input-check: arm-input
	$(ARM_VM_PYTHON) tools/emulate_input.py --elf $(INPUT_DIR)/input_synthetic.elf --output $(INPUT_DIR)/input_vm.json --preview $(INPUT_DIR)/input_preview.ppm --log $(INPUT_DIR)/example.csv
	python3 tools/input_log_decode.py $(INPUT_DIR)/example.csv --device "SYNTHETIC VM" --output $(INPUT_DIR)/example_decoded.json
