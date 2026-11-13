# SPMP-OS Multimedia & UI / API v1

Цель: автономная мультимедийная ветка без изменения файлов Stage10 `src/cores`, `src/loader`, `third_party`, RedBoot/Flash/NAND.

## Что действительно работает

- `mm_inspect`: распознавание WAV PCM, MP3 (ID3v2 и MPEG заголовок), MP4 (ftyp, stsd и FourCC), AVI, JPEG/PNG/GIF/BMP. Часть контейнерных сведений может быть неизвестна. `avc1` и `mp4a` **не означают, что декодирование реализовано**.
- `mm_play_wav`: потоковое преобразование WAV PCM mono/stereo 8/16-bit, 8–48 kHz в stereo S16 с callback для будущего DAC. Используется фиксированный буфер 256 кадров; нет полного чтения файла в RAM. HOST CLI записывает PCM в файл для автоматической проверки, не воспроизводит звук на устройстве.
- `mm_render_bmp`: BMP 24/32-bit BI_RGB без сжатия, первая плоскость, масштабирование с letterbox в RGB565. Используется не более одной строки (4096 байт), ширина исходника до 1024, общая площадь до 4 млн пикселей.
- `mm_skin_parse`: темы RGB565 с 10 цветами; длина INI до 4 KiB; неизвестные ключи отвергаются; при ошибке прежняя тема остаётся. Рендер домашнего меню — чистый C с RGB565, 320×240.
- HOST ONLY: декодирование JPEG/PNG/GIF через системные libjpeg/libpng/giflib. Для GIF — только первый кадр (в том числе чересстрочные изображения), без анимированных эффектов и alpha-композитинга. У PNG interlaced-файлы пока отвергаются; прозрачность при этом прототипе не композитится. На ARM нет линковки с этими Linux-библиотеками.
- `build/spmp-mm-armv5.o`: реальный ARM EABI5 ET_REL объект из переносимого C, без внешних символов. Его **нельзя считать работающим устройственным модулем**: нет `spmp_module_info_v1`, точек входа/импортов файловой системы и проверенного RAM/HAL; он не является установочным IMG/FSP.

## Границы интеграции

Точка интеграции — `features/multimedia/include/spmp_mm.h`. Опциональный `spmp_mm_stage10.h` / `stage10_bridge.c` адаптирует аудиовывод к публичному `defos_hal` Stage10 без изменения самого HAL; `make stage10-bridge` создаёт отдельный ARMv5 объект. Вызовы `mm_reader.read_at` и `mm_audio_output.submit_s16` передаются через адаптер, который будет реализован после того, как станут известны проверенные драйверы SD и DAC. Никакой глобальной переменной эмуляторов и прямого доступа к GPIO/MMIO.

Не использовать интерфейс `spmp_overlay.h` для произвольной загрузки мультимедиа поверх игрового ядра до разработки диспетчера модулей: Stage10 знает только один overlay slot, и загрузка поверх игрового ядра может его вытеснить. Файловый менеджер и Home должны вызывать `spmp_mm_api_v1` через независимый диспетчер после согласования общей архитектуры.

## Следующий этап

1. Подтвердить драйвер SD/FAT, DMA/DAC, доступность штатного `LIBMP3.O` и ABI. Только после этого — действительное MP3 воспроизведение (PCM вывод, pause/seek, ID3, playlist).
2. Перенести фото-декодеры в ARMv5 с лимитами памяти, определить реальную свободную RAM. Для JPEG приоритет — штатный `JPGDEC.IMG`, если устройство поддерживает его API; для PNG — небольшой потоковый decoder.
3. MP4/AVI: разобрать установленную модель видеодекодера, контейнеры/таймстампы, аппаратные `H264_CODEC.IMG`/`MPG4ASPDEC.IMG`, AV sync; пока только метаданные.
4. Шрифты Unicode/кириллица, фоновые картинки и прозрачные PNG иконки при проверенном бюджете RAM.
5. Снять показания тестов на самой Defender MultiMix Magic. Не изменять RedBoot и область восстановления.

## Локальная сборка на Linux

```sh
make -C features/multimedia test
make -C features/multimedia arm-core
features/multimedia/build/mm-host --home --theme features/multimedia/themes/aurora.ini --output home.ppm
features/multimedia/build/mm-host --inspect song.mp3
features/multimedia/build/mm-host --photo image.png --output photo.ppm
features/multimedia/build/mm-host --wav song.wav --pcm audio_s16_stereo.raw
```

Host-зависимости: C11 compiler, Python3 + Pillow (только тесты), pkg-config, libpng, libjpeg, giflib. ARM: clang и ld.lld. Устройство Linux/SDL2 не требуется.
