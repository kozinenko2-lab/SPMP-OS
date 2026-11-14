# Аудит SPMP-OS по основному ТЗ

14 ноября 2026, Europe/Moscow. Проверенная исходная ревизия:
`8f0f6c9f8dfa1c96876e7821fc2ecf433e361005`.
[ТЗ](PRODUCT_REQUIREMENTS_RU.md), [новый план](PRODUCT_ROADMAP_RU.md),
[машиночитаемое свидетельство](evidence/product_audit_2026-11-14.json).

Вывод: база пригодна для дальнейшей разработки, но это **несколько отдельных
прототипов и исследовательских слоёв**, а не связанная новая ОС. Сохранять
стоит переносимые алгоритмы, адаптеры ядер и тесты. Основная работа — общие
службы, согласование API и проверенные backends MMI. Массовое удаление или
переписывание всех ядер сейчас потеряло бы полезные результаты.

Аудит охватывает архитектуру, сборочные связи, ключевые API/реализации и
запуски выбранных стендов. Это не построчная проверка всех third_party и не
проверка всех ROM, кодеков или функций на консоли. Поведение системы в рамках
этого этапа не изменялось; новых device-пакетов не выпускалось.

## Соответствие семи направлениям

| Направление | Что есть в коде | Что требуется для продукта |
|---|---|---|
| Эмуляторы | Реестр, адаптеры, ROM resolution, меню игры, save-state/настройки, ограниченный ELF loader/runtime | Пакеты с обнаружением и версиями, стабильный API, вызов настоящего ARM-модуля, device video/audio/VFS, профили памяти/скорости |
| Порты/native | Исследованный OEM NGame/startup; ссылки на исходники homebrew; первый собственный RAM MINI | Менеджер пакетов/данных и launcher; перенос портов, совместимость старых BIN с корректным возвратом оболочки |
| Мультимедиа/книги | MP3/libmad, WAV/FLAC/Tremor, микшер; metadata probes, BMP; host JPEG/PNG/GIF | Настоящий PCM, ARM image backend, приложения с очередями/управлением; video decoder+sync/seek; новая читалка TXT/PDF/FB2/RTF |
| Файловый менеджер | FM01–FM05: две панели, операции, свойства, клавиатура, редактор | Общий VFS/FAT backend, два тома, dispatcher запуска и интеграция в рабочую оболочку |
| Темы/шрифты/звуки | Несколько поколений palette/layout/font/SFX, 34 темы, font callbacks | Новый согласованный с пользователем theme/UI слой, системные настройки/шрифты/звуки для всех приложений |
| Периферия | Отдельные OEM research интерфейсы; upstream libgame имеет controller ID | Проверка RCA PAL/NTSC, PCM capture, камеры, протокола аксессуаров; общий player API |
| Настройки | Per-core CRC config, отдельный theme config, локальные RU/EN строки, gpSP virtual time | Общий конфиг, локализация всех экранов, реальный RTC, тома, physical TV, настройки внешнего вида и ограниченный reset |

После этой исходной ревизии выполнен [этап 15](../research/boot_reconstruction/STAGE15_REPORT_RU.md):
A02 исправлен, добавлен явный MMI→FM adapter, read-only FM интегрирован в
RAM-пакет, реализован прямой TrueType. Ниже сохранены находки именно исходной
ревизии аудита; они не означают, что последующие исправления отсутствуют.

## 1. Эмуляторы: что означает текущий список

В [registry.c](../project/spmp-os/src/cores/registry.c) **20 записей**:

- 11 помечены `HOST_TESTED`: diagnostic, CHIP-8, FCEUmm NES, ограниченный
  NES-baseline, Gambatte GB/GBC, gpSP GBA, Snes9x, PicoDrive MD, z26 Atari2600,
  TGEmu PCE и SMS Plus GX для SMS/GG. Diagnostic и baseline не являются двумя
  дополнительными поддержанными консолями.
- 6 `SOURCE_AVAILABLE`: Doom, Spectrum, C64, CPC, ScummVM, xRick.
- 3 `SOURCE_UNVERIFIED`: Quake, MSX, PS1 research.

Это состояния импортированной базы. В этом аудите заново выполнен вывод
реестра, а не полная проверка всех ядер и ROM. `HOST_TESTED` не означает
установленный ARM-пакет или проверку на Defender. Некоторые ядра/части имеют
ARM ET_REL сборки, но полная линковка, системные зависимости и аппаратный
backend проверяются отдельно.

