# Проверки этапа 11

- Original ARM926 SD driver registration -> id0; real resolver/open/FAT path
  conversion: old id1 rejected, corrected id0 reaches backend — PASS (10 cases).
- Original ARM file/time services rerun with corrected SD path — PASS (20 cases).
- Compiled input app/virtual SD: 20 cases, all 7 extra prefixes including errno,
  IO failure errno snapshot, learned menu, previous file preserved — PASS.
- Original RAP loader -> SD0 input package -> normal return: 3 cases — PASS;
  all app services/device backends mocked.
- Host C: first errno preserved before failing close, fd0, append/partial/error/
  file/session/poll limits and menu hold — PASS.
- Python: public 68 PASS/5 SKIP, private 73 PASS. Ghidra: 291 selected exports,
  original GZF/source hashes and mapped bytes verified.

Hardware: user reports previous input test runs, but SD Open Error and no CSV.
Corrected SD0 build is VM_TESTED_ONLY; actual SD write remains pending.
[Отчёт](../research/boot_reconstruction/STAGE11_REPORT_RU.md).

---

# Проверки этапа 10

- Exact-RAP ARM926: 20 time/delay/open/FAT create/write/seek/close cases — PASS.
- Source-built input app: 19 ARM926/virtual SD cases, learned MAP/EXIT,
  IO failures/partial writes/clock wrap/poll cap/extra prefix rejection — PASS.
- Original RAP wrapper -> packaged input -> normal return: 3 scenarios — PASS.
  App services and OS/device backends mocked, no hardware SD access.
- Host C: existing data preservation, fd0, IO status, short-write bound,
  file/session/poll limits and FG restore — PASS.
- 73 Python tests: 68 PASS/5 optional SKIP without private inputs;
  73 PASS with exact RAP/control game/native ELF.
- Ghidra: 287 exports, all original GZF hashes/mapped bytes verified.
- Published CI artifact 14088835633 (9488 bytes) independently downloaded:
  ZIP digest/ELF repack identity and 3 original-RAP cases — PASS.
  BIN SHA `a04a104a548658ac9d5923ddab15a0c07b3f006c84bf0db5315ede7e0903af7c`;
  provenance: `research/boot_reconstruction/evidence/input_ci_rap_vm.json`.

New interactive/SD hardware status is VM_TESTED_ONLY. The user's previous
three-bar frame/exit report applies to artifact 14076639604 only.
[Отчёт](../research/boot_reconstruction/STAGE10_REPORT_RU.md).

---

# Проверки этапа 9

**Аппаратное подтверждение пользователя, 14 ноября 2026:** опубликованный
SPMP_PROBE_VM.bin (artifact 14076639604) запустился на консоли, показал кадр
с тремя цветными полосами и вышел. Статус USER_CONFIRMED_ON_CONSOLE относится
к этому frame/exit. [Запись свидетельства](../research/boot_reconstruction/evidence/native_probe_hardware_user_report.json).
Физический input, повторные запуски, длительная стабильность и RTOS cleanup
пользователь этим сообщением отдельно не проверял.

- Exact-SHA RAP extraction: 85 slots + девять prefix CRC32, generated headers
  consistent; оригинал до/после прогона неизменён — PASS.
- Собственный NGame: structural check + decode round-trip; actual RAP loader
  побайтно восстанавливает payload локального пакета — PASS.
- 116 public ARM926 scenarios: native binder/probe, каждый slot/prefix mismatch,
  memory/stack/framebuffer gates, BSS/return/ABI — PASS.
- 6 private RAP scenarios: file/header/memcpy/cipher/footer/API-init/entry и
  normal return/negative cleanup. Bad magic/footer entry не достигают — PASS.
- 69 Python: без private inputs 64 PASS/5 SKIP; с RAP/game/native ELF 69 PASS.
  Три C-модели, host probe/adapter проходят. Startup VM/link checks сохранены.

- Опубликованный CI artifact 14076639604 проверен отдельно: ZIP SHA от GitHub,
  BIN/repacked ELF identity, шесть actual-RAP cases — PASS. CI BIN 3704 байта,
  SHA `9dcef4e8593ef44b93c69d44d570e78e947bae9e35905107a89f286f2fc35782`.

