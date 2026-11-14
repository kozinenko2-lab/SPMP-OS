# Link dependencies must be declared BEFORE GNU Make expands prerequisite lists.
FCEUMM_DIR = third_party/fceumm
FCEUMM_INC = $(FCEUMM_DIR)/src/drivers/libretro/libretro-common/include
FCEUMM_STATIC = $(FCEUMM_DIR)/libfceumm_spmp.a
FCEU_COMMON_LIB = build/libfceumm_common.a

Z26_DIR = third_party/z26
Z26_FLAGS = -Dlatch=spmp_z26_latch -Ithird_party/z26 -Wno-implicit-function-declaration -Wno-old-style-definition -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-implicit-fallthrough -Wno-unused-but-set-variable -Wno-format-security
TGEMU_DIR = third_party/tgemu/src
TGEMU_SRC = $(addprefix $(TGEMU_DIR)/,cpu/h6280.c pce.c psg.c render.c system.c vce.c vdc.c)
TGEMU_FLAGS = -DLSB_FIRST -DFAST_MEM -DSPMP_PORTABLE_C -DSPMP_TGEMU_NO_ZIP -Ithird_party/tgemu/src -Ithird_party/tgemu/src/cpu
CC ?= cc
CLANG ?= clang
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude -I.
SMSPLUS_BASE = third_party/smsplus
SMSPLUS_SOURCES = $(addprefix $(SMSPLUS_BASE)/,system.c sms.c vdp.c pio.c render.c tms.c memz80.c loadrom.c cpu/z80.c sound/sms_sound.c sound/sn76489.c)
SMSPLUS_SO = $(SMSPLUS_BASE)/libsms_spmp.so
SMSPLUS_INC = -I$(SMSPLUS_BASE)/compat -I$(SMSPLUS_BASE) -I$(SMSPLUS_BASE)/cpu -I$(SMSPLUS_BASE)/sound
SMSPLUS_FLAGS = -DIS_LITTLE_ENDIAN=1 -Wno-strict-aliasing -Wno-unused-parameter -Wno-error -Wno-pedantic
HOST_SRC = src/core.c src/render.c src/host/main.c src/host/posix_fs.c src/apps.c src/theme.c src/buttons.c src/media.c src/cores/registry.c src/cores/diagnostic.c src/cores/chip8.c src/cores/nes_nrom.c src/cores/fceumm_adapter.c src/cores/gambatte_adapter.c src/cores/tgemu_adapter.c src/cores/sms_host_loader.c src/cores/libretro_multi_adapter.c src/platform/spmp_small_video.c src/cores/megadrive_rom.c src/cores/tgemu_storage.c src/cores/z26_adapter.c src/ui/spmp_game_menu.c src/ui/spmp_settings.c src/ui/spmp_menu_bridge.c src/ui/spmp_ui_router.c src/platform/spmp_input_hal.c src/platform/spmp_hw_adapter.c $(TGEMU_SRC) src/host/core_cli.c
.PHONY: all host probe arm-core test clean
all: host probe arm-core arm-chip8 arm-pcengine arm-atari
build:
	mkdir -p build
host: build/spmp-os-host
build/spmp-os-host: $(HOST_SRC) include/defos.h include/spmp_theme.h include/spmp_apps.h include/spmp_media.h include/spmp_buttons.h include/spmp_cores.h $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-implicit-fallthrough -Wno-unused-but-set-variable $(HOST_SRC) -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
probe: build/ram_probe_armv5.o
build/ram_probe_armv5.o: src/probe/ram_probe.S | build
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -c $< -o $@
test: all test-cores
	python3 -m unittest discover -s tests -v
