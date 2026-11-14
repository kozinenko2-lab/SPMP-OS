# Этап 10: интерактивный input logger

SPMP_INPUT_TEST.bin: release-to-arm, 13 prompts, cooked MMI masks/edges,
курсор и меню LIVE/MAP/EXIT по записанным UP/DOWN/A. Номинальный предел 60 секунд
и 3000 polls. Журнал /fat20a1/hda1/SPMPKEYS.CSV открывается напрямую через
проверенные eCos services: create без truncate, seek EOF, bounded writes, close.
Ошибки SD показываются; UI продолжает работать, внутреннего fallback нет.

19 ARM VM + 3 full RAP wrapper cases + 20 original ARM service cases — PASS.
73 Python tests: public 68 PASS/5 SKIP, private 73 PASS; host C journal/error — PASS.
Ghidra exports теперь 287 (256 MMI/31 RedBoot), добавлены FAT open/write и
storage selection. Новый Input build добавляет 6 локальных typed interfaces.

[Отчёт этапа 10](../research/boot_reconstruction/STAGE10_REPORT_RU.md),
[инструкция пакета](../research/boot_reconstruction/examples/input_test/README_RU.md).
Artifact `spmp-input-test-vm-tested` содержит BIN/manifest/decoder.
Аппаратный статус нового input/SD пакета — VM_TESTED_ONLY; старый one-frame
контрольный BIN остаётся USER_CONFIRMED_ON_CONSOLE. Далее — CSV с реальной SD,
сопоставление кнопок и повторные запуски.

---

# Этап 9: native binding и собственный NGame

Собран RAM NGame diagnostic probe: один кадр и обычный возврат. До BSS startup
проверяет 85 slots, девять code-prefix CRC32, allocation/capacity/kind, stack
owner/SP и два framebuffer. Офлайн профиль извлекается только из точного RAP SHA.
Runtime checks — совместимость выбранных интерфейсов, не полный RAM-image SHA.

116 public synthetic ARM926 scenarios и 6 actual-RAP loader/entry/return cases
прошли. Оригинальный RAP принимает собственный container/decode/footer;
cache/file/audio/OS/graphics/input backends в VM подменены.
**14 ноября 2026 пользователь подтвердил аппаратный запуск artifact 14076639604:
кадр с тремя разноцветными полосами и выход из приложения.**
[Аппаратное свидетельство](../research/boot_reconstruction/evidence/native_probe_hardware_user_report.json)
имеет статус USER_CONFIRMED_ON_CONSOLE; это пользовательское наблюдение, не RAM/IRQ trace.

Точный опубликованный CI BIN (artifact 14076639604, 3704 байта) скачан и
отдельно прошёл те же 6 original-RAP сценариев; SHA и provenance сохранены.

[Отчёт этапа 9](../research/boot_reconstruction/STAGE9_REPORT_RU.md).
CI собирает BIN/ELF/maps/manifest в artifact `native-probe-ngame-vm-tested`.
Это диагностическое RAM-приложение, не готовая оболочка или Flash-прошивка.
Далее — интерактивный ввод и сопоставление кнопок, повторные запуски и
длительная работа, расширение UI SPMP-OS. Детали cache/IRQ/cleanup и универсальная
metadata/LCM совместимость требуют дальнейших проверок. Экспорты OEM/API прежние: 284/28.

---

# Этап 8: собственный startup и linked ARM ELF

Новый startup сохраняет r4–r11/LR и исходный SP, выравнивает унаследованный
стек для AAPCS C, очищает linker-defined BSS и передаёт только r0. Два ELF из
собственного кода собраны для ARMv5TE/EABI5/soft-float: минимальный UNBOUND entry
и отдельный VM fixture с существующим probe/typed adapter.

30 сценариев ARM926 подтверждают BSS/data/stack/return, вызовы compiled C и
обработку ошибок. Три неверных link layout и семь malformed ELF отвергнуты.
Linker резервирует место под будущий footer до BSS; 128 КиБ — research budget,
не измеренная свободная RAM. Нет контейнера, OEM pointer binding или device run.