Runtime CRC32/table checks не являются полной SHA-проверкой прошивки в RAM.
Cache/CP15, file/audio/thread/command/message и все девять graphics/input API
backends в private chain подменены. Отдельное аппаратное наблюдение выше
подтверждает запуск/видимый кадр/выход; per-call GPU/IRQ/cache semantics и
RTOS liveness оно не измеряет. Это RAM NGame, не Flash/NAND image.
[Полный отчёт](../research/boot_reconstruction/STAGE9_REPORT_RU.md).

---

# Проверки этапа 8

- Clang/LLD 19.1.7: два source-built ARMv5TE little-endian EABI5 soft-float ELF,
  no undefined symbols. Default entry не разыменовывает raw API pointer.
- ARM926 VM: 30 сценариев, BSS до main, data/guards, r0, SP alignment,
  восстановление SP/r4–r11, ARM/Thumb return — PASS.
- Связанные probe + typed adapter: bounded input/render API sequence и ошибки
  dimensions/keys/fill/paint/missing callback/zero steps — PASS.
- Linker отклоняет переполнение 128 КиБ research budget, сдвинутый entry и
  footer/BSS collision. ELF audit отвергает семь повреждённых вариантов — PASS.
- Прежние public tests: 61 Python, без private inputs 58 PASS / 3 SKIP;
  три C-модели, host probe и host-injected adapter — PASS.
- CI дополнен сборкой/аудитом/ARM VM и artifact `native-startup-armv5-analysis`.

Это собственные analysis ELFs, не упаковка NGame и не аппаратный запуск.
VM не загружает OEM прошивку и не обращается к реальным API/MMIO/RTOS. Результат
не определяет доступный стек/память на консоли и не подтверждает живой HAL.
[Полный отчёт](../research/boot_reconstruction/STAGE8_REPORT_RU.md).

---

# Проверки этапа 7

- Рабочий контрольный game SHA и decoded payload SHA сверены; private originals
  сохранили хеши. Ghidra 12.2/JDK 25 воспроизвела 7 private game exports.
- Оригинальный RAP NGame loader 0x4617E4: status 0, payload побайтно совпал с
  host decoder, extra tail сохранён при сдвиге заголовка — PASS.
- Два bounded ARM startup-сценария: main-stub и early input exit. BSS bounds,
  private allocator, 14 вызовов API без diagnostics во втором сценарии,
  SP/r4–r11 и return — PASS. Все game API перехвачены.
- 61 Python-тест: 58 PASS + 3 optional SKIP без private inputs; 61 PASS с RAP
  и контрольной игрой. Три C-модели, host probe и typed adapter — PASS.
- Control memory comparison: реальные fixed game ranges + синтетические
  allocation assumptions, tail/load-vs-BSS/run, тесный allocation и audio overlap — PASS.
- Exact-SHA private Ghidra runner запрещает output внутри публичного checkout.

Backend file-read/bulk-copy/decoder scratch/delay для loader и все backend
игры подменены явно. Это не gameplay/hardware test. В Git нет игры, assets,
C/ASM или database игры. Число OEM exports остаётся 284. Контейнер SPMP-OS,
прошивка и запись Flash/NAND не создавались/не выполнялись.
[Полный отчёт](../research/boot_reconstruction/STAGE7_REPORT_RU.md).

---

# Проверки этапа 6

- Ghidra 12.2/JDK 25: 284 выбранные функции (253 MMI + 27/4 RedBoot), mapped
  bytes совпадают; три исходных GZF сохранили хеши.
- `audit_stage6.py`: 58 ARM/data checks; таблица из 28 typed native slots — PASS.
- `emulate_display_pcm.py`: 53 ARM-сценария — PASS. Synthetic register cells
  проверяют адреса/значения/порядок, не физические MMIO, LCD latch или DMA.
- Реальный pool allocator с default LCD allocations, shell stack и Game thread:
  cache/выделение/освобождение, восстановление оставшегося пула — PASS.
