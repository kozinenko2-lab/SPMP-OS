# SPMP-OS — полный комплект для продолжения в Codex

**Состояние:** 14 ноября 2026 г. **Целевая консоль:** Defender MultiMix Magic, Sunplus SPMP8000, ARMv5TE / ARM926EJ-S, eCos. **Цель:** восстановить архитектуру RedBoot и `8000_MMI.RAP`, создать SPMP-OS и/или модернизировать оригинальную оболочку, сохраняя безопасный заводской запуск.

[Этап 15: файловый менеджер и прямой Roboto TTF](research/boot_reconstruction/STAGE15_REPORT_RU.md).
Новый **SPMP_OS_FILES.bin**: две SD-панели FM01, кнопки, сведения о файле,
разделение System/User, журнал. **Просмотр без файловых операций и запуска ROM**;
Atari 2600 — следующий первый эмулятор. Шрифт читается напрямую из TTF,
с кэшем; аппаратный статус пакета VM_TESTED_ONLY.
[Установка SD-пакета](research/boot_reconstruction/examples/mmi_files/README_RU.md).
[Скачать проверенный пакет FILES](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43575053229/artifacts/14105733004) — внутри `build/mmi_files/SPMP-OS-SD.zip`.

[Основное ТЗ](docs/PRODUCT_REQUIREMENTS_RU.md),
[аудит текущего кода](docs/PRODUCT_AUDIT_2026-11-14_RU.md) и
[очередность разработки](docs/PRODUCT_ROADMAP_RU.md) — актуальная основа
проекта после семи требований пользователя. Существующие SPMP-порты ULI,
Neo Geo Pocket/MSX и Doom/Quake учтены как кандидаты; старые темы предстоит
переработать после отдельного описания дизайна. Полной интегрированной ОС пока нет.

[Последний RAM-пакет, этап 14](research/boot_reconstruction/STAGE14_REPORT_RU.md): из двух
пользовательских RAW-сеансов получены 14 состояний кнопок и отдельное событие
POWER. HOLD отложена по просьбе пользователя. Карта подключена к исходникам
SPMP-OS; **SPMP_OS_MINI.bin** — первый минимальный RAM-прототип меню под MMI:
INFORMATION / CONTROLS / EXIT, собственное POWER-меню, журнал **SPMPOS.CSV**.
[Инструкция](research/boot_reconstruction/examples/mmi_shell/README_RU.md).
Новая оболочка **VM_TESTED_ONLY**, аппаратный запуск ещё не подтверждён.
Полная SPMP-OS, эмуляторы и файловый менеджер пока не перенесены в этот пакет.

Основа: [рабочий контрольный BIN пользователя](research/boot_reconstruction/STAGE7_REPORT_RU.md)
и [собственный ARM startup](research/boot_reconstruction/STAGE8_REPORT_RU.md).
Оригиналы прошивки, игра и её ресурсы остаются приватными.

## С чего начать

1. Прочитайте `AGENTS.md` (правила безопасности для Codex) и `docs/STATUS_ROADMAP_RU.md`.
2. Откройте `project/spmp-os/`: это текущая объединённая кодовая база, а **не набор несвязанных патчей**.
3. Присланные оригинальные Ghidra `.gzf` — в `ghidra/user_GZF/`. Для нового импорта подготовлены ARM ELF в `ghidra/Stage52_ELF_Imports/.../elf/`.
4. Фирменный MMI, полный FSP и оригинальный образ RedBoot — в `firmware/originals/`. Извлечённое содержимое FSP — в `firmware/unpacked_FSP/` (1107 файлов). Варианты RedBoot — в `firmware/redboot_research/extracted/` и в ELF-импортах.
5. Исследование архитектуры и Java-скрипты Ghidra — `research/Stage53_to_55/`.
6. Все оригинальные патчи мультимедийной ветки доступны в `upstream_chat_branch/`. Отчёт реального объединения — `integration/chat_merge_report.json`.