[Отчёт этапа 8](../research/boot_reconstruction/STAGE8_REPORT_RU.md).
Далее: image-specific native API binding, runtime allocation/stack capture,
packing и проверка IRQ/cache/LCD/cleanup. RedBoot variant по-прежнему не выбран.
Счётчики OEM exports/typed declarations остаются 284/28.

---

# Этап 7: рабочий контрольный BIN

Пользователь предоставил запускающийся на его консоли BattleGround BIN.
Оригинальный NGame loader из RAP в RAM-only ARM VM принял файл; payload
побайтно совпал с host decode. Подтверждены entry 0xA00000, API в r0,
очистка 963 слов BSS, собственный allocator ниже 0xE00000 и обычный return.
Два bounded startup-сценария проверяют SP/r4–r11; графика/ресурсы/ввод подменены.

Исследованы реальная последовательность графических API и таблица 12 WAV /
9 MIDI. Все WAV: mono PCM 8000 Hz / 8 bit. Учтено временное пересечение extra
JPEG tail с будущим BSS. Новые инструменты exact-SHA, публичные метаданные и
10 regression tests; private game/Ghidra/asset exports остаются вне checkout.

[Отчёт этапа 7](../research/boot_reconstruction/STAGE7_REPORT_RU.md).
Далее: собственный startup/linker layout SPMP-OS по подтверждённым правилам,
проверка runtime allocation/stack на плате, нижние LCD/IRQ/cache backend.
Рабочий образец не устанавливает вариант RedBoot и не подтверждает запуск
SPMP-OS. Публичных OEM exports по-прежнему 284, typed declarations — 28.

---

# Этап 6: LCD/PCM, stack-owner, memory planner и прототип

Экспортированы 284 выбранные функции (253 MMI + 31 RedBoot). Новый этап:
58 точных ARM/data checks, 53 сценария оригинального ARM с синтетическими
register cells, 28 типизированных объявлений API суммарно. Уточнены отдельные
boot LCD allocations, virtual game buffers, переключение scanout, IRQ/DSR,
PCM queue и DMA dataflow. Найден pool-owner стека: allocation 0x100010,
в Game thread передаётся 1 МиБ с динамической базой.

Добавлены фазовый host memory planner и минимальный C-прототип полос/курсора,
ввода и обычного возврата, с typed adapter и host-тестами. ARM ET_REL objects
проверяются в CI; рабочего пакета NGame/target entry нет. FillRect скрывает GPU
timeout, PCM submit имеет слабые проверки размера/первого старта; оригинал не менялся.

[Отчёт этапа 6](../research/boot_reconstruction/STAGE6_REPORT_RU.md).
Следующее: довести candidate HAL/startup/linking по контрольному рабочему OEM BIN,
получить runtime allocations/stack и подтвердить IRQ/GPU/worker completion на
конкретной плате. Для RedBoot остаются board variant, bootcfg/customize и
размещение секций. Полных собираемых OEM sources/аппаратной проверки SPMP-OS нет.

---

# Этап 5: игровой пул, звуковые ресурсы и возврат BIN

Экспортированы 206 выбранных функций (175 MMI + 31 RedBoot). Новый этап:
75 точных ARM-проверок, 119 RAM-only ARM-сценариев; 7 новых объявлений API,
24 суммарно. Установлен startup constructor пула [0x9FFF00,0x2000000),
проверены реальный allocator, выделение/освобождение блоков Game thread,
resource API и обычный возврат BIN через отрицательный cleanup mode.

Выявлены пересечение audio workspace/tone library с основным блоком, слабые
проверки allocation/capacity, ранний return stopRes и неверное zero-padding
неполного PCM block. Оригиналы не исправлялись. VM с подставным entry/backend
не является запуском рабочего BIN или всей консоли.

