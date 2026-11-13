CC ?= gcc
CLANG ?= clang
AR ?= llvm-ar
CFLAGS = -O2 -std=gnu11 -fno-strict-aliasing -DLITTLE_ENDIAN=1234 -DBIG_ENDIAN=4321 -DBYTE_ORDER=1234 -DHAVE_ALLOCA_H
INC = -Iinclude -Ivendor/Tremor -Ivendor/ogg/include
TREMOR = block codebook floor0 floor1 info mapping0 mdct registry res012 sharedbook synthesis vorbisfile window
OGG = bitwise framing
SOURCE = $(addprefix vendor/Tremor/,$(addsuffix .c,$(TREMOR))) $(addprefix vendor/ogg/src/,$(addsuffix .c,$(OGG))) src/spmp_audio_v5.c src/spmp_flac_lite.c src/spmp_audio_probe.c src/spmp_ima_adpcm.c src/spmp_audio_sink.c
HOST_OBJS = $(patsubst %.c,build/host/%.o,$(SOURCE))
ARM_OBJS = $(patsubst %.c,build/arm/%.o,$(SOURCE))
all: host
build/host/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
# ARM requires an external lightweight C runtime and an allocator, see PORTING_RU.
build/arm/%.o: %.c
	@mkdir -p $(dir $@)
	$(CLANG) --target=armv5te-none-eabi -mfloat-abi=soft -ffreestanding -Os -std=gnu11 -fno-strict-aliasing -DLITTLE_ENDIAN=1234 -DBIG_ENDIAN=4321 -DBYTE_ORDER=1234 -D_ARM_ASSEM_ -DHAVE_ALLOCA_H -I freestanding/include $(INC) -c $< -o $@
host: $(HOST_OBJS)
	ar rcs artifacts/libspmp_audio_v5_host.a $(HOST_OBJS)
	$(CC) -O2 $(INC) tests/audio_decode.c artifacts/libspmp_audio_v5_host.a -lm -o build/audio_decode
arm: $(ARM_OBJS)
	$(AR) rcs artifacts/libspmp_audio_v5_armv5.a $(ARM_OBJS)
clean:
	rm -rf build artifacts/*.a
.PHONY: all host arm clean

# Optional OpenCORE adapter: needs real OpenCORE decoder symbols at link time.
amr-host: host
	$(CC) -O2 $(INC) tests/audio_amr_decode.c src/spmp_amr_opencore.c artifacts/libspmp_audio_v5_host.a -l:libopencore-amrnb.so.0 -l:libopencore-amrwb.so.0 -lm -o build/audio_amr_decode
amr-arm: arm
	$(CLANG) --target=armv5te-none-eabi -mfloat-abi=soft -ffreestanding -Os -std=gnu11 -I freestanding/include $(INC) -c src/spmp_amr_opencore.c -o artifacts/spmp_amr_opencore_armv5.o
.PHONY: amr-host amr-arm

STAGE10_INCLUDE ?= ../../../include
stage10-arm:
	$(CLANG) --target=armv5te-none-eabi -mfloat-abi=soft -ffreestanding -Os -std=gnu11 -I freestanding/include -Iinclude -I$(STAGE10_INCLUDE) -c src/spmp_audio_stage10.c -o artifacts/spmp_audio_stage10_armv5.o
stage10-host:
	$(CC) -O2 -Iinclude -I$(STAGE10_INCLUDE) tests/stage10_adapter.c src/spmp_audio_stage10.c -o build/stage10_adapter
.PHONY: stage10-arm stage10-host
