# SPMP-OS Audio v5 — FLAC / WAV / OGG Vorbis / AMR / WMA

**Ветка:** `features/multimedia/audio_v5` — независимый аддитивный патч для SPMP-OS Stage10.
**Дата:** 2026-11-11.  **Цель:** ARMv5TE, soft-float, RGB565/PCM16 аудио окружение Defender MultiMix Magic.

## Реальный статус

| Формат | Поддержка | Проверка |
|---|---|---|
| WAV PCM | 8, 16, 24, 32 бит, моно/стерео, потоковый PCM16, seek | Подтверждён exact PCM на ПК |
| WAV IMA ADPCM | 4-битный Microsoft IMA, моно/стерео, чтение блоков, seek | ПК-выход сравнен с FFmpeg, небольшие численные отличия |
| FLAC | Fixed-point FLAC-lite v3 (subset стандартного FLAC), моно/стерео, потоковый PCM16 | Равен эталону на проверенном тесте, seek пока не сделан |
| OGG Vorbis | Fixed-point Tremor/libogg, mono/stereo, streaming, seek при seekable IO | Полное декодирование ПК-потока; численная близость FFmpeg |
| AMR-NB / AMR-WB | Подключаемый адаптер OpenCORE + 20-мс кадры | На ПК проверено с установленными OpenCORE `.so`; **ARMv5-кодек не поставляется** |
| WMA/ASF | Проверка ASF GUID, WMA codec id/rate/channels/bitrate/duration + внешний decoder ABI | **PCM-декодирования WMA НЕТ** |

Сжатые данные читаются по callbacks `read/seek/size` из SD/FAT, а PCM выдаётся через `sp_audio_decode`/`sp_audio_pump`. WAV и FLAC не требуют FPU; OGG Tromor работает целочисленно. FLAC использует workspace >= 192 КиБ, OGG — дополнительный heap для Tremor. Оценка фактического RAM на реальной консоли ещё отсутствует.

## Быстрый запуск тестов на ПК

```sh
cd features/multimedia/audio_v5
make host
cc -O2 -Iinclude tests/audio_probe.c artifacts/libspmp_audio_v5_host.a -lm -o build/audio_probe
cc -O2 -Iinclude tests/audio_backend_test.c artifacts/libspmp_audio_v5_host.a -lm -o build/audio_backend_test
python3 tests/regression.py
```

Опционально, при наличии OpenCORE в системе: `make amr-host`. Для тестового Stage10: `make stage10-host STAGE10_INCLUDE=/path/to/SPMP-OS/include`.

## Сборка для ARMv5

```sh
make arm
make amr-arm
make stage10-arm STAGE10_INCLUDE=/path/to/SPMP-OS/include
```

Артефакты:
- `artifacts/libspmp_audio_v5_armv5.a` — OGG/WAV/FLAC + интерфейс PCM.
- `artifacts/spmp_amr_opencore_armv5.o` — **только обёртка**, требуется OpenCORE ARMv5.
- `artifacts/spmp_audio_stage10_armv5.o` — glue к публичному HAL Stage10.

**Сборка `*.a` не подтверждает выполнение**: полноценный образ требует allocator, `malloc/calloc/realloc/free`, базовый C runtime (`mem*`, `qsort`, `errno`, stdio helpers в vorbisfile), ARM EABI helpers, контроллер SD, IRQ/DMA и DAC. Это всё ещё необходимо проверить на устройстве и не подменяется компиляцией на ПК.

## API

`sp_audio_player_size()` даёт размер структуры, которую выделяет вызывающая ОС. `sp_audio_open(...)` принимает `sp_audio_io`, формат или `SP_FMT_AUTO`, и workspace. `sp_audio_get_info` даёт частоту/каналы. `sp_audio_decode` выдаёт интерлив PCM16 (количество кадров, не байтов); `sp_audio_seek_ms` позиционирует WAV/OGG; `sp_audio_close` освобождает ресурс декодера. `sp_audio_open_with_backend` поддерживает WMA/AMR адаптеры, не подменяя отсутствие декодера утверждением о поддержке WMA. `sp_audio_pump` передаёт PCM пользовательскому sink.

### Подключение OpenCORE

```c
sp_amr_opencore_ctx ctx;
sp_audio_external_backend backend;
sp_amr_opencore_make(&backend,&ctx,SP_FMT_AMR_NB);
sp_audio_open_with_backend(player,io,SP_FMT_AMR_NB,workspace,workspace_size,&backend);
```

В AMR используются `Decoder_Interface_*` и `D_IF_*`, которые **должны быть реализованы отдельно поставленной библиотекой opencore-amr для выбранной архитектуры**. Архив v5 не содержит сторонние AMR-бинарники и не утверждает, что они уже доступны в оригинальной прошивке через тот же ABI. WMA поддерживается аналогичным `sp_audio_external_backend`, но реализацию декодера предстоит сделать.

### Слияние с игровой веткой

Путь патча исключительно `features/multimedia/audio_v5`. Инсталлятор проверяет контрольные суммы, не перезаписывает чужие файлы, не трогает загрузчик, ядра эмуляторов, ROM, RedBoot/NAND, а откат возможен, пока файлы патча не отредактированы. Тестируется наложение поверх Stage10 вместе с `codec_lab` v3 и `mp3_player` v4. Встроенного перенаправления всех старых приложений `MUSICPLAYMAIN.XML` на новый код пока нет.

## Лицензии

Tremor — BSD-подобная Xiph.org, см. `vendor/Tremor/COPYING`; libogg — BSD-подобная Xiph.org, см. `vendor/ogg/COPYING`. OpenCORE библиотеки не распространяются в архиве, необходим отдельный аудит лицензирования/патентов по конкретным регионам. Исходные `LIBWMA.O`, `LIBAMRNB.O`, `LIBAMRWB.O` из фабричной прошивки не включены в пакет.