Реестр фиксирован в C. Gambatte/SMS/multi-libretro используют Linux `dlopen`
в host-варианте; это не загрузчик MMI. В коде исходников объявлены, например,
Snes9x `1.63` и PicoDrive `2.04` + revision; это не доказательство последних
upstream-версий на дату аудита. Общего lockfile/каталога выпускаемых device
пакетов пока нет. `SOURCES_AUDIT.csv` полезен для происхождения некоторых
архивов, но не заменяет версии всех будущих модулей.

Существующий [ELF overlay](../project/spmp-os/src/loader/spmp_overlay.c)
стоит сохранить: проверяет структуру, метаданные, границы и ограниченный набор
ARM relocations, использует cache callbacks. Но он отвергает undefined/absolute
symbols и Thumb, использует точное равенство `core_api_version==5` и рассчитан
на заранее подготовленные модули. В `src/modules/` настоящий
`spmp_module_entry_v1/.spmp.meta` имеет probe, а не готовый комплект эмуляторов.
[spmp_runtime](../project/spmp-os/src/loader/spmp_runtime.c) координирует
состояния/память, но не вызывает ARM entry. Это основа loader, не законченная
система сменных приложений.

## 2. Уточнение пользователя: ULI и существующие SPMP-порты

Через GitHub сверены открытые источники, без импорта ассетов или исполнения
скачанного кода. [Метаданные и git object SHA](evidence/uli_spmp_sources_2026-11-14.json).

