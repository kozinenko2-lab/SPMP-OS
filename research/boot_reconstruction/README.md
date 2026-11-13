# Восстановление загрузочного тракта

Статический анализ оригинальных образов; не реализация полной прошивки.

**Текущий этап:** [STAGE5_REPORT_RU.md](STAGE5_REPORT_RU.md) — экспорт
206 выбранных функций из трёх пользовательских GZF, память Game thread,
resource/audio API, возврат BIN и две подтверждённые ошибки звуковых wrapper/worker.
24 объявления API суммарно; на новом этапе 119 сценариев ARM в RAM-only VM.
Псевдокод: `decompiled/`. Предыдущие результаты:
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