- Публичные тесты: 51 Python (один optional private), 3 C-модели,
  host-прототип и typed host-injected OEM adapter — PASS. С RAP все 51 без SKIP.
- Memory planner: load-vs-run, audio/header crossing, heap/stack collision,
  stale/NULL/wrapped allocation, LCD/framebuffer overlap и strict loader bound — PASS.
- Host-прототип: RGB565 bars/cursor, scripted input, ограничение шагов,
  ошибки размеров/present, восстановление FG и однократный finish — PASS.
- CI компилирует два freestanding ARMv5TE soft-float ET_REL object и проверяет
  отсутствие undefined symbols. Это не target entry/контейнер/прошивка.

Выход FillRect при GPU timeout и PCM submit при ошибке первого reserve/zero/odd
size сохранены как OEM-наблюдения. Thread-delete изучен до точного условия
TCB.state==0x10; реальное завершение/вытеснение RTOS не сертифицировано.
Аппаратного запуска, записи Flash/NAND и изменения оригиналов нет.
[Полный отчёт](../research/boot_reconstruction/STAGE6_REPORT_RU.md).

---

# Проверки этапа 5

- Ghidra 12.2/JDK 25: 206 выбранных функций (175 MMI + 27/4 RedBoot),
  mapped bytes совпадают, исходные три GZF сохранили хеши.
- `audit_stage5.py`: 75 точных ARM-проверок, 21 startup constructor,
  7 новых resource/exit объявлений соответствуют слотам — PASS.
- RAM-only Unicorn: 119 сценариев — PASS; настоящий allocator/info/free,
  Game thread, resource/WAV wrappers и ограниченное наблюдение worker.
- Проверен путь к подставному entry с 85 слотами API и последующий возврат;
  NGame-loading, cache и thread/PCM/file backend в этой цепочке подменены.
- Подтверждены ранний return stopRes при уже остановленном MIDI и очистка
  начала вместо хвоста неполного PCM-буфера. Source models сохраняют OEM-поведение.
- Public tests: 37 Python (один optional private) и 3 C-модели — PASS;
  с локальным RAP все 37 Python проходят без SKIP. ARMv5TE soft-float syntax
  check в CI охватывает оба заголовка объявлений.
- `audit_stage4.py`: прежние 56 ARM-проверок и 17 объявлений — PASS.

Scheduler unlock и внешние backend подменены явно; RAM-пул синтетический,
physical RAM/RTOS liveness не сертифицированы. В worker проверяется PCM до
драйвера. Нет запуска на консоли, модификации оригиналов или Flash-пакета.
[Полный отчёт](../research/boot_reconstruction/STAGE5_REPORT_RU.md).

---

# Проверки этапа 4

- Ghidra 12.2/JDK 25: 170 выбранных функций (139 MMI + 27/4 RedBoot),
  mapped bytes совпадают; исходные три GZF сохранили хеши.
- `audit_stage4.py`: 56 точных ARM-проверок, 17 объявлений соответствуют слотам — PASS.
- RAM-only Unicorn: 99 сценариев оригинальных wrapper с явными backend-заглушками — PASS.
  Проверены выходы, ветвления, отдельные эффекты в RAM и SP/r4–r11 на этих путях.
- Public tests: 25 Python-тестов (один optional private), 3 C-модели — PASS;
  с локальным RAP все 25 Python проходят без SKIP. Сохранённые ARM-векторы
  проверяют границы fd, короткие передачи, RGB, image query, ввод и два allocator.
- Header layouts компилируются на host; в CI добавлена проверка синтаксиса
  объявлений ARMv5TE soft-float. Это не ARM-приложение и не проверка hardware.

Backend FS/LCD/input/audio и вытеснение RTOS не эмулируются. VM-заглушки
сохраняют остальные регистры; все пути сохранения ABI не сертифицированы.
Ни запуск на Defender, ни запись оригиналов, ни создание Flash-пакета не выполнялись.
[Подробный отчёт](../research/boot_reconstruction/STAGE4_REPORT_RU.md).

---

# Проверки этапа 3

