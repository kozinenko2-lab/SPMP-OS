CC = clang
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror -Iinclude
ARM = clang --target=armv5te-none-eabi -mfloat-abi=soft -march=armv5te -ffreestanding -fno-builtin -fno-stack-protector -O2 -std=c11 -Iinclude
.PHONY: test arm

test:
	python3 tests/test_fat_reader.py
	$(CC) $(CFLAGS) src/spmp_dualboot.c tests/test_dualboot.c -o build/test_dualboot
	build/test_dualboot

arm:
	@mkdir -p build
	$(ARM) -c src/spmp_fat_reader.c -o build/spmp_fat_reader_ARMv5.o
	$(ARM) -c src/spmp_fat_bridge.c -o build/spmp_fat_bridge_ARMv5.o
	$(ARM) -c src/spmp_dualboot.c -o build/spmp_dualboot_ARMv5.o
	$(ARM) -c runtime_from_stage40/spmp_minilibc.c -o build/spmp_minilibc_ARMv5.o
	$(ARM) -c runtime_from_stage40/spmp_numeric.c -o build/spmp_numeric_ARMv5.o
	clang --target=armv5te-none-eabi -nostdlib -r build/spmp_fat_reader_ARMv5.o build/spmp_fat_bridge_ARMv5.o build/spmp_dualboot_ARMv5.o build/spmp_minilibc_ARMv5.o build/spmp_numeric_ARMv5.o -o build/SPMP_Stage42_SDReader_BootBridge_ARMv5.o
	@if llvm-objdump -t build/SPMP_Stage42_SDReader_BootBridge_ARMv5.o | grep -q '\*UND\*'; then echo 'ERROR: undefined ARM symbols'; exit 1; fi
