CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
.PHONY: all test clean
all: build/mmp_inspect
build/mmp_inspect: src/mmp_inspect.c
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@
build/test_boot_policy: src/redboot_boot_policy.c src/redboot_boot_policy.h src/oem_layouts.h src/mmi_native_abi.h tests/test_boot_policy.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/redboot_boot_policy.c tests/test_boot_policy.c -o $@
build/test_reloc_model: src/redboot_reloc_model.c src/redboot_reloc_model.h tests/test_reloc_model.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/redboot_reloc_model.c tests/test_reloc_model.c -o $@
build/test_flag_model: src/mmi_flag_model.c src/mmi_flag_model.h tests/test_flag_model.c
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc src/mmi_flag_model.c tests/test_flag_model.c -o $@
test: all build/test_boot_policy build/test_reloc_model build/test_flag_model
	./build/test_reloc_model
	./build/test_flag_model
	./build/test_boot_policy
	python3 -m unittest discover -s tests -v
clean:
	rm -rf build