## Что реально объединено

Базовая кодовая версия **Stage 49** объединена с **исследовательскими Stage 53–55**. Патчи чата: мультимедиа v1–v6, Theme Engine v7–v9 (34 темы), файловый менеджер **FM01, FM02, FM03, FM04, FM05** (навигация, операции, свойства, экранная клавиатура, текстовый редактор). Для FM01–FM05 выполнены штатные `install.py` и `--verify`. При объединении мультимедиа/тем уже существовали 354 идентичные файла, 327 новых файлов добавлено; конфликтов нет. Все 14 оригинальных ZIP-патчей сохранены.

## Сборка (здесь уже выполнена и проверена)

```sh
make -C project/spmp-os host
(cd project/spmp-os && ./build/spmp-os-host --cores)
make -C project/spmp-os arm-core arm-native-core arm-chip8
for n in 01 02 03 04 05; do (cd project/spmp-os/features/filemanager/FM$n && sh tools/build.sh); done
SPMP_MMI_RAP=firmware/originals/8000_MMI.RAP python3 -m unittest discover -s research/Stage53_to_55/SPMP-OS_Stage55/tests -v
python3 tools/verify_integrity.py --quick
```

- `builds/host_linux_x86_64/spmp-os-host` — собранная программа **для ПК Linux x86_64**; её можно использовать для исследований и проверок.
- `builds/armv5_EABI5/` — ARMv5 библиотеки и модули, не готовая к загрузке прошивка.
- На Defender не проверены: собственный старт SPMP-OS, FAT/LCD/PCM HAL, стабильная загрузка из RAM, Dual Boot.
- В предыдущем тесте MMI отвергла экспериментальный `MMI_DIAG.bin` с сообщением **«неверный формат»**. Не считать этот BIN рабочим, нужна точная диагностика OEM-загрузчика.

## Карта каталогов

| Каталог | Содержимое |
|---|---|
| `project/spmp-os/` | Рабочее объединённое дерево исходников SPMP-OS и FM01–FM05 |
| `builds/` | Скопированные рабочие host и ARMv5-артефакты |
| `firmware/originals/` | Оригинальный `8000_MMI.RAP`, FSP.img, Images_RedBoot.img (не изменять) |
| `firmware/unpacked_FSP/` | Распакованные оригинальные ресурсы, ROMFS, BIN, ROM, аудиобиблиотеки |
| `firmware/redboot_research/` | 5 PRAW-вариантов загрузчика и отчёты |
| `ghidra/user_GZF/` | Три оригинальных Ghidra GZF из `SPMP8k.zip` |
| `ghidra/Stage52_ELF_Imports/` | ELF и сегменты с оригинальными адресами ARM |
| `research/Stage53_to_55/` | Графы вызовов, карта системных потоков, скрипты Ghidra, тесты |
| `research/historical_boot_patches/` | Исходные патчи Stage41–49, не модифицирующие оригиналы |
| `upstream_chat_branch/` | Полные оригинальные 14 патчей, исходники, изображения и документы |
| `integration/` | Результаты слияния и контроль конфликтов |
| `tools/` | Проверка контрольных сумм и рабочей среды |
| `docs/` | Приоритеты разработки и протоколы проверок |

## Самое важное по безопасности

**Никаких Flash/NAND/RedBoot-записей.** Программа для ПК и ARMv5 ET_REL не являются установочной прошивкой. Заводские бинарники являются личными пользовательскими ресурсами — не заливать их в публичные репозитории. Для будущей консоли использовать MMI/eCos как стартовую аппаратную основу, пока не восстановлены проверенная карта RAM, HAL и механизм восстановления.

## Продолжить в Codex

После распаковки откройте каталог `SPMP-OS_Codex` как проект Codex, прочитайте `AGENTS.md` и дайте задачу из `docs/CODEX_PROMPT_RU.md`. Можно перенести только исходники в приватный Git-репозиторий, а образы прошивки хранить отдельно локально.
