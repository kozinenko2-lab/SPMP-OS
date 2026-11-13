CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror
INCLUDE = -Iinclude
.PHONY: test clean
build/bridge_test: src/spmp_stock_bridge.c tests/bridge_test.c include/spmp_stock_bridge.h
	mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDE) src/spmp_stock_bridge.c tests/bridge_test.c -o $@
test: build/bridge_test
	./build/bridge_test
	python3 tests/test_audit.py
clean:
	rm -rf build
