# Этап 15: FM в SPMP-OS, System/User, прямой TrueType и Atari path

По уточнению пользователя сначала развивается файловый менеджер, первым
эмулятором будет Atari 2600. Имеющиеся остальные ядра адаптируются и собираются
под ARMv5, без массовой замены их версий. HOLD остаётся отложенной.

Собран **SPMP_OS_FILES.bin** и SD-пакет с отдельными System/User. FM01 model
линкуется в настоящий RAM NGame, используется измеренный профиль кнопок.
**Статус нового пакета — VM_TESTED_ONLY.** Это read-only этап FM: просмотр,
панели, сведения и выбор пути. Файловые операции FM02 и запуск Atari на
устройстве ещё не включены. [Инструкция](examples/mmi_files/README_RU.md),
[структура SD](../../docs/SD_LAYOUT_RU.md).

## Восстановленный directory ABI оригинальной MMI

Точный SHA RAP прежний: `5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.
Уточнения из libspmp8k использованы как указатели для поиска; адреса и код
проверены по оригинальному образу, не через слепой runtime scanner.

| Функция | Адрес | Подтверждённое поведение |
|---|---:|---|
| ecos_opendir | 0x2a69bc | Резервирует fd начиная с 1; возвращает положительный fd либо 0 при ошибке; указывает directory/read flags |
| ecos_readdir_r | 0x2a6b8c | Читает 0x104=260 байт через read; status0/resultNULL — EOF; ошибка возвращается числом errno |
| ecos_closedir | 0x2a6be0 | Передаёт fd в eCos close |
| ecos_stat_path | 0x2a6504 | Через mount resolver вызывает filesystem stat method +0x34; 0/-1 и errno |

Dirent не имеет d_type. Статистика — 32 байта, mode+0, size+16, mtime+24;
директория bit1, regular bit8. Размер/режим подтверждены также FAT stat ARM
фрагментом. Для директории fd0 не используется, для обычных файлов fd0 допустим.

Ghidra 12.2 повторно открыл проверенные копии исходных GZF и экспортировал
четыре новые функции с ARM. Суммарно **295 выбранных экспортов** (MMI264,
RedBoot31). [Псевдокод/ARM](decompiled/mmi/ecos_opendir.c),
[14 original ARM cases](evidence/directory_services_arm.json),
[compatibility profile](evidence/files_profile.json).
OS/mount/FAT в VM подменены; реальная SD этими тестами не затрагивалась.

Новые пять prefix CRC включают read и проверяются до BSS, после прежних
table/memory/input/lock checks; несовпадение возвращает 0x90a. Это проверка
выбранных интерфейсов, не полный SHA образа в RAM.

## Файловый менеджер и ввод

В `project/spmp-os` добавлены `spmp_sd_directory` и `spmp_mmi_files`.
Один SD root `/fat20a0/hda0`, две независимые панели FM01. Сортировка папок
перед файлами и UTF-8 byte order, девять строк. Pager держит девять кандидатов
и lower bound страницы, не выделяет память на весь каталог. Для следующей
страницы повторно обходит каталог; для предыдущей при необходимости начинает
поиск сначала. Cache сбрасывается X. Обход ограничен 4096 записями; ошибки
read/stat/close не превращаются в успешный неполный список. Descriptors закрываются.
Подсчёт синхронный; отмена длительного обхода и hotplug — дальнейшие задачи.

Явный адаптер MMI→FM устраняет конфликт битов: X не становится START,
настоящий START/SELECT переводятся в legacy FM constants. L/R и крестовина
переключают панели; POWER остаётся отдельным системным событием. A открывает
каталог/сведения, B родителя/назад, START Home. Начальные пути User/System,
при их отсутствии есть попытка открыть только корень SD. На NAND fallback нет.

Рендер использует MMI set-color/fill/paint и собственную проверенную память.
Нового framebuffer, heap или RTOS-потоков нет. Старые темы как дизайн не
принимаются; новый интерфейс пока функциональный, без окончательного оформления.

Журнал `User/Logs/SPMPFM.CSV` дописывается общим bounded helper. `#PATH`
сохраняет percent-encoded выбранный UTF-8 путь; `#FONT` сообщает режим шрифта.
Наличие END по-прежнему предшествует окончательному возврату и не заменяет
аппаратное наблюдение cleanup. Установочный ZIP создаёт каталоги на ПК;
программа не вызывает непроверенный mkdir и не переносит пользовательские данные.

## Прямой Roboto TTF

Пользователь предоставил Roboto Regular и уточнил возможность использования
TTF напрямую. Реализован **потоковый целочисленный TrueType renderer**, без
предварительной конвертации шрифта. Исходный TTF 171272 байта, SHA256
`017c0be9aaa6d0359737e1fa762ad304c0e0107927faff5a6c1f415c7f5244ed`.
Name table: Roboto Regular, Version 2.137; 2017, Google, Apache-2.0; fsType0.
Оригинал без изменений, metadata/license/notice включены в System/Fonts.

