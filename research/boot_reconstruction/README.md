# Восстановление загрузочного тракта

Статический анализ оригинальных образов; не реализация полной прошивки.

**Текущий этап:** [STAGE15_REPORT_RU.md](STAGE15_REPORT_RU.md) — FM01 в RAM
SPMP-OS, явные кнопки, две SD-панели, System/User и прямой потоковый Roboto TTF.
Новый [пакет](examples/mmi_files/README_RU.md) VM_TESTED_ONLY, read-only просмотр;
FM02 operations и Atari execution пока не включены. Directory/stat ABI проверен,
Ghidra selected exports — 295. Atari external-path adapter прошёл host tests и
ARMv5 compile. Предыдущий этап:

**Этап 14:** [STAGE14_REPORT_RU.md](STAGE14_REPORT_RU.md) — два аппаратных
RAW-сеанса, 14 кнопок состояния + POWER, HOLD отложена. Новые MMI input/menu
модули SPMP-OS и **SPMP_OS_MINI.bin**, INFORMATION/CONTROLS/EXIT, собственное
POWER-меню и журнал SPMPOS.CSV. Пакет VM_TESTED_ONLY;
[инструкция](examples/mmi_shell/README_RU.md). Предыдущий
[RAW_R1](STAGE13_REPORT_RU.md) получил данные с консоли, но штатное POWER-меню
по сообщению пользователя не появлялось; блокировка проявилась после выхода.
[Пользовательский журнал SD0](STAGE12_REPORT_RU.md), [SD0 fix](STAGE11_REPORT_RU.md),
[native binding](STAGE9_REPORT_RU.md). OEM exports остаются 291.
Псевдокод: `decompiled/`. Предыдущие результаты:
[память/ресурсы/возврат](STAGE5_REPORT_RU.md),
[файлы/графика/ввод](STAGE4_REPORT_RU.md),
[GUI/NGame/85 слотов](STAGE3_REPORT_RU.md), [bootcfg/customize](STAGE2_REPORT_RU.md).

В `src/` добавлены проверяемые host-модели boot policy, релокаций и flag poll.
Это отдельные алгоритмы, а не собираемый исходник всей заводской прошивки.


- `src/mmp_inspect.c` — восстановленные алгоритм сумм MMPi и обход MMPd/MMPs,
  с добавленной проверкой границ. Работает на компьютере, не пишет по адресам
  целевой памяти. Это реконструкция, а не исходник производителя.
- `tools/audit_boot.py` — проверка SHA-256 обоих оригиналов, точных ARM-вызовов,
  сегментов и пяти путей stock loader/go. Требует локальные оригиналы.
- `evidence/boot_audit.json` — результат проверки предоставленных образов.
- `evidence/*.asm` — выбранные фрагменты дизассемблера, не полные образы.
- [REPORT_RU.md](REPORT_RU.md) — выводы и оставшиеся пробелы.

`make test` выполняет синтетические тесты без заводских файлов. При установленной
переменной `SPMP_MMI_RAP` дополнительно проверяется точный оригинальный RAP.

Дополнительно:

- `tools/ngame_inspect.py` — ограниченный по размерам host-инспектор NGame,
  декодирование и footer-проверка. Не подтверждает исполняемость или ABI.
- `tools/emulate_native_prefix.py` — опциональная изолированная проверка
  host-алгоритма и таблицы API оригинальным ARM-кодом, требует локальный RAP
  и `requirements-emulation.txt`. Не запускает прошивку на устройстве.
- `tools/audit_stage3.py` — точные ARM-проверки GUI/NGame/API по оригиналу.
- `src/oem_layouts.h` — числовые 32-битные структуры, не callable bindings.
- `src/mmi_native_abi.h` — типизированное подмножество native ABI без привязки адресов.
- `tools/native_abi_models.py` — host-модели выбранных файловых, графических,
  клавиатурных и allocator-предикатов.
- `tools/audit_stage4.py` — 56 точных ARM-проверок и 17 объявлений/слотов.
- `tools/emulate_native_abi.py` / `arm_abi_vm.py` — отдельная проверка 99 сценариев
  оригинальными ARM-wrapper с явными backend-заглушками, требует Unicorn и RAP.
- `src/mmi_native_resources_abi.h` — 7 объявлений resource/exit, layouts table/buffer/pool-info.
- `tools/native_lifecycle_models.py` — модели свежего пула, reserve-предикатов,
  состояния звука, смешивания samples и порядка exit-вызовов.
- `tools/audit_stage5.py` — 75 точных ARM-проверок, 21 constructor и resource/exit slots.
- `tools/emulate_native_lifecycle.py` — 119 новых сценариев: настоящий allocator,
  Game thread, WAV/resource wrappers, worker и возврат подставного BIN entry.
- `tools/native_memory_plan.py` — bounded planner для supplied kind-6 allocation,
  загрузочной/runtime фаз, heap/stack и protected OEM ranges; результат только model_consistent.
- `tools/emulate_display_pcm.py` / `display_pcm_models.py` — 53 сценария LCD/PCM/delete;
  отдельные allowlisted register cells в Unicorn, реальный DMA/IRQ не исполняется.
- `tools/audit_stage6.py` — 58 ARM/data checks, объединённые JSON/CSV 28 объявлений.
- [examples/native_probe](examples/native_probe/README_RU.md) — freestanding core,
  typed adapter и host-пример с экраном/вводом/возвратом. Нет target entry/пакета.

Контрольный рабочий образец:

- `tools/control_game_inspect.py` — exact-SHA structural/startup/resource audit,
  сравнение наблюдённых диапазонов с supplied allocation assumptions.
- `tools/emulate_control_game.py` — actual RAP loader и два bounded startup
  сценария, с явными RAM-only backend mocks; нужны оба приватных файла.
- `tools/run_control_ghidra.py` — приватный Ghidra import вне checkout.
- `evidence/control_game_*.json` — метаданные и traces без игрового кода/ресурсов.
- `SPMP_CONTROL_GAME` вместе с `SPMP_MMI_RAP` включает optional private tests.

Собственный startup:

- [examples/native_startup](examples/native_startup/README_RU.md) — ARM entry,
  linker assertions, непрозрачный r0 и отдельный compiled-probe VM fixture.
- `make arm-startup-check` — требует clang/lld/llvm-nm и optional Unicorn;
  создаёт research ELF/maps/JSON в build, не целевой контейнер.
- `tools/startup_elf.py`, `emulate_startup.py`, `check_startup_link_limits.py` —
  ELF audit, ARM926 execution и отрицательные layout checks.
- `evidence/startup_*` — источник/версия сборки, traces и собственный disassembly.

Native NGame probe:

- [Памятка пакета](examples/native_startup/NGAME_README_RU.md),
  `make arm-native-check`: новый RAM container, без Flash/NAND image.
- `tools/make_binding_profile.py`, `ngame_pack.py`, `emulate_native_package.py`:
  compatibility references, packing и public/private ARM chains.
- `SPMP_NATIVE_ELF` вместе с `SPMP_MMI_RAP` включает optional private chain test.
