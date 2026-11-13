CC ?= clang
CLANG ?= clang
CFLAGS=-std=c11 -O1 -Wall -Wextra -Werror -Iinclude
ARMFLAGS=-target armv5te-none-eabi -march=armv5te -mfloat-abi=soft -ffreestanding -fno-builtin -Os -Wall -Wextra -Werror -Iinclude
.PHONY: all test arm native-syntax clean
default: all
all: test arm native-syntax
build/test_mmi49: src/spmp_mmi49_logger.c tests/test_mmi49.c include/spmp_mmi49_logger.h
	$(CLANG) $(CFLAGS) src/spmp_mmi49_logger.c tests/test_mmi49.c -o $@
test: build/test_mmi49
	./build/test_mmi49
	$(CLANG) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer src/spmp_mmi49_logger.c tests/test_mmi49.c -o build/test_mmi49_asan
	./build/test_mmi49_asan
native-syntax:
	$(CLANG) -target armv5te-none-eabi -march=armv5te -mfloat-abi=soft -ffreestanding -fsyntax-only -Wall -Wextra -Werror -Itests/stubs native_probe/game.c
arm:
	$(CLANG) $(ARMFLAGS) -c src/spmp_mmi49_logger.c -o build/spmp_mmi49_logger_ARMv5.o
clean:
	rm -f build/test_mmi49 build/test_mmi49_asan build/spmp_mmi49_logger_ARMv5.o
