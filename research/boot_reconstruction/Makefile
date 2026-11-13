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
test: all build/test_boot_policy build/test_reloc_model build/test_flag_model build/native_probe_host build/test_native_probe_adapter
	./build/test_reloc_model
	./build/test_flag_model
	./build/test_boot_policy
	./build/native_probe_host --frame build/native_probe_frame.ppm
	./build/test_native_probe_adapter
	python3 -m unittest discover -s tests -v
clean:
	rm -rf build