- Ghidra 12.2/JDK 25: 99 выбранных C/ASM-экспортов, mapped bytes совпадают с оригиналами.
- Исправления GZF только в временных копиях: 2 merge, noreturn context restore;
  исходные GZF сохранили хеши.
- `audit_stage3.py`: 37 ARM-проверок, 8 cipher-таблиц, 85 API-слотов — PASS.
- Изолированная Unicorn VM: 14 сравнений host cipher с исходным ARM — PASS;
  85/85 слотов initializer совпали, clock-helper подменён только в VM.
- Public tests: 16 Python-тестов (один требует локальный RAP), 3 C-модели;
  новые NGame-тесты используют векторы, полученные от оригинального ARM.
- Добавленные проверки границ NGame строже, чем восстановленные проверки OEM;
  `structural_match` не означает, что файл исполняем.

Эмуляция охватывает криптофункцию и table initializer, а не всю консоль.
Ни hardware run, ни запись Flash/NAND, ни устройство-загрузочный пакет не выполнялись.
Подробности: [отчёт](../research/boot_reconstruction/STAGE3_REPORT_RU.md).

---

# Проверки этапа 2

- Ghidra 12.2/JDK 25: открыты три GZF; 57/57 выбранных функций экспортированы.
- Mapped bytes: 2 сегмента MMI, по 4 RedBoot v1/v3 совпадают с оригиналами.
- Два ранее отсутствовавших определения функций MMI созданы в временных базах.
- Хеши трёх оригинальных GZF после обработки не изменились.
- `audit_stage2.py`: 38 точных ARM-проверок, dispatch из 29 релокаций,
  таблица из 5 внешних символов, callsites/литералы событий MMI — PASS.
- Три C-модели (bootcfg, relocation, flag poll): сборка C11 с -Werror и тесты — PASS.
- Инспектор MMP: публичные синтетические тесты — PASS, проверка оригинала
  опциональна и требует SPMP_MMI_RAP.

Нового аппаратного тестирования нет. Псевдокод Ghidra не является собираемой
прошивкой; исходный язык баз ARM v8 требует ручной сверки CP15/CPSR с ARMv5TE.
Логи/базы инструмента и образы не опубликованы; выбранные текстовые результаты
и проверяемые исходники находятся в research/boot_reconstruction.

---

# Проверка первого публичного импорта

Локальная среда Linux, 2026-11-13. Все пункты ниже выполнены при подготовке
импорта, а не перенесены из исторических журналов архива.

| Проверка | Результат |
|---|---|
| SPMP-OS host, сборка из исходников без архивных .o/.a/.so | PASS |
| Запуск host --cores | PASS, печать реестра |
| Инспектор MMP: публичные синтетические тесты | 6 PASS, 1 optional private SKIP |
| Инспектор MMP с точным локальным RAP | 7 PASS |
| SHA-256, суммы MMI, четыре сегмента, ARM-вызовы и пять RedBoot-вариантов | PASS |
| Существующие тесты Stage55 с локальным RAP | 27 PASS |
| Манифесты FM01–FM05 и наличие 34 тем | PASS |
| FM01, host-тест модели/навигации/отрисовки | PASS |
| Полный FM01–FM05 host+ARM build | Не завершён: отсутствует clang, остановка на ARM-части FM01 |

Печать --cores не запускает каждый эмулятор. Статусы HOST_TESTED в реестре
пришли из исходной базы; этот smoke-check не подтверждает их заново.

Оригинальные прошивки и базы Ghidra находятся вне публичного Git.
Тесты с ними выполнялись локально. Ghidra не установлена; нового экспорта
баз на этом этапе не было. Аппаратные тесты, загрузка на консоли,
ARM-cross-build всего проекта и сборка установочного образа не выполнялись.
GitHub Actions настраивается отдельно; локальные результаты не означают,
что удалённый запуск CI уже завершён.

`PUBLIC_SHA256.json` фиксирует отслеживаемые исходники этого импорта, кроме
самого файла хешей. При изменении исходников обновляйте его через
`python3 tools/update_public_manifest.py` после `git add`.
Исторический FILE_SHA256.tsv относится к приватному входному архиву.
