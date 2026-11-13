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