clean:
	rm -f build/spmp-os-host build/ram_probe_armv5.o build/safety_pattern_armv5.o build/*.ppm build/*.png
# Target code remains relocatable intentionally. No unverified RAM address.
arm-core: build/safety_pattern_armv5.o
build/safety_pattern_armv5.o: src/platform/safety_pattern.c include/defos_hal.h | build
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -ffreestanding -fno-builtin -O2 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c $< -o $@

# Compile only the standalone diagnostic core for ARMv5. No device boot address or HAL is implied.
arm-native-core: build/diagnostic_armv5.o
build/diagnostic_armv5.o: src/cores/diagnostic.c include/spmp_cores.h | build
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -ffreestanding -fno-builtin -O2 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c $< -o $@

TGEMU_SOURCE_ZIP ?= ../tgemu-spmp8000-master.zip
.PHONY: arm-tgemu-cpu
arm-tgemu-cpu: | build
	python3 tools/build_tgemu_cpu.py --archive "$(TGEMU_SOURCE_ZIP)" --output build/h6280_tgemu_armv5.o

# Small original CHIP-8 engine is genuinely compiled for the target CPU as an
# ET_REL module. Still NOT a runnable firmware, loader/HAL/syscalls unverified.
.PHONY: arm-chip8
arm-chip8: build/chip8_armv5.o
build/chip8_armv5.o: src/cores/chip8.c include/spmp_cores.h | build
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -ffreestanding -fno-builtin -O2 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c $< -o $@

.PHONY: test-cores
CORE_HARNESS_SRC = $(filter-out src/host/main.c src/host/core_cli.c,$(HOST_SRC))
build/atari_harness: $(CORE_HARNESS_SRC) tests/atari_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/atari_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
build/pcengine_harness: $(CORE_HARNESS_SRC) tests/pcengine_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/pcengine_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
build/pce_save_harness: $(CORE_HARNESS_SRC) tests/pce_save_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/pce_save_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
build/nes_harness: src/cores/nes_nrom.c tests/nes_harness.c include/spmp_cores.h | build
	$(CC) $(CFLAGS) src/cores/nes_nrom.c tests/nes_harness.c -o $@
test-cores: build/atari_harness build/pcengine_harness build/pce_save_harness build/nes_harness build/fceumm_harness build/gambatte_harness
	./build/atari_harness
	./build/pcengine_harness
	./build/pce_save_harness
	./build/nes_harness
	./build/fceumm_harness
	./build/gambatte_harness

# Standalone relocatable ARMv5 targets for eventual device HAL linking.
# No RedBoot entry point or flash writes; these are ET_REL, not firmware files.
ARM_FLAGS = --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -O2
ARM_TGEMU_SRC = src/cores/tgemu_adapter.c src/cores/sms_host_loader.c src/cores/libretro_multi_adapter.c src/platform/spmp_small_video.c src/cores/megadrive_rom.c src/cores/tgemu_storage.c $(TGEMU_SRC)
ARM_TGEMU_OBJ = $(patsubst %.c,build/arm6/%.o,$(ARM_TGEMU_SRC))
.PHONY: arm-pcengine arm-atari
build/arm6/%.o: %.c
	@mkdir -p $(dir $@)
	$(CLANG) $(ARM_FLAGS) $(TGEMU_FLAGS) -Iinclude -Itools/arm_shim -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -c $< -o $@
arm-pcengine: build/pcengine_armv5_stage6.o
build/pcengine_armv5_stage6.o: $(ARM_TGEMU_OBJ) | build
	ld.lld -m armelf -r $(ARM_TGEMU_OBJ) -o $@
arm-atari: build/atari2600_armv5_stage6.o
build/atari2600_armv5_stage6.o: src/cores/z26_adapter.c $(wildcard third_party/z26/*.c) include/spmp_cores.h | build
	$(CLANG) $(ARM_FLAGS) -std=gnu11 -Iinclude -Itools/arm_z26_shim -Itools/arm_shim -Ithird_party/z26 -Wno-implicit-function-declaration -Wno-old-style-definition -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-implicit-int -Wno-format-security -Wno-deprecated-non-prototype -c $< -o $@

# Self-contained limited NES preview core for the ARM926EJ-S.
.PHONY: arm-nes
arm-nes: build/nes_armv5_stage7.o
build/nes_armv5_stage7.o: src/cores/nes_nrom.c include/spmp_cores.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -c $< -o $@


# Genuine full FCEUmm engine from the user-supplied GPL source archive.
# Disabled HD pack/NTSC effects preserve RAM and RGB565 output on small devices.
# HOST build only. A full ARMv5+libc port still needs the verified console HAL.
FCEU_COMMON_BASE = $(FCEUMM_DIR)/src/drivers/libretro/libretro-common
FCEU_COMMON_REL = compat/compat_posix_string.c compat/compat_snprintf.c \
 compat/compat_strcasestr.c compat/compat_strl.c compat/fopen_utf8.c \
 encodings/encoding_utf.c file/file_path.c file/file_path_io.c \
 streams/file_stream.c streams/file_stream_transforms.c \
 string/stdstring.c time/rtime.c vfs/vfs_implementation.c
FCEU_COMMON_OBJ = $(patsubst %.c,build/fceumm-common/%.o,$(FCEU_COMMON_REL))
.PHONY: fceumm-static arm-fceumm-cpu
fceumm-static: $(FCEUMM_STATIC) $(FCEU_COMMON_LIB)
$(FCEUMM_STATIC):
	$(MAKE) -C $(FCEUMM_DIR) -f Makefile.libretro -j4 platform=unix STATIC_LINKING=1 \
		TARGET=libfceumm_spmp.a HAVE_NTSC=0 HAVE_HDPACK=0 WANT_32BPP=0 GIT_VERSION=' unknown'
build/fceumm-common/%.o: $(FCEU_COMMON_BASE)/%.c
	@mkdir -p $(dir $@)
	$(CC) -O2 -std=gnu11 -fPIC -D__LIBRETRO__ -I$(FCEU_COMMON_BASE)/include -c $< -o $@
$(FCEU_COMMON_LIB): $(FCEU_COMMON_OBJ) | build
	ar rcs $@ $(FCEU_COMMON_OBJ)
build/fceumm_harness: src/cores/fceumm_adapter.c tests/fceumm_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) -I$(FCEUMM_INC) src/cores/fceumm_adapter.c tests/fceumm_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
# Proven CPU-source ARMv5 compilation, NOT the full multi-mapper ARM core.
arm-fceumm-cpu: build/fceumm_x6502_armv5.o
build/fceumm_x6502_armv5.o: $(FCEUMM_DIR)/src/x6502.c | build
	$(CLANG) $(ARM_FLAGS) -D__LIBRETRO__ -DPATH_MAX=1024 -DFCEU_VERSION_NUMERIC=9900 -DFRONTEND_SUPPORTS_RGB565 -DSTATIC_LINKING \
		-Itools/arm_shim -I$(FCEUMM_DIR)/src -I$(FCEUMM_DIR)/src/drivers/libretro \
		-I$(FCEUMM_INC) -c $< -o $@

# Gambatte is a genuine GPLv2 GB/GBC libretro core, loaded on demand in the
# host prototype. The SPMP ARM5 module loader still needs implementation.
GAMBATTE_SO = third_party/gambatte/gambatte_libretro.so
$(GAMBATTE_SO):
	$(MAKE) -C third_party/gambatte -f Makefile.libretro platform=unix -j4
.PHONY: gambatte-host
gambatte-host: $(GAMBATTE_SO)

build/gambatte_harness: $(CORE_HARNESS_SRC) tests/gambatte_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/gambatte_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@

# Stage 10: ARMv5 ELF32 ET_REL overlay loader (host simulation; no Flash access).
OVERLAY_SRC = src/loader/spmp_overlay.c
OVERLAY_FLAGS = -fno-pic -fno-pie -fno-unwind-tables -fno-asynchronous-unwind-tables
.PHONY: arm-overlay overlay-tests stage10-check
build/probe_overlay_armv5.o: src/modules/probe_overlay.c include/spmp_overlay.h | build
	$(CLANG) $(ARM_FLAGS) $(OVERLAY_FLAGS) -std=c11 -Iinclude -c $< -o $@
build/probe_alt_armv5.o: src/modules/probe_overlay.c include/spmp_overlay.h | build
	$(CLANG) $(ARM_FLAGS) $(OVERLAY_FLAGS) -std=c11 -Iinclude -DSPMP_OVERLAY_CORE_ID=\"stage10-alt\" -c $< -o $@
build/overlay_harness: $(OVERLAY_SRC) tests/loader/overlay_harness.c include/spmp_overlay.h build/probe_overlay_armv5.o build/probe_alt_armv5.o | build
	$(CC) $(CFLAGS) $(OVERLAY_SRC) tests/loader/overlay_harness.c -o $@
arm-overlay: build/probe_overlay_armv5.o build/probe_alt_armv5.o
overlay-tests: build/overlay_harness
	./build/overlay_harness build/probe_overlay_armv5.o build/probe_alt_armv5.o
stage10-check: overlay-tests host test-cores
	python3 -m unittest discover -s tests -v

build/memory_harness: src/loader/spmp_memory.c tests/loader/memory_harness.c include/spmp_memory.h | build
	$(CC) $(CFLAGS) src/loader/spmp_memory.c tests/loader/memory_harness.c -o $@
.PHONY: memory-tests arm-module-runtime
memory-tests: build/memory_harness
	./build/memory_harness
arm-module-runtime: build/spmp_overlay_loader_armv5.o build/spmp_memory_armv5.o
build/spmp_overlay_loader_armv5.o: src/loader/spmp_overlay.c include/spmp_overlay.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -c $< -o $@
build/spmp_memory_armv5.o: src/loader/spmp_memory.c include/spmp_memory.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -c $< -o $@
stage10-check: memory-tests arm-module-runtime

build/runtime_harness: src/loader/spmp_runtime.c src/loader/spmp_overlay.c src/loader/spmp_memory.c tests/loader/runtime_harness.c include/spmp_runtime.h build/probe_overlay_armv5.o build/probe_alt_armv5.o | build
	$(CC) $(CFLAGS) src/loader/spmp_runtime.c src/loader/spmp_overlay.c src/loader/spmp_memory.c tests/loader/runtime_harness.c -o $@
.PHONY: runtime-tests
runtime-tests: build/runtime_harness
	./build/runtime_harness build/probe_overlay_armv5.o build/probe_alt_armv5.o
build/spmp_runtime_armv5.o: src/loader/spmp_runtime.c include/spmp_runtime.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -c $< -o $@
arm-module-runtime: build/spmp_runtime_armv5.o
stage10-check: runtime-tests

# Stage 15: portable unified emulator UI, verified host storage, target-safe ARMv5 objects.
STAGE15_SRC = src/ui/spmp_game_menu.c src/ui/spmp_settings.c src/platform/spmp_input_hal.c
.PHONY: stage15-check stage15-arm stage15-ui-tests
build/stage15_menu_harness: tests/ui/menu_harness.c $(STAGE15_SRC) src/host/spmp_ui_posix.c src/render.c src/theme.c | build
	$(CC) $(CFLAGS) tests/ui/menu_harness.c $(STAGE15_SRC) src/host/spmp_ui_posix.c src/render.c src/theme.c src/apps.c -o $@
stage15-ui-tests: build/stage15_menu_harness build/stage15_router_harness build/stage15_live_cores_harness build/stage15_layout_harness build/stage15_chip8_state_harness
	./build/stage15_menu_harness
	./build/stage15_router_harness
	./build/stage15_live_cores_harness
	./build/stage15_layout_harness
	./build/stage15_chip8_state_harness
build/spmp_menu_armv5.o: src/ui/spmp_game_menu.c include/spmp_game_menu.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
build/spmp_input_hal_armv5.o: src/platform/spmp_input_hal.c include/spmp_input_hal.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
build/spmp_menu_bridge_armv5.o: src/ui/spmp_menu_bridge.c include/spmp_game_menu.h include/spmp_cores.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
stage15-arm: build/spmp_menu_armv5.o build/spmp_input_hal_armv5.o build/spmp_menu_bridge_armv5.o arm-module-runtime
stage15-check: stage15-ui-tests stage15-arm stage10-check

build/stage15_router_harness: tests/ui/router_harness.c src/ui/spmp_ui_router.c src/ui/spmp_game_menu.c src/ui/spmp_settings.c src/platform/spmp_input_hal.c src/platform/spmp_hw_adapter.c | build
	$(CC) $(CFLAGS) tests/ui/router_harness.c src/ui/spmp_ui_router.c src/ui/spmp_game_menu.c src/ui/spmp_settings.c src/platform/spmp_input_hal.c src/platform/spmp_hw_adapter.c -o $@
build/spmp_hw_adapter_armv5.o: src/platform/spmp_hw_adapter.c include/spmp_hw_adapter.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
build/spmp_ui_router_armv5.o: src/ui/spmp_ui_router.c include/spmp_ui_router.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
stage15-arm: build/spmp_hw_adapter_armv5.o build/spmp_ui_router_armv5.o

build/spmp_menu_render_armv5.o: src/render.c include/spmp_game_menu.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
stage15-arm: build/spmp_menu_render_armv5.o
build/stage15_live_cores_harness: $(CORE_HARNESS_SRC) tests/ui/live_cores_harness.c src/host/spmp_ui_posix.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/ui/live_cores_harness.c src/host/spmp_ui_posix.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
build/stage15_layout_harness: tests/ui/layout_harness.c src/platform/spmp_hal_layout.c include/spmp_hal_layout.h | build
	$(CC) $(CFLAGS) tests/ui/layout_harness.c src/platform/spmp_hal_layout.c -o $@
build/spmp_hal_layout_armv5.o: src/platform/spmp_hal_layout.c include/spmp_hal_layout.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
stage15-arm: build/spmp_hal_layout_armv5.o
build/stage15_chip8_state_harness: tests/ui/chip8_state_harness.c src/cores/chip8.c include/spmp_cores.h | build
	$(CC) $(CFLAGS) tests/ui/chip8_state_harness.c src/cores/chip8.c -o $@

# Stage 16: per-core settings persisted in versioned CRC-checked config files.
.PHONY: stage16-settings-test stage16-arm stage16-check
build/settings_harness: tests/ui/settings_harness.c src/ui/spmp_settings.c include/spmp_settings.h | build
	$(CC) $(CFLAGS) tests/ui/settings_harness.c src/ui/spmp_settings.c -o $@
stage16-settings-test: build/settings_harness
	./build/settings_harness
build/spmp_settings_armv5.o: src/ui/spmp_settings.c include/spmp_settings.h | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Iinclude -Itools/arm_shim -Wall -Wextra -Werror -c $< -o $@
stage16-arm: stage15-arm build/spmp_settings_armv5.o
stage16-check: stage16-settings-test stage16-arm stage15-check

# Stage16: SMS Plus GX from Retro-Go; true Z80/VDP/PSG code, HOST verified.
.PHONY: sms-test sms-arm
build/sms_harness: $(CORE_HARNESS_SRC) tests/sms_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/sms_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
sms-test: build/sms_harness
	./build/sms_harness
# Full target core - statically relocatable, still NOT an executable firmware.
sms-arm: | build
	@mkdir -p build/sms-arm
	@set -e; for src in $(SMSPLUS_SOURCES) src/cores/smsplus_adapter.c; do \
	  obj=build/sms-arm/$$(basename $${src%.c}).o; \
	  $(CLANG) $(ARM_FLAGS) $(OVERLAY_FLAGS) -std=gnu11 -Itools/arm_sms_shim -Iinclude $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -Wno-unknown-attributes -Wno-unused-variable -Wno-unused-parameter -c $$src -o $$obj; \
	done
	ld.lld -r -m armelf_linux_eabi build/sms-arm/*.o -o build/spmp_sms_armv5.o
stage16-check: sms-test sms-arm

$(SMSPLUS_SO): $(SMSPLUS_SOURCES) src/cores/smsplus_adapter.c include/spmp_cores.h
	$(CC) -O2 -std=gnu11 -fPIC -shared -fvisibility=hidden -Wl,-Bsymbolic -DIS_LITTLE_ENDIAN=1 -Iinclude $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(SMSPLUS_SOURCES) src/cores/smsplus_adapter.c -lm -o $@

.PHONY: settings-preview
build/settings_preview: tests/ui/settings_preview.c $(STAGE15_SRC) src/render.c src/theme.c src/apps.c | build
	$(CC) $(CFLAGS) tests/ui/settings_preview.c $(STAGE15_SRC) src/render.c src/theme.c src/apps.c -o $@
settings-preview: build/settings_preview
	./build/settings_preview

.PHONY: sms-asan
build/sms_direct_asan: tests/sms_direct_harness.c src/cores/smsplus_adapter.c $(SMSPLUS_SOURCES) | build
	$(CC) -g -O2 -fno-strict-aliasing -std=gnu11 -fsanitize=address,undefined -fno-omit-frame-pointer -DIS_LITTLE_ENDIAN=1 -Iinclude $(SMSPLUS_INC) -Wno-error $(SMSPLUS_SOURCES) src/cores/smsplus_adapter.c tests/sms_direct_harness.c -lm -o $@
sms-asan: build/sms_direct_asan
	ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 ./build/sms_direct_asan


# Stage17 host Snes9x from user source. Mainline is heavy, not yet viable on ARMv5.
.PHONY: stage17-snes stage17-test
stage17-snes:
	$(MAKE) -C third_party/snes9x/libretro -j4 LTO= platform=unix
build/stage17_snes_harness: $(CORE_HARNESS_SRC) tests/stage17_snes_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -I. -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/stage17_snes_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
stage17-test: stage17-snes build/stage17_snes_harness
	./build/stage17_snes_harness


.PHONY: stage17-md stage17-arm-md
build/stage17_md_harness: src/cores/megadrive_rom.c tests/stage17_md_harness.c include/spmp_md_rom.h | build
	$(CC) $(CFLAGS) src/cores/megadrive_rom.c tests/stage17_md_harness.c -o $@
stage17-md: build/stage17_md_harness
	./build/stage17_md_harness
stage17-arm-md: | build
	$(CLANG) $(ARM_FLAGS) -Iinclude -Itools/arm_shim -c src/cores/megadrive_rom.c -o build/spmp_md_rom_armv5.o

.PHONY: stage17-check stage17-catalog
stage17-catalog:
	python3 tools/stage17_firmware_catalog.py
stage17-check: stage17-test stage17-md stage17-arm-md stage17-catalog
	$(MAKE) -j4 stage16-check

# Stage 18: real Snes9x with bounded, exact-sized cartridge allocations.
.PHONY: stage18-test stage18-check
build/stage18_snes_memory_harness: $(CORE_HARNESS_SRC) tests/stage18_snes_memory_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -I. -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/stage18_snes_memory_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
stage18-test: stage17-snes build/stage18_snes_memory_harness
	./build/stage18_snes_memory_harness
stage18-check: stage17-check stage18-test
	python3 -m unittest discover -s tests -v
.PHONY: stage18-demo stage18-full-check
stage18-demo: host stage17-snes
	python3 tools/generate_stage18_snes_smoke.py
	./build/spmp-os-host --run-core snes --content samples/snes_stage18_smoke.sfc --frames 5 --screenshot build/stage18_snes_frame.ppm
stage18-full-check: stage18-check arm-module-runtime stage18-demo
	python3 -m unittest discover -s tests -v

# Stage 19: actual PicoDrive Mega Drive-only host core from supplied source.
.PHONY: stage19-picodrive stage19-md-test
PICODRIVE_FLAGS = platform=unix GIT_REVISION=-spmp19 NO_ARM_ASM=1 use_cyclone=0 use_fame=1 use_drz80=0 use_cz80=1 use_svpdrc=0 use_sh2drc=0 use_libchdr=0 no_sms=1
stage19-picodrive:
	$(MAKE) -C third_party/picodrive -f Makefile.libretro -j4 $(PICODRIVE_FLAGS)
build/stage19_md_harness: $(CORE_HARNESS_SRC) tests/stage19_megadrive_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -I. -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/stage19_megadrive_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
stage19-md-test: stage19-picodrive build/stage19_md_harness
	python3 tools/generate_stage19_md_rom.py
	./build/stage19_md_harness samples/spmp_md_68000_test.md

# Selected actual PicoDrive C engine modules, not a complete ARMv5 console binary.
# Compiled with ARMv5TE software float. Assembly Cyclone68000 is absent from ZIP.
.PHONY: stage19-arm-md stage19-check stage19-demo
PICO_ARM_SRC = third_party/picodrive/pico/sound/sn76496.c third_party/picodrive/pico/sound/ym2612.c third_party/picodrive/cpu/fame/famec.c third_party/picodrive/cpu/cz80/cz80.c third_party/picodrive/pico/draw.c third_party/picodrive/pico/videoport.c third_party/picodrive/pico/sound/mix.c
PICO_ARM_FLAGS = -DNDEBUG -DEMU_F68K -D_USE_CZ80 -DFAMEC_NO_GOTOS -DNO_SMS -D__LIBRETRO__ -Itools/arm_picodrive_shim -Itools/arm_sms_shim -Itools/arm_shim -Ithird_party/picodrive
stage19-arm-md: | build
	@mkdir -p build/pico-arm
	@set -e; for src in $(PICO_ARM_SRC); do \
	  obj=build/pico-arm/$$(basename $${src%.c}).o; \
	  $(CLANG) $(ARM_FLAGS) $(PICO_ARM_FLAGS) -c $$src -o $$obj; \
	done
	ld.lld -r -m armelf_linux_eabi build/pico-arm/*.o -o build/spmp_picodrive_stage19_armv5_unresolved.o
	readelf -h build/spmp_picodrive_stage19_armv5_unresolved.o | grep -E 'ELF32|ARM|REL \('
stage19-demo: host stage19-picodrive
	python3 tools/generate_stage19_md_rom.py
	./build/spmp-os-host --run-core megadrive --content samples/spmp_md_68000_test.md --frames 8 --screenshot build/stage19_megadrive_frame.ppm
# Stage 10 runs all Python tests during its own regression. Prepare Stage 19
# artifacts FIRST so a truly clean source checkout is self-contained.
stage19-check:
	$(MAKE) -j4 stage19-picodrive
	$(MAKE) -j3 stage19-md-test stage19-arm-md
	$(MAKE) -j4 stage18-check
	$(MAKE) stage19-demo
	python3 -m unittest discover -s tests -v

# Stage 20: expanded ARMv5TE PicoDrive core build (relocatable only).
.PHONY: stage20-arm-picodrive stage20-check
stage20-arm-picodrive:
	python3 tools/build_picodrive_armv5_full.py
stage20-check: stage20-arm-picodrive stage19-md-test
	python3 -m unittest discover -s tests -v

# Stage 22: FULL PicoDrive CD+32X software path. No user firmware writes.
.PHONY: stage22-picodrive stage22-md-cd-32x stage22-arm stage22-check
stage22-picodrive:
	cmake -S third_party/picodrive/pico/cd/libchdr/new_upstream -B build/stage22_libchdr_host -DCHDR_WANT_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DWITH_SYSTEM_ZLIB=ON -DWITH_SYSTEM_ZSTD=ON
	cmake --build build/stage22_libchdr_host -j4
	$(MAKE) -C third_party/picodrive -f Makefile.libretro -j4 platform=unix GIT_REVISION=spmp22 NO_ARM_ASM=1 use_cyclone=0 use_fame=1 use_drz80=0 use_cz80=1 use_svpdrc=0 use_sh2drc=0 use_libchdr=0 no_32x=0 no_sms=1 SPMP_CHD_ENABLED=1 SPMP_SKIP_LTO=1
build/stage22_32x_harness: $(CORE_HARNESS_SRC) tests/stage22_32x_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -I. -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/stage22_32x_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
build/stage22_cd_harness: $(CORE_HARNESS_SRC) tests/stage22_cd_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) -I. -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable $(CORE_HARNESS_SRC) tests/stage22_cd_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
stage22-md-cd-32x: stage22-picodrive build/stage19_md_harness build/stage22_32x_harness build/stage22_cd_harness
	python3 tools/generate_stage19_md_rom.py
	python3 tools/generate_stage22_32x_rom.py
	python3 tools/generate_stage22_cd_media.py
	./build/stage19_md_harness samples/spmp_md_68000_test.md
	./build/stage22_32x_harness samples/spmp_32x_init_test.32x
	SPMP_BIOS_DIR=build/stage22_test_bios ./build/stage22_cd_harness samples/spmp_test_cd.cue
stage22-arm:
	python3 tools/build_picodrive_all_armv5.py
stage22-check: stage22-arm stage22-md-cd-32x
	python3 -m unittest discover -s tests -p 'test_stage22*' -v

# Reproducible Stage 22 check from source-only ZIP; original RedBoot is untouched.
.PHONY: stage22-release-check
stage22-release-check:
	$(MAKE) -j6 host test-cores arm-overlay arm-module-runtime
	$(MAKE) -j4 stage17-snes build/stage17_snes_harness build/stage18_snes_memory_harness
	$(MAKE) -j4 stage20-arm-picodrive
	@test -s build/picodrive_armv5_md_gc/report.json || python3 tools/build_picodrive_md_gc.py
	$(MAKE) -j4 stage22-check
	python3 -m unittest discover -s tests -v

# Stage 23: portable screen scaling and GBA ROM integrity preflight.
.PHONY: stage23-check stage23-arm
build/stage23_video_harness: tests/stage23_video_harness.c src/platform/spmp_small_video.c include/spmp_small_video.h | build
	$(CC) $(CFLAGS) tests/stage23_video_harness.c src/platform/spmp_small_video.c -o $@
build/stage23_gba_harness: tests/stage23_gba_harness.c src/cores/gba_cart.c include/spmp_gba_cart.h | build
	$(CC) $(CFLAGS) tests/stage23_gba_harness.c src/cores/gba_cart.c -o $@
stage23-arm: | build
	$(CLANG) $(ARM_FLAGS) -Iinclude -Itools/arm_shim -c src/platform/spmp_small_video.c -o build/stage23_video_armv5.o
	$(CLANG) $(ARM_FLAGS) -Iinclude -Itools/arm_shim -c src/cores/gba_cart.c -o build/stage23_gba_armv5.o
stage23-check: build/stage23_video_harness build/stage23_gba_harness stage23-arm
	./build/stage23_video_harness
	./build/stage23_gba_harness

# Stage24: original libretro gpSP, host interpreter. ARMv5 JIT must not be used.
.PHONY: stage24-gpsp stage24-arm stage24-test
stage24-gpsp:
	$(MAKE) -C third_party/gpsp -j4 HAVE_DYNAREC=0 GIT_VERSION=unknown
stage24-arm: | build
	$(CLANG) $(ARM_FLAGS) -Iinclude -Itools/arm_shim -c src/cores/gba_cart.c -o build/stage24_gba_cart_armv5.o
stage24-test: stage24-gpsp stage24-arm host
	python3 -m unittest discover -s tests -p 'test_stage24*.py' -v

build/stage24_gba_harness: $(CORE_HARNESS_SRC) tests/stage24_gba_harness.c $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) $(GAMBATTE_SO) $(SMSPLUS_SO) | build
	$(CC) $(CFLAGS) $(TGEMU_FLAGS) $(Z26_FLAGS) $(SMSPLUS_INC) $(SMSPLUS_FLAGS) $(CORE_HARNESS_SRC) tests/stage24_gba_harness.c -I$(FCEUMM_INC) $(FCEUMM_STATIC) $(FCEU_COMMON_LIB) -lm -ldl -o $@
stage24-integration: stage24-gpsp build/stage24_gba_harness
	./build/stage24_gba_harness tests/roms/stage24_gba_test.gba

# Stage 26: freestanding per-core bounded heap; does not replace libc malloc
# until each emulator explicitly adopts the allocator.
.PHONY: stage26-check stage26-arm-runtime
build/stage26_heap_harness: tests/heap_harness.c src/runtime/spmp_heap.c include/spmp_heap.h | build
	$(CC) -std=c11 -O2 -Wall -Wextra -Werror -Iinclude $< src/runtime/spmp_heap.c -o $@
build/stage26_heap_armv5.o: src/runtime/spmp_heap.c include/spmp_heap.h | build
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -c $< -o $@
stage26-arm-runtime: build/stage26_heap_armv5.o
stage26-check: build/stage26_heap_harness stage26-arm-runtime
	./build/stage26_heap_harness
	file build/stage26_heap_armv5.o

# Stage 27: gpSP ARM7TDMI interpreter compiled to ARMv5 EABI5, with bounded
# allocator and bundled free BIOS. Still requires platform/libc HAL symbols.
.PHONY: stage27-arm stage27-check
stage27-arm:
	python3 tools/build_gpsp_armv5.py
build/stage27/gpsp_heap_harness: tests/gpsp_heap_bridge_harness.c src/runtime/spmp_heap.c src/modules/gpsp/gpsp_heap_bridge.c include/spmp_heap.h src/modules/gpsp/gpsp_heap_bridge.h | build
	mkdir -p build/stage27
	$(CC) -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude -Isrc/modules/gpsp tests/gpsp_heap_bridge_harness.c src/runtime/spmp_heap.c src/modules/gpsp/gpsp_heap_bridge.c -o $@
stage27-check: stage27-arm build/stage27/gpsp_heap_harness
	$(CC) -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude -Isrc/modules/gpsp tests/gpsp_heap_bridge_harness.c src/runtime/spmp_heap.c src/modules/gpsp/gpsp_heap_bridge.c -o build/stage27/gpsp_heap_harness
	./build/stage27/gpsp_heap_harness
	python3 -m unittest discover -s tests -p 'test_stage27*' -v

# Stage 28: read-only streaming from SD/FAT HAL (no host POSIX dependencies).
.PHONY: stage28-file-check stage28-check
build/stage28_file_hal_harness: tests/gpsp_file_hal_harness.c src/runtime/spmp_gpsp_filestream.c include/spmp_filestream.h | build
	$(CC) -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude -Ithird_party/gpsp/libretro -Ithird_party/gpsp/libretro/libretro-common/include -Ithird_party/gpsp tests/gpsp_file_hal_harness.c src/runtime/spmp_gpsp_filestream.c -o $@
stage28-file-check: build/stage28_file_hal_harness
	./build/stage28_file_hal_harness
stage28-check: stage28-file-check stage27-check
	python3 -m unittest discover -s tests -p 'test_stage28*' -v

.PHONY: stage29-check
stage29-check:
	$(CC) -std=c11 -O1 -g -fno-builtin -fsanitize=address,undefined -fno-omit-frame-pointer tests/stage29_minilibc_harness.c src/runtime/spmp_minilibc.c -o build/stage29_minilibc_harness
	./build/stage29_minilibc_harness
	python3 tools/build_gpsp_armv5.py
	python3 -m unittest discover -s tests -p 'test_stage29*' -v

.PHONY: stage30-check
stage30-check:
	@mkdir -p build
	$(CC) -std=c11 -O1 -g -fno-builtin -fsanitize=address,undefined -fno-omit-frame-pointer src/runtime/spmp_numeric.c tests/stage30_numeric_harness.c -o build/stage30_numeric_harness
	./build/stage30_numeric_harness
	python3 tools/build_gpsp_armv5.py
	python3 -m unittest discover -s tests -p 'test_stage30*' -v
	python3 -m unittest discover -s tests -p 'test_stage29*' -v

# Stage31: 64-bit unsigned ARM EABI division, no platform dependencies.
.PHONY: stage31-check
stage31-check:
	@mkdir -p build
	$(CC) -O1 -g -std=c11 -fsanitize=address,undefined -fno-omit-frame-pointer src/runtime/spmp_aeabi_uldivmod.c tests/stage31_uldiv_harness.c -o build/stage31_uldiv_harness
	./build/stage31_uldiv_harness
	python3 tools/build_gpsp_armv5.py
	python3 -m unittest discover -s tests -p 'test_stage31*' -v
	python3 -m unittest discover -s tests -p 'test_stage30*' -v
	python3 -m unittest discover -s tests -p 'test_stage29*' -v

# Stage32 — real HAL-backed gpSP logging/RTC facade and scoped cheat parser.
.PHONY: stage32-check
stage32-check:
	mkdir -p build
	$(CC) -std=c11 -O1 -g -fno-builtin -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude -Itools/arm_gpsp_shim tests/stage32_services_harness.c src/runtime/spmp_gpsp_services.c src/runtime/spmp_gpsp_time.c -o build/stage32_services_harness
	./build/stage32_services_harness
	python3 -m unittest discover -s tests -p 'test_stage32*' -v

# Stage33 — software IEEE-754 subset for gpSP ARMv5 EABI; host/native fuzz,
# ARM ELF ABI and all remaining relocatable imports.
.PHONY: stage33-check
stage33-check:
	@mkdir -p build
	$(CC) -std=c11 -O2 -Wall -Wextra -fno-fast-math -fno-builtin tests/stage33_softfloat_harness.c src/runtime/spmp_aeabi_softfloat.c -lm -o build/stage33_softfloat_harness
	./build/stage33_softfloat_harness
	python3 -m unittest discover -s tests -p 'test_stage33*' -v

# Stage34: board-agnostic, read-only hardware discovery + explicit RAM ownership
# gate around Stage10 ELF relocator. NO RedBoot command, no flash programmer,
# no fixed console address, no device-side execution or MMIO.
STAGE34_CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror -Iinclude
.PHONY: stage34-check stage34-arm
build/stage34_boot_harness: tests/loader/stage34_boot_harness.c src/loader/spmp_boot_gate.c src/loader/spmp_overlay.c include/spmp_boot_gate.h build/probe_overlay_armv5.o | build
	$(CC) $(STAGE34_CFLAGS) src/loader/spmp_boot_gate.c src/loader/spmp_overlay.c tests/loader/stage34_boot_harness.c -o $@
build/stage34_hal_fs_harness: tests/loader/stage34_hal_fs_harness.c src/platform/spmp_diag_hal.c src/platform/spmp_ro_fs.c | build
	$(CC) $(STAGE34_CFLAGS) src/platform/spmp_diag_hal.c src/platform/spmp_ro_fs.c tests/loader/stage34_hal_fs_harness.c -o $@
stage34-arm: | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/loader/spmp_boot_gate.c -o build/spmp_boot_gate_armv5.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/platform/spmp_diag_hal.c -o build/spmp_diag_hal_armv5.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/platform/spmp_ro_fs.c -o build/spmp_ro_fs_armv5.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/loader/spmp_overlay.c -o build/spmp_overlay_stage34_armv5.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/runtime/spmp_minilibc.c -o build/spmp_minilibc_stage34_armv5.o
	ld.lld -m armelf -r build/spmp_boot_gate_armv5.o build/spmp_overlay_stage34_armv5.o build/spmp_diag_hal_armv5.o build/spmp_ro_fs_armv5.o build/spmp_minilibc_stage34_armv5.o -o build/spmp_stage34_ram_gate_armv5.o
	@test -z "$$(nm -u build/spmp_stage34_ram_gate_armv5.o)" || { echo "Stage34: unresolved ARM imports"; nm -u build/spmp_stage34_ram_gate_armv5.o; exit 1; }
stage34-check: build/stage34_boot_harness build/stage34_hal_fs_harness stage34-arm
	./build/stage34_boot_harness build/probe_overlay_armv5.o
	./build/stage34_hal_fs_harness
	python3 -m unittest discover -s tests -p 'test_stage33*' -v

# Stage40: read-only dual-panel UI backed by verified Stage34 FS injection.
# Writes only to a caller-owned 320x240 RAM surface. NEVER touches OEM LCD/MMIO.
STAGE40_RESEARCH = ../research/oem_abi_stage40
STAGE39_RESEARCH = ../research/oem_abi_stage39
STAGE40_SOURCES = src/ui/spmp_stage40_file_ui.c src/platform/spmp_ro_fs.c $(STAGE39_RESEARCH)/src/spmp_stage39_lcd_pcm.c
STAGE40_INC = -Iinclude -I$(STAGE39_RESEARCH)/src
.PHONY: stage40-ui-test stage40-arm stage40-verify
build/stage40_ui_test: $(STAGE40_SOURCES) $(STAGE40_RESEARCH)/tests/test_ui.c include/spmp_stage40_file_ui.h | build
	$(CLANG) -std=c11 -O2 -Wall -Wextra -Werror $(STAGE40_INC) $(STAGE40_SOURCES) $(STAGE40_RESEARCH)/tests/test_ui.c -o $@
stage40-ui-test: build/stage40_ui_test
	./build/stage40_ui_test
build/stage40_arm/%.o: | build
	@mkdir -p build/stage40_arm
	$(CLANG) --target=armv5te-none-eabi -mcpu=arm926ej-s -marm -mfloat-abi=soft -ffreestanding -fno-builtin -O2 -std=c11 -Wall -Wextra -Werror -Itools/arm_shim $(STAGE40_INC) -c $(STAGE40_ARM_$*) -o $@
STAGE40_ARM_ui = src/ui/spmp_stage40_file_ui.c
STAGE40_ARM_fs = src/platform/spmp_ro_fs.c
STAGE40_ARM_pixels = $(STAGE39_RESEARCH)/src/spmp_stage39_lcd_pcm.c
STAGE40_ARM_libc = src/runtime/spmp_minilibc.c
build/spmp_stage40_readonly_ui_armv5.o: build/stage40_arm/ui.o build/stage40_arm/fs.o build/stage40_arm/pixels.o build/stage40_arm/libc.o | build
	ld.lld -m armelf -r $^ -o $@
stage40-arm: build/spmp_stage40_readonly_ui_armv5.o
stage40-verify: stage40-ui-test stage40-arm
	@if test -n "$(SPMP_RAP_PATH)"; then SPMP_RAP_PATH="$(SPMP_RAP_PATH)" python3 -m unittest discover -s $(STAGE40_RESEARCH)/tests -p 'test_trace.py' -v; else echo 'Optional RAP trace: set SPMP_RAP_PATH=/path/to/8000_MMI.RAP'; fi

# Measured MMI input foundation. Pure callbacks; linked NGame target lives in research.
MMI_PORT_SRC = src/platform/spmp_mmi_input.c src/ui/spmp_mmi_shell.c
.PHONY: mmi-input-check mmi-input-arm
build/mmi_input_harness: tests/ui/mmi_input_harness.c $(MMI_PORT_SRC) include/spmp_mmi_input.h include/spmp_mmi_shell.h include/spmp_mmi_user01_profile.h | build
	$(CC) $(CFLAGS) tests/ui/mmi_input_harness.c $(MMI_PORT_SRC) -o $@
mmi-input-check: build/mmi_input_harness
	./build/mmi_input_harness

MMI_FILES_SRC = src/platform/spmp_sd_directory.c src/ui/spmp_mmi_files.c features/filemanager/FM01/src/fm01_core.c
MMI_FILES_INC = -Ifeatures/filemanager/FM01/include
.PHONY: mmi-files-check
build/mmi_files_harness: tests/ui/mmi_files_harness.c $(MMI_FILES_SRC) include/spmp_mmi_files.h include/spmp_sd_directory.h | build
	$(CC) $(CFLAGS) $(MMI_FILES_INC) tests/ui/mmi_files_harness.c $(MMI_FILES_SRC) -o $@
mmi-files-check: build/mmi_files_harness
	./build/mmi_files_harness

.PHONY: ttf-check
build/ttf_harness: tests/ui/ttf_harness.c src/ui/spmp_ttf.c include/spmp_ttf.h | build
	$(CC) $(CFLAGS) tests/ui/ttf_harness.c src/ui/spmp_ttf.c -o $@
ttf-check: build/ttf_harness
	./build/ttf_harness
mmi-input-arm: | build
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -c src/platform/spmp_mmi_input.c -o build/spmp_mmi_input_armv5.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -Wall -Wextra -Werror -Iinclude -c src/ui/spmp_mmi_shell.c -o build/spmp_mmi_shell_armv5.o

# Shared copy engine: test callbacks only, not linked into the device FILES app.
COPY_INC = -Iinclude -Ifeatures/filemanager/FM01/include -Ifeatures/filemanager/FM02/include
COPY_SRC = src/platform/spmp_file_copy.c src/platform/spmp_fm02_copy.c tests/ui/file_copy_bridge.c features/filemanager/FM01/src/fm01_core.c features/filemanager/FM02/src/fm02_core.c
COPY_HEADERS = include/spmp_file_copy.h include/spmp_fm02_copy.h features/filemanager/FM01/include/spmp_fm01.h features/filemanager/FM02/include/spmp_fm02.h
COPY_VM_PYTHON ?= python3
COPY_ASAN_RUNTIME ?= $(shell $(CC) -print-file-name=libasan.so)
.PHONY: file-copy-check file-copy-arm-check file-copy-sanitize-check
build/libfilecopy_test.so: $(COPY_SRC) $(COPY_HEADERS) | build
	$(CC) $(CFLAGS) $(COPY_INC) -fPIC -shared $(COPY_SRC) -o $@
file-copy-check: build/libfilecopy_test.so
	python3 ../../research/boot_reconstruction/tools/test_file_copy.py --library $< --output build/file_copy_host.json
build/libfilecopy_test_asan.so: $(COPY_SRC) $(COPY_HEADERS) | build
	$(CC) $(CFLAGS) $(COPY_INC) -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fPIC -shared $(COPY_SRC) -o $@
file-copy-sanitize-check: build/libfilecopy_test_asan.so
	LD_PRELOAD="$(COPY_ASAN_RUNTIME)" ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 python3 ../../research/boot_reconstruction/tools/test_file_copy.py --library $< --output build/file_copy_sanitizers.json
build/file_copy_arm/copy.elf: $(COPY_SRC) $(COPY_HEADERS) src/runtime/spmp_minilibc.c ../../research/boot_reconstruction/examples/native_startup/entry.S ../../research/boot_reconstruction/examples/native_startup/research.ld | build
	mkdir -p build/file_copy_arm
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(COPY_INC) -c src/platform/spmp_file_copy.c -o build/file_copy_arm/copy.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(COPY_INC) -c src/platform/spmp_fm02_copy.c -o build/file_copy_arm/bridge.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(COPY_INC) -c tests/ui/file_copy_bridge.c -o build/file_copy_arm/test.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(COPY_INC) -c features/filemanager/FM01/src/fm01_core.c -o build/file_copy_arm/fm01.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(COPY_INC) -c features/filemanager/FM02/src/fm02_core.c -o build/file_copy_arm/fm02.o
	$(CLANG) $(ARM_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror -Iinclude -Itools/arm_shim -c src/runtime/spmp_minilibc.c -o build/file_copy_arm/libc.o
	$(CLANG) $(ARM_FLAGS) -c ../../research/boot_reconstruction/examples/native_startup/entry.S -o build/file_copy_arm/entry.o
	ld.lld -T ../../research/boot_reconstruction/examples/native_startup/research.ld --no-undefined --fatal-warnings -Map=build/file_copy_arm/copy.map -o $@ build/file_copy_arm/copy.o build/file_copy_arm/bridge.o build/file_copy_arm/test.o build/file_copy_arm/fm01.o build/file_copy_arm/fm02.o build/file_copy_arm/libc.o build/file_copy_arm/entry.o
file-copy-arm-check: build/file_copy_arm/copy.elf
	$(COPY_VM_PYTHON) ../../research/boot_reconstruction/tools/test_file_copy.py --elf $< --output build/file_copy_arm/copy_vm.json
