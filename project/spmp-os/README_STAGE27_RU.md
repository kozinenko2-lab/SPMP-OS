# SPMP-OS Stage 27 — gpSP ARMv5 interpreter + bounded resident memory

## Что проверено

- Собраны **15/15 основных выбранных файлов** gpSP под `armv5te-none-eabi`, `soft-float`, без `HAVE_DYNAREC` (13 C + `video.cc` + `cpu.cc`). Это настоящий код интерпретатора ARM7TDMI; стадия 25 собирала под ARMv5 лишь вспомогательные компоненты.
- Встроен поставляемый с gpSP открытый BIOS `bios/open_gba_bios.bin` (16 КиБ) через исходный `bios_data.S`.
- Скомпилированы и присоединены `src/runtime/spmp_heap.c` и новый адаптер `src/modules/gpsp/gpsp_heap_bridge.c`. Перенаправлены через флаги компиляции все обращения C/C++ к `malloc`, `calloc`, `realloc`, `free` в 15 выбранных файлах. Память ядра **обязательно** выделяется и проверяется загрузчиком до вызова `gpsp_heap_bind`.
- Адаптер отказывает в выделениях до bind и после unbind, не разрешает unbind пока имеются занятые блоки, поддерживает полный цикл повторного запуска. Выполнены 100 циклов с ASan/UBSan.
- `compat_strl.c` встроен отдельно. В результирующем объекте `gpsp_interpreter_core.o` остаётся **42 внешние зависимости** (полный список — `build/stage27/gpsp_armv5/audit.json`).
- Тест предыдущей стадии с выполнением гостевого ARM7-кода, изменением красного пикселя RGB565, звуком, save/load и повторным запуском проходит **на ПК**.
- Проверки Stage 26 bounded heap: 16 000 случайных операций — пройдены.

## Ограничения (важно)

Это **перемещаемый ARM ELF32/EABI5**, не готовый загружаемый `SPMP-OS`-модуль и не образ FSP. Для окончательного связывания требуется реализация и проверка `__aeabi_*` для ARM soft-float, файлового `filestream_*` через настоящий SD/FAT HAL, RTC (`time`, `localtime`) и базовых libc-функций. Число 42 — количество внешних имён после частичного линкования, не число ошибок исходников. Текущий ROM-тест GBA проводился на компьютере, ARM-объект ещё не выполнялся на Defender.

Составленный объект **не включает все платформенные исходники libretro-common**, а только обязательное для базовой компиляции ядро, плюс `compat_strl.c`. Имеющиеся 42 зависимости документированы, а не заменены фиктивными реализациями. Перед подключением памяти к реальному загрузчику требуется проверка границ RAM и невозможности пересечения области RedBoot. `gpsp_heap_bind` нельзя вызывать повторно во время живой игры. JIT для ARMv5 не включён.

## Повторная сборка

```sh
make stage27-check
make stage26-check
python3 -m unittest discover -s tests -p 'test_stage25_gba_cpu.py' -v
./build/stage24_gba_harness tests/roms/stage24_gba_test.gba
```

Инструменты: Clang/Clang++, `ld.lld`, `nm`, `readelf`, host C compiler. Последняя команда требует предварительно собранный компьютерный GBA-стенд (`make build/stage24_gba_harness stage24-gpsp`).

Изменения Stage 27: `tools/build_gpsp_armv5.py`, `tools/arm_gpsp_shim/`, `src/modules/gpsp/`, `tests/gpsp_heap_bridge_harness.c`, `tests/test_stage27_gpsp_armv5.py`, `include/spmp_heap.h`, `src/runtime/spmp_heap.c`, `Makefile`.

Сохранились исходные ядра NES/GB/SMS/Mega Drive/Sega CD/32X/SNES/GBA и экспериментальный PSX4ALL. Заводская FSP и RedBoot НЕ изменялись.
