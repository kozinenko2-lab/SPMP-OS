CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
.PHONY: all test clean
all: build/mmp_inspect
build/mmp_inspect: src/mmp_inspect.c
	mkdir -p build
	$(CC) $(CFLAGS) $< -o $@
test: all
	python3 -m unittest discover -s tests -v
clean:
	rm -rf build
