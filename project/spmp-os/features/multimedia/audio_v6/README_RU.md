# SPMP-OS Audio v6 — безопасный микшер ARMv5, интерфейсы WMA и AMR

Пакет **не переписывает** Stage10, игровые ядра, RedBoot или NAND. Это независимый патч `features/multimedia/audio_v6`. Устанавливается рядом с MP3 v4 и Audio v5.

## Реальное состояние

- **Работает**: fixed-point стереомикшер 4 голосов, Q16 линейная передискретизация входа 8–192 кГц на выход 8–192 кГц, поканальная регулировка, панорама, линейное нарастание/затухание, saturating mix, мастер-громкость, статистика, потоковые буферы PCM без динамических выделений, вызов HAL Stage10. Пороговые тесты ПК; ARMv5 ELF объекты **скомпилированы, не запущены на устройстве**.
- **MP3 v4**: `spmix_mp3_push` принимает callback `spmp_mp3_player` и отправляет PCM в очередь без потери, если заранее выделена достаточная ёмкость. Исполнение MP3 и наполнение очереди — только основной поток/задача, не IRQ.
- **WAV/FLAC/OGG/AMR/WMA v5**: `spmix_audio_v5_pull` соединяет действующий `sp_audio_decode` с микшером. Для AMR требуется настоящий OpenCORE decoder backend, который v5 ранее проверял на ПК и **ещё не собран для ARMv5**.
- **WMA/ASF**: новый безопасный потоковый анализатор всего ASF-заголовка, WMA v1/v2, rate/channels/bitrate, duration с preroll, packet size, ограничение размеров. **Декодера WMA в Audio v6 НЕТ.** На ПК можно проверить его PCM через FFmpeg в тестах; это не ARMv5-декодирование.
- **Штатные WMA/AMR**: включён скрипт чтения ELF-символов и проверка возможности повторной компоновки пользовательских `LIBWMA.O`, `LIBAMRNB.O`, `LIBAMRWB.O`, не включённых в пакет. Подключение к новым приложениям требует восстановления ABI, а работа decoder_run и звук на плате не подтверждены.

## Требования и запуск

```sh
cd features/multimedia/audio_v6
make host check
make arm
make v5-arm V5_DIR=../audio_v5
make stage10-arm STAGE10=../../..
```

`libspmp_audio_v6_armv5.a` не требует FPU, libc allocator или тредов. Требуются обработчики `memset` и `memcpy` от системного runtime и стандартные soft-integer AEABI helpers при полной линковке (их ABI зависит от итоговой сборки). `libspmp_audio_v6_armv5.a` не содержит MP3/FLAC/WMA/AMR-декодеров: они остаются отдельными модулями. Дополнительные ARMv5-объекты соединения Stage10/v5 создаются отдельными целями Makefile. Включайте только один поставщик C-runtime и `__aeabi_*` для итогового ELF.

## Рекомендуемый поток данных

```text
SD/FAT -> декодер (MP3 v4, FLAC/WAV/OGG v5, внешний AMR/WMA)
       -> ring 16-bit PCM или spmix_audio_v5_pull
       -> SPMP Audio Mixer v6 (S16 stereo + resample)
       -> Stage10 HAL audio_write_s16 -> DAC DMA
```

В реальной системе нельзя декодировать из IRQ, нужно делать предзаполнение PCM очереди в foreground. Запрос от audio_write_s16 может блокироваться или переполнить DMA очередь — HAL должен явно возвращать ошибку. Сейчас драйвер DAC/DMA Defender не подтверждён на физическом устройстве.

## Лицензии

Новое ядро v6: MIT. Зависимости MP3 v4/libmad GPL-2.0-or-later; OGG Tremor BSD-like; OpenCORE AMR Apache-2.0 с возможными патентными ограничениями. При общем бинарнике соблюдайте лицензии интегрируемых модулей; штатные закрытые ARM объекты FSP не распространяются в данном архиве.

## Примеры интеграции и тесты

В `tests/test_integration_mp3_v4.c` показан полный путь MP3 v4 callback -> stereo ring -> mixer. Первая PCM-порция определяет частоту и каналы MP3. `tests/test_integration_v5.c` демонстрирует буферизованное подключение WAV/FLAC/OGG и OpenCORE AMR. Для FLAC внешний PCM-кэш должен вместить хотя бы один блок кодека (типично 4608 кадров, тест использует 8192); иначе v5 декодер вернёт ошибку нехватки памяти.

```sh
make host arm check
make benchmark
make check-sanitizers
make v5-arm V5_DIR=../audio_v5
make stage10-arm STAGE10=../../..
make stage10-check STAGE10=../../..
python3 tests/regression_v6.py --v5 ../audio_v5 --mp3-v4 ../mp3_player
```

Проверочный запуск `regression_v6.py` необязателен при отсутствии Audio v5/MP3 v4 — тогда пропущенные тесты отмечаются явно; для AMR на компьютере нужны OpenCORE `.so`. `Makefile` для ARM использует Clang 17 и llvm-ar, аппаратный SYSROOT от Defender ещё не готов.

## Следующий аппаратный этап

1. Подтвердить карту памяти, DAC/I2S/DMA и работу SD/FAT; измерить PCM underruns и длительные циклы на Defender из RAM (RedBoot/Flash не изменять).
2. Собрать сами OpenCORE AMR-NB/WB decoder cores в ARMv5 freestanding (или восстановить оригинальный ABI штатных модулей), затем тесты speech-векторов на плате.
3. Реализовать нативный WMA v1/v2 fixed-point decoder с ASF packet demux и каналом PCM, а не только контейнерный parser; проверить лицензию выбранного исходного кода.
4. Ограничить RAM пиковые значения FLAC/Tremor и пересчитать ресурсы при одновременном запуске эмулятора и музыки.