`spmp_ttf` читает sfnt directory, glyf/loca/hmtx, cmap4/12, простые quadratic
контуры и XY composites с ограничением глубины/числа точек/рёбер. Outline
переводится в monochrome glyph при первом обращении. Нет heap, FPU, выполнения
TrueType hint instructions, CFF/OTF или сложного shaping. Шрифт читается через
2-КиБ cache, 96 растеризованных символов кэшируются; workspace≤32 КиБ.
В текущем UI 13 px, API допускает 6–20 px в пределах fixed glyph cell.

Файл не загружается целиком. В меню/панелях/сведениях/POWER используется один
поставщик шрифта. При ошибке TTF остаётся встроенный резервный bitmap из
прежней базы; его notice также вложен. Выбранный User ROM при просмотре
не читается — файловые payload reads здесь относятся только к системному TTF.

Host tests покрывают латиницу, кириллицу/составную Ё, cache, размеры,
truncation/header failures и 400 мутаций входа. ASan/UBSan — PASS.
Compiled ARM VM действительно читает TTF и выполняет renderer, включая
missing/bad-font fallback. Скорость и читаемость на физическом LCD ещё не измерены.

## Atari 2600 и внешний ROM

Адаптер z26 уже имел `read_entire(host,path,...)`. Подключён file_size preflight,
размер чтения ограничен CartRom, сверяется размер до/после чтения. Обёртка
`spmp_ui_wrap_host` теперь передаёт file_size с правильным user context.
Два синтетических ROM с разными именами/каталогами выполняют разные программы
6507; relaunch и отказ missing/bad-size проходят. Зависимости от имени ROM
или соседства с BIN в этом интерфейсе нет.

Пользовательский Adventure 4096 байт сохранён локально и дал пять host-кадров
с normal stop. В репозиторий попали только [метаданные](evidence/atari_user_adventure_reference.json),
без ROM/скриншотов. Обновлённый adapter собран ARMv5TE/EABI5/soft-float ET_REL.
Это ещё не запускаемый Atari-пакет под MMI; звук, mapper/game compatibility,
память и framebuffer/return предстоит подключить/проверить. Произвольный путь
не означает гарантированную поддержку любого mapper или всех игр Atari.

## Проверки и следующий шаг

- Host FM adapter/paging/ошибки/путь ROM — PASS; ASan/UBSan — PASS.
- TrueType host/cache/ошибочные входы — PASS; ASan/UBSan — PASS.
- File-size forwarding и Atari synthetic/private host — PASS; ARM adapter compiled.
- Compiled FM ARM — **20 случаев PASS**, SD/graphics/time simulated.
- Python — **86 total**, public81 PASS/5 optional SKIP.
- Exact original RAP loader/collector/return — **3 PASS**, отдельно от
  directory wrappers, с виртуальными SD/font/graphics services.
  [Сборка](evidence/files_build.json), [public VM](evidence/files_vm.json),
  [original RAP](evidence/files_rap_vm.json).

После аппаратного запуска этого пакета: включать FAT write backend/FM02
с отдельными проверками копирования/отмены/ошибок и подключать Atari runtime
к выбранному внешнему пути. Остальные существующие ядра сохраняются кандидатами
для адаптации ARMv5 по общей модели; новый пакет не выдаёт их за готовые.

## Опубликованный пакет

[Скачать FILES](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43575053229/artifacts/14105733004).
Исходники `19fad81496b75a3d0a34e6aab430ffe1f41cc7c8`, workflow43575053229 — PASS.
Внутри `build/mmi_files/SPMP-OS-SD.zip`; распаковать его содержимое в корень SD.
Опубликованный BIN — 36736 байт, SHA256
`f352e98ab1f0e2fb6c50f8b96a231ffe78ed55268e92d6e8bd615c9936bb5f8e`.
Скачанный artifact ZIP проверен по GitHub digest, BIN заново упакован из
приложенного ELF побайтно; manifest и BIN вложенного SD ZIP совпали.
Roboto SHA совпал с оригиналом; ROM/OEM отсутствуют.

[20 публичных CI случаев](evidence/files_ci_vm.json) используют synthetic bindings.
**Именно опубликованный реальный ELF** отдельно прошёл 3 случая оригинального
RAP loader/collector/normal return: [результат и provenance](evidence/files_ci_rap_vm.json).
Графика, SD и время в этих проверках виртуальные; статус остаётся VM_TESTED_ONLY.
Локальные evidence/files_package.json и files_build.json относятся к Clang19;
размер и SHA опубликованной CI-сборки указаны отдельно выше.

Исправлена также граница `list_page`: при малой ёмкости caller получает
не больше `cap` записей. Host regression с guard-entry и ASan/UBSan — PASS.