[Отчёт этапа 5](../research/boot_reconstruction/STAGE5_REPORT_RU.md).
Далее: нижние PCM/LCD backend, размещение SPMP-OS с учётом найденных диапазонов,
проверка thread-delete/контекста на реальном RTOS и контрольный рабочий OEM BIN.
Для RedBoot по-прежнему нужны board variant, реальный bootcfg/customize и
размещение секций. Живой HAL и аппаратный запуск пока не подтверждены.

---

# Этап 4: native ABI файлов, графики, ввода и памяти

Экспортированы 170 выбранных функций (MMI 139, RedBoot 31). Добавлены 17
типизированных объявлений, 56 точных ARM-проверок и 99 сценариев оригинального
ARM в VM с backend-заглушками. Уточнены 32-битный status fsSeek, count-out и
асимметричные проверки fd, трёхаргументный QueryImage, dimensions-only
GetFrameBuffer, заимствование пикселей LoadImage и системные действия getKeys.
General heap и game pool различаются; ни один allocator не является native-слотом.

[Отчёт этапа 4](../research/boot_reconstruction/STAGE4_REPORT_RU.md).
Далее: границы и время жизни памяти Game thread, audio/resource ABI, возврат
приложения и штатный OEM BIN как контрольный образец; для RedBoot — реальный
bootcfg/customize, размещение секций и привязка к плате. Живой HAL SPMP-OS
пока не подключён, аппаратного запуска нет. Полных исходников OEM ещё нет.

---

# Этап 3: GUI, NGame, native API

Экспорт расширен до 99 функций. Исправлены две ложные внутренние границы
native-функций и noreturn context restore. Установлены зарегистрированный GUI
callback/циклы, контейнерный путь NGame, entry BIN и массив из 85 API-слотов.
Преобразование префикса восстановлено и совпало с оригинальным ARM в 14 VM-тестах;
все 85 слотов совпали с оригинальным инициализатором при изолированной эмуляции.

[Отчёт этапа 3](../research/boot_reconstruction/STAGE3_REPORT_RU.md).
Далее: сигнатуры display/input/file/memory/audio API, контекст/стек native-entry,
успешный штатный BIN как контрольный образец, ограничения RAM и безопасный возврат.
До этих проверок структурно подходящий NGame не является рабочим приложением.
Прошивка и GZF не менялись; аппаратного запуска не было.

---

# Этап 2: декомпиляция оригинальной прошивки

Открыты три пользовательские GZF в Ghidra 12.2, сверены mapped bytes по
оригиналам, экспортированы 57 выбранных функций. Восстановлены bootcfg v3/v5,
ELF ET_REL customize-loader, четыре поддержанных типа релокаций и поведение
событий системной инициализации MMI. Добавлены три C-модели и тесты.

[Полный отчёт и ограничения](../research/boot_reconstruction/STAGE2_REPORT_RU.md).
Следующее: фактический customize.o/bootcfg, ABI модулей, критерии успешной
инициализации, GUI loop/native loader. Полной реконструкции прошивки ещё нет;
на консоли этот этап не проверялся.

---

# Текущее состояние публичной базы

Импортирована незавершённая SPMP-OS. Исторические статусы ниже сохранены
для происхождения материалов и не являются аппаратной сертификацией.
Текущие проверки: [VALIDATION.md](VALIDATION.md).
Новый этап: [восстановление загрузки](../research/boot_reconstruction/REPORT_RU.md).

Приоритет: выбор PRAW для платы → BootROM/ScanRam → RedBoot → MMP → startup MMI
→ системные службы → контракт HAL → интерфейс SPMP-OS.
Оригинальные образы и базы Ghidra остаются локальными.

---

# SPMP-OS — архитектура и план развития

## Фактический статус

**Source stage:** Stage49 + GUI/MMI/RedBoot research Stage53–55, + multimedia/theme/FM01–FM05 from independent chat. Host build succeeded on Linux x86_64. FM01..FM05 builds/tests passed. 27 Stage55 static tests passed. On real Defender no modified OEM boot image and no functioning SPMP-OS image was tested.