| Источник/система | Что подтверждено | Дальнейшая работа |
|---|---|---|
| [uli/tgemu-spmp8000](https://github.com/uli/tgemu-spmp8000) | Makefile опирается на libspmp8k; game.c содержит video/audio/input init и cleanup; TGEmu уже есть в нашей базе | Сравнить нашу portable-ветку с upstream, выделить backend и паковать PCE-модуль |
| [uli/libspmp8k](https://github.com/uli/libspmp8k) | Есть libgame, Doom110, z26, примеры audio/input; header описывает per-controller mapping | Использовать как справочник и донор переносимых частей после сверки адресов/ABI точного RAP |
| [Doom110](https://github.com/uli/libspmp8k/tree/master/demos/doom110) | Исходники платформенного порта доступны; Makefile имеет `-DNO_SOUND` | Обновить выбранный engine, реализовать PCM и пакет данных |
| Atari2600, PCE, SMS/GG | Адаптеры соответствующих ядер уже есть в SPMP-OS | Довести до отдельно устанавливаемых ARM-модулей и измерить на консоли |
| [Spectrum](https://github.com/uli/xpectrum-spmp8000), [C64](https://github.com/uli/Frodo4-spmp8000), [CPC](https://github.com/uli/wiituka-spmp8000) | Публичные репозитории ULI найдены, исходные ссылки совпадают с направлениями старой базы | Проверить платформенные части, клавиатуру, звук и версии ядер |
| Quake 1, MSX | Пользователь подтвердил существование портов; в прежнем реестре отмечены binary-only источники | Найти/сопоставить исходную версию либо отдельно исследовать совместимость BIN; не считать порт отсутствующим |
| Neo Geo Pocket | Пользователь указал существующий порт; записи в нашем реестре нет | Добавить в список кандидатов, найти конкретный SPMP-порт/ядро и оценить перенос |

Наличие портов — полезное подтверждение практической базы SPMP8000; обновление
ядра и его интеграция остаются отдельными задачами. Поиски репозиториев только
по строкам `spmp8000 quake/neogeo/msx` не дали результатов; это **не доказательство
отсутствия** исходников. Каталог не ограничивается перечисленными системами.

В `libgame.h` документированы `emu_keymap.controller` (обычно 0/1),
`emuIfKeyInit`, `emuIfKeyGetInput` и cleanup. Это кандидат пути к комплектным
контроллерам, ещё не проверенный на данной Defender. `demos/usb/usb.c`
перенаправляет отладочный вывод и меняет hooks; это не драйвер HID/USB host.
Старые сканеры адресов и системные hooks не копируются вслепую в новый backend.

## 3. Конкретные стыковочные проблемы

| ID / приоритет | Наблюдение в исходниках | Решение |
|---|---|---|
| A01 / P0 | MMI frame, DEFOS/FM, libretro и UI power имеют разные bit layouts | Один нормализованный контракт и явные адаптеры; raw state/event, held/edge и system event не смешивать |
| A02 / P0 | [spmp_ui_wrap_host](../project/spmp-os/src/ui/spmp_ui_router.c#L49) не передаёт `file_size` | Добавить wrapper с исходным user context, проверить передачу всех новых callbacks; иначе теряется memory preflight новых adapters |
| A03 / P0 | HOST_SRC и MINI не линкуют FM/multimedia/theme features в общий продукт | Зафиксировать сборочную цель связанной оболочки и интегрировать службы по одному |
| A04 / P0 | Нет завершённого device module-entry/import/cache/return пути | Развить существующий loader; сначала собственный маленький модуль, затем настоящее ядро |
| A05 / P1 | `spmp_core_host.buttons` не имеет player index; [libretro port>0](../project/spmp-os/src/cores/libretro_multi_adapter.c#L199) отвергается | Расширить versioned API и adapters для нескольких игроков до заявления о поддержке двух джойстиков |
| A06 / P1 | Разные модели файлового просмотра и разные IO contracts | Продуктовый FM строить на FM01–FM05 и общем VFS; Stage34/40 оставить стендами |
| A07 / P1 | FM02 POSIX привязан к одному root/device и использует renameat2/fsync | Новый per-volume FAT backend; межтомовый move через copy/verify/remove, не обещать POSIX atomicity на FAT |
| A08 / P1 | `defos_enter` сообщает статус, FM01 leaf возвращает 1; общий launcher отсутствует | Единый content dispatcher и association policy, включая неоднозначные BIN/ROM |
| A09 / P1 | Только per-core/theme settings, часть опций помечена deferred | Разделить global/core/game config и показывать фактическую применимость настройки |
| A10 / P1 | Video probes/stock bridge не являются готовыми декодерами | Матрица container+codec+limits, доказанный ABI и нормальный playback lifecycle |
| A11 / P2 | FM05 с GCC/Werror останавливается на misleading-indentation | Исправить переносимость при дальнейшей работе; это не выявленная ошибка алгоритма редактирования |
| A12 / P2 | Старые статусы/номера Stage и дубли theme/UI выглядят как общий progress | Новые ТЗ/аудит/план считать текущими; старые отчёты сохранять как историю с датой |

Пример A01: `DEFOS_KEY_START` и `FM01_KEY_START` — `1<<6`; у нового MMI
слоя этот логический бит соответствует X, а START переводится в другой бит.
Прямое подключение этих uint32_t поменяет смысл кнопок. Старый UI POWER=256
также нельзя трактовать как held-кнопку libretro. Это вопрос адаптеров между
интерфейсами, не необходимость повторного физического теста HOLD.

## 4. Мультимедиа: сохранять код, разделять уровни готовности

- MP3/libmad: реальный fixed-point decoder, ID3, bounded IO, approximate seek,
  pause, volume, M3U reader и app bridge. `make check` выполнен заново — PASS.
  Смена трека/единый playlist UI и воспроизведение через физический DAC ещё
  требуют интеграции; хороший host PCM не является звуком на Defender.
- Audio v5: WAV/IMA, FLAC-lite и Tremor/OGG с адаптерами. В этом аудите
  анализировались код/контракты; полная v5 regression заново не запускалась.
- Audio v6: fixed-point mixer/resample/queues и adapters. `check` и
  `stage10-check` — PASS с подставным sink. ASF/WMA — metadata parser,
  **не WMA decoder**. External OpenCORE/FFmpeg tests не являются ARM-декодерами.
- BMP имеет portable renderer. JPEG/PNG/GIF в `host_image.c` зависят от host
  libjpeg/libpng/giflib; GIF рисует первый кадр. Rotate/zoom/slideshow/thumbnail
  приложения на устройстве ещё не реализованы.
- `inspect.c` разбирает выбранные поля MP4/AVI и форматов изображений/аудио.
  `spmp_stock_bridge` проверяет переданные callbacks/capabilities/verified flag;
  он сам не восстанавливает decoder ABI и не предоставляет физический codec.
- TXT/PDF/FB2/RTF reader отсутствует в рассмотренных product sources. FM05
  ограничен UTF-8 и 32768 байтами текста; использовать его как читалку всего
  документа нельзя. Можно сохранить редактирование и font callback.

## 5. Темы, настройки и периферия

Темы существуют в старом основном INI/palette слое и отдельных v7/v8/v9
features. HOST_SRC не делает v9 общим UI. v9 содержит список/сетку, компактный
bitmap font, RGB565 background, config и синтез коротких SFX. Встречаются
зашитые RU/EN строки. По новому указанию пользователя внешний слой требуется
переработать полностью; менять сейчас только фоны/палитры было бы неверной
интерпретацией ТЗ. Выбрать новый дизайн без будущих пояснений пользователя
тоже нельзя. Font/UTF-8 validation, bounded background IO и тесты полезны.

`spmp_settings` сохраняет настройки ядра в CRC-конфиг, не всеобщие настройки
ОС. PAL/NTSC там — эмулируемый регион. `spmp_gpsp_time.c` допускает фиктивную
дату 2000-01-01 и clock callback: это не готовый сервис установки RTC.
Общих языка/даты/шрифта/томов/reset и физического TV settings backend пока нет.

Зарегистрированные SD id0, MSDC id1 и NAND id2 подтверждены предыдущим ARM
исследованием. Пользователь подтвердил SD-журналы, но одновременный доступ
SD+internal не проверен. Одних mount paths недостаточно для утверждения, что
OEM backend не использует единственный активный storage context.

Нативного законченного RCA/mic/camera/USB backend в reviewed product code
нет. Даже экспорт с названием `native_camera_mode` в выбранном Ghidra
псевдокоде переключает double buffer/очищает framebuffer и сам по себе не
доказывает захват кадров с сенсора. Поэтому периферия — отдельная трассировка
RAP/платы, а не включение пункта меню с похожим названием.

## 6. Что сохранить, переработать и вывести из продуктового пути

| Решение | Компоненты |
|---|---|
| Сохранить и развивать | Проверенный NGame/startup; exact-image bindings; измеренный профиль ввода; переносимые core adapters/ROM resolution; ELF bounds/relocation tests; memory/runtime модели; MP3/FLAC/OGG/mixer; FM01–FM05 алгоритмы и тесты |
| Переработать/объединить | Input/player ABI, callback forwarding, VFS и два тома, package/launcher API, resource lifecycle, общий config/i18n/fonts, codec/file associations, настоящий FAT/backend |
| Заменить в продукте | Нынешние theme/layout поколения, статический каталог установленных приложений, отдельные домашние экраны/FS models после миграции на общий слой |
| Оставить только для ПК/истории | POSIX IO/dlopen, ограниченный NES-baseline как тест, Stage34/40/boot research и synthetic layouts, старые неподтверждённые package drafts |

«Вывести из продуктового пути» не означает удалить архивы и доказательства.
Удаление дублей разумно после замены и воспроизводимых проверок. Закрытые
OEM библиотеки, ресурсы игр и абсолютные аппаратные hooks не становятся общими
службами только потому, что они встречаются в старом порте.

## 7. Повторённые проверки и ограничения

| Проверка этого аудита | Результат |
|---|---|
| `make host`, `--cores` | PASS; host target уже актуален, вывод 20 записей. Это не новый прогон всех эмуляторов |
| Settings/MMI input + menu/router/layout/CHIP-8 state harness | PASS, исполнялись соответствующие host harness |
| ELF overlay/memory/runtime | PASS: relocated probe objects, guards/cache fixtures, pause/switch/save-failure; гостевой ARM entry на ПК не выполняется |
| FM01/FM02/FM03/FM04 build scripts | PASS, host tests и ARMv5 ET_REL; FM04 также ASan/UBSan |
| FM05, исходный GCC/Werror запуск | FAIL на misleading-indentation; замечание переносимости A11 |
| FM05, запуск с Clang | Core и POSIX tests PASS; script остановлен отсутствующими Clang ASan runtime в этом окружении. Полный script не объявляется PASS |
| MP3 `make check` | PASS, FFmpeg reference/ошибочные входы на ПК |
| Audio v6 `check stage10-check` | PASS, mixer/ASF и fake audio sink |
| Image host `make test` | BLOCKED: отсутствуют development packages/pkg-config libpng/libjpeg/giflib; это ограничение окружения, не установленный дефект decoder |

Точные exit codes, команды, хвосты журналов и SHA рассмотренных файлов
сохранены в JSON. Исходники компонентов не менялись для обхода проверок.
Ранее полный CI `d36fa32` прошёл с установленным Clang runtime и FM01–FM05;
новое локальное окружение не отменяет исторический результат, но не позволяет
заявить о повторном полном PASS FM05 здесь.

Положительные аппаратные свидетельства остаются прежними: собственные три
полосы/выход и SD input journals пользователя. MINI остаётся VM_TESTED_ONLY:
сообщения о его запуске на консоли ещё не было. Ни FPS новых ядер, ни видео,
звук, камера, TV, RTC или два одновременных тома этим аудитом не подтверждены.

Следующий практический шаг — очередь B из дорожной карты: единые службы и
связанный read-only SD browser, затем первый сменный модуль. Это позволяет
использовать имеющиеся порты ULI и библиотеки вместо новой реализации каждого
эмулятора с нуля, сохраняя проверяемые границы между ядром и платформой.