### OEM findings verified against original binaries

| Компонент | Адрес / факт | Примечание |
|---|---|---|
| RedBoot family 1 | ROMFS stock loader near `0x0004A998`, image loader near `0x00038F0C` | Five OEM candidates from `Images_RedBoot_201000427.img`; exact board variant not confirmed |
| RedBoot family 3 | stock loader near `0x000467F4`, image loader near `0x00036BF4` | Alternate OEM family; never flash candidate PRAW |
| RedBoot entry | `0x00030040` | ARM image entry |
| MMI entry | `0x00280040` | OEM startup; do not infer RAM availability from image load address |
| Factory MMI | `/Rom/image/8000_mmi.rap` | Linked ROMFS path in RedBoot |
| OEM diagnostic stub | `0x00298B64` | Trivial `BX LR`; large number of callers; not a proven active logging hook |
| MMI UI | `Demo GUI` thread `0x003BE5B0`, priority 25, stack 0x4000 | Thread startup after eCos scheduler |
| MMI init | `Systen initial Thread` entry `0x00447540` | OEM spelling preserved |
| MMI background | `Systen BackGround Thread` entry `0x004473BC` | eCos delay loop |
| Thread creation | `0x00339AAC` | 35 direct spawning sites identified in Stage54 |
| GUI coordinator | `0x00447640` | Reads/writes `0x90005040`, preserving its side effects is important |
| OEM FAT ops | `fatfs_fo_write` `0x00283FC4`, `fatfs_fo_close` `0x00284108` | Described Stage44; internal `fsync` appears to return zero without flushing |
| RAM hazard | around `0x0056AD90–0x006C8FDC` cleared during early MMI | Do not place cross-stage logs there |
| OEM native game | `NGame`, `SUNPLUS556GAME` signature checks | The test BIN was rejected; exact launch conditions still unknown |

Review `research/Stage53_to_55/*/evidence` for exact instruction evidence. Findings involving dynamic behavior remain hypotheses until console or hardware trace validates them. No address should be applied to arbitrary firmware revisions.

## Immediate Codex tasks (priority order)

1. **Reverse-engineer MMI/eCos startup**: create named functions in Ghidra around `0x00280040`, `0x002807C8`, `0x003BE5B0`, `0x00447640`, `0x00447540`, `0x004473BC`. Resolve hardware side effects, stack initialization, OS thread init.
2. **Trace native launcher / OEM package format**: determine why `MMI_DIAG.bin` yielded «неверный формат». Recover exact path of ROMFS emulators vs native BIN, signature/size checks, entry ABI. Avoid testing unverified executables in Flash.
3. **Document device HAL contract**: SD/FAT, LCD RGB565 320×240, buttons, GPIO, power, PCM/FM, ROMFS. Establish bounded memory and thread-safe wrappers.
4. **Run SPMP-OS UI on stock MMI/eCos** without RedBoot changes: integration test from RAM / user-run app only after OEM ABI is validated; attach FM01–FM05 and theme v9 to active UI code. Host tests first.
5. **Expand read-only instrumentation** of original MMI for boot logs, preserving normal operation; UART remains an option for earliest errors; logging to SD happens only after filesystem init.
6. **Independent SPMP-OS**: only later, after verified recovery and address/clock/pinmux mapping. SD-first RedBoot modifications are separate and currently blocked by unverified early SD/MMC access and watchdog.

## Definition of Done for each stage

- Ghidra decompiled output and ARM disassembly both attached; exact original SHA checked.
- Patches limited to `project/spmp-os/` and tools, never `firmware/originals/`.
- Tests run on host and ARMv5 builds generated when possible; note dependency and failures.
- Explicitly distinguish `SOURCE_ONLY`, `HOST_TESTED`, `ARM_OBJECT_ONLY`, `DEVICE_TESTED`.
- Roadmap updated with evidence, unresolved issues, and actual next experiment.
