# Этап 6: карта памяти, LCD/PCM и минимальный прототип

Продолжен согласованный план: подготовлены проверяемая карта размещения,
трасса LCD/PCM, условия thread-delete и прототип экрана/ввода/возврата.
Экспорт расширен с 206 до **284 выбранных функций**: MMI — 253, RedBoot — 27/4.
Новые 78 функций относятся к MMI; RedBoot на этом этапе не расширялся.
Типизированное подмножество native API выросло с 24 до **28 объявлений**.

Сборка полной заводской прошивки пока не восстановлена. `decompiled/` содержит
псевдокод Ghidra и ARM; собираемые новые программы — отдельные host-модели
и переносимый прототип.

## Проверка источника и исполнения

Только RAP SHA-256
`5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.
Свежий импорт трёх GZF в Ghidra 12.2 сверил mapped bytes, исходные GZF сохранили
хеши. Новых merge/noreturn-repairs нет. Предупреждения C сохранены в export_summary;
ARM v8 в исходных базах не заменяет ручную проверку инструкций ARMv5TE.

- `audit_stage6.py`: **58 точных ARM/data checks**, LCD driver table,
  IRQ/dataflow, PCM queue/DMA и карта всех 28 объявлений.
- `emulate_display_pcm.py`: **53 сценария оригинального ARM**: LCD+stack
  allocations, Paint, InitGraph/настройка контекста, graphics wait/FillRect,
  DMA-арифметика, очередь PCM, mono duplication и thread-delete wrapper.
- Публичные проверки: **51 Python-тест**, 3 C-модели, host-прототип и typed adapter.
  Без RAP один optional SKIP; с локальным RAP все 51 проходят.

В новом VM-инструменте отдельные register addresses отображены как
**синтетические RAM-ячейки Unicorn**. Каждое чтение/запись проверяется allowlist;
проверяются арифметика и порядок инструкций. Физических MMIO/устройства/DMA нет.
Mutex/flag wait, blitter, queue/PCM backend и scheduler unlock подменяются явно
в соответствующих тестах. IRQ handlers экспортированы и прочитаны статически;
реальные IRQ, latch, DMA completion и RTOS liveness не эмулируются.

Результаты: `evidence/stage6_verified.json`, `stage6_verified.csv`,
`display_pcm_arm_emulation.json`. CSV объединяет 28 сигнатур предыдущих и нового
этапов; он не преобразует числовые адреса в вызываемые указатели.

## 1. Две группы framebuffer и отдельный стек

Boot LCD-код 0x2A4BE8 получает буферы через 0x2B97CC. Исходная panel table
0x55BC3C использует getters 0x34C384/38C, возвращающие 320×240. Это данные
исходной таблицы данного образа, не аппаратный замер дисплея.

Для kind=1 запрос не меньше 0xB7840, индексы 0..3. Для kind=0 запрашивается
width×height×2, индексы 0..1. Адреса кэшируются по 0x555EFC/0x555F0C и берутся
из того же game pool. В startup-последовательности видны два основных и два
дополнительных буфера. Это **не** два буфера, выделяемых позднее Game thread.

InitGraph 0x2DF000 сравнивает размеры драйвера с 320×240:

| Путь | Render / scanout getters | Present |
|---|---|---|
| Совпали 320×240 | 0x2A4268 / 0x2E83C0 — штатные LCD buffers | 0x2A3FD4 |
| Иначе | 0x2DEF30 / 0x2DEF40 — виртуальные game buffers | 0x2DEF60, blitter/scale path |

При fallback указатель из 0x5641E0 попадает в virtual scanout 0x557030,
из 0x5641E4 — в virtual render 0x55702C. В прямом пути эти два game buffer
не становятся LCD-адресами автоматически. Адреса из «пустого пула» этапа 5
остаются только условным сценарием, не фактической раскладкой запущенной MMI.

Найден и stack-owner: 0x42F910 выделяет **0x100010** байт game pool и сохраняет
указатель по 0x6769B0. Повторный вызов использует кэш. В shell launch 0x430694
адрес выравнивается `(pointer+3)&~3`; запрос 0xD01 передаёт его и stack_size
**0x100000** в 20-байтной структуре. Handler 0x2D3D38 передаёт эти поля
thread-create. Free-helper 0x42F954 освобождает кэшированный блок; полнота его
runtime lifecycle ещё не подтверждена. Game thread не освобождает свой stack
в рассмотренной очистке основного игрового блока.

VM выполнила реальный allocator с четырьмя default LCD allocations, stack-owner,
затем Game thread. В этом **условном порядке без прочих allocations**:

| Область | Полученный адрес / запрос |
|---|---|
| LCD primary 0/1 | 0x01F487B0 / 0x01E90F60, по 0xB7840 |
| LCD secondary 0/1 | 0x01E6B750 / 0x01E45F40, по 0x25800 |
| Stack allocation | 0x01D45F20, 0x100010; передаваемый stack — 1 МиБ |
| Основной game block | 0x009FFF10, 0x012FAFE4 |
| Loader capacity относительно entry 0xA00000 | 0x012FAEF4 |

После Game thread его блок/два virtual buffer освобождены; LCD и stack
allocations остались. VM использует собственный тестовый SP, а не запускает
реальный RTOS с выделенным stack. Эти числа не следует зашивать в SPMP-OS.

## 2. Планировщик размещения

[`tools/native_memory_plan.py`](tools/native_memory_plan.py) принимает observed
allocation, game/LCD buffers, OEM stack/allocation и размеры будущего приложения.
Пример `examples/native_memory_profile.json` взят из описанного RAM-only сценария;
размеры payload/BSS/heap/private stack в нём иллюстративные.

Проверяются overflow/NULL/alignment, принадлежность конфигурируемому пулу,
взаимные пересечения, фактический entry, строгая OEM граница file_size<capacity,
ограничения короткого cipher-prefix и размещение разных фаз:

- load: временное чтение всего контейнера с header 128 байт и tail в 0xA00000;
- run: decoded payload, caller-initialized BSS, private heap и private stack reserve;
- protected: MMI code/BSS/extra data, LCD/game buffers, supplied OEM stack,
  audio workspace и tone library [0xF00000,0x1032000).

Load и run могут занимать одни адреса в разное время; одновременно живущие
run-регионы пересекаться не могут. Даже если payload заканчивается до audio,
временный контейнер с header может пересечь его — отдельный тест это отвергает.
Reserve audio здесь консервативная host-политика, не новая проверка оригинала.

Для прототипа выбран нижний arena до 0xF00000; область выше tone library
автоматически не объявляется heap. Private stack reserve не переключает SP:
native приложение наследует OEM stack. BSS должен инициализировать будущий
startup, BIN-loader не является полноценным ELF-loader приложения.

Успешный результат называется **model_consistent**, не «готово к запуску».
Allocation ownership на устройстве, физическая RAM и RTOS/cache ABI остаются
неподтверждёнными. План относится к kind-6 под MMI, не к раннему RedBoot.

## 3. Реальная цепочка Paint/LCD

Driver table 0x557658 связывает render getter 0x2A4268 с 0x5DCE4C, scanout
getter 0x2E83C0 — с полем +0xC структуры 0x60640C. Width/height берутся
как uint16 по +6/+4. Present 0x2A3FD4 при разных адресах:

1. Берёт mutex 0x5DCE24 и повторно читает scanout.
2. Передаёт render в setter 0x2E83F0; setter пишет state+0xC и 0x93000054.
3. При ненулевом глобале 0x5570D4 дополнительно пишет 1 в 0x93000078.
4. Сохраняет старый scanout как новый render, освобождает mutex.
5. При разрешённой capability и flags&0x10000 выполняет flag-mask(0) и wait
   по объекту 0x5DCE1C с mask=1, mode=1, без timeout в этом wrapper.

После этого native Paint копирует width×height×2 из нового scanout в новый
render, выполняет cache-sync и обновляет destination context. В VM проверены
разные/одинаковые pointers, update flag, условие ожидания и копия 153600 байт.
Реальные latch/vblank и CPU cache не проверены. При равных адресах весь блок
present/copy пропускается, Paint всё равно возвращает 0.

LCD initialize регистрирует vector 23, ISR=0x2A3020, DSR=0x2A315C.
DSR выставляет маску 0x1 того же event object 0x5DCE1C. В ISR видна запись
0x01010000 по 0x93000050 и работа с полями flags. Названия регистров следуют
dataflow оригинала; это не проверенная datasheet-модель панели.

Fallback present меняет два virtual pointer, задаёт transfer 320×240 в текущий
физический render через blitter, затем вызывает аппаратный present. Его
callback selection проверен, но корректность масштабирования/пикселей GPU нет.

### Почему одного успешного FillRect недостаточно

Слот +0xC4 (0x2DD52C) принимает `{int16 x,y; uint16 width,height}`; x/y
передаются backend с sign extension. Wrapper допускает x/y от -1024 до 1023,
width/height 0..1023. Слот +0x54 использует наблюдаемый префикс origin x/y;
полный размер входного объекта сверх этих полей не объявлен.

Graphics wait 0x2E479C проверяет `(reg[0x92006010]&0x3B)==0x10`, иначе делает
до 300 вызовов delay(100). Единицы delay здесь не установлены. Timeout даёт 1,
**но FillRect игнорирует этот возврат и возвращает 0**. В VM с постоянно busy
status проверено 300 опросов/delay и успешный status самого FillRect. Это
существенное ограничение будущего адаптера: status не доказывает готовый кадр.

## 4. PCM queue, mono и DMA

Очередь использует RAM по 0x588B60 с исходной capacity=0x42000, внутри MMI BSS.
Это отдельная область от mixer buffer 0x400 и MIDI workspace 1 МиБ этапа 5.
Reserve-helper 0x2949F0 требует минимальный threshold и **free>request**.
Для request=0 используется отдельный порог free>0x2000. Отрицательный запрос
на достаточном свободном месте даёт -1. Верхняя reserve-обёртка использует
RTOS mutex/flag и ограниченные повторения; её scheduler progress не проверен.

MMI PCM submit 0x2C567C при channels=1 дублирует каждый PCM16 sample в два
канала, запрашивая вдвое больше байт. Channels=2 копирует bytes напрямую.
Проверены первый/последующий submit и отказ reserve. Дополнительные факты:

- Первый submit с reserve=0 всё равно вызывает commit(0), driver-start и
  выставляет started=1. Последующий submit при таком отказе просто возвращается.
- Mono-loop имеет do-while: count=0 всё равно копирует один sample (4 output
  bytes), нечётный count округляет число читаемых samples вверх. В VM count=1
  также дал 4 output bytes при reserve request=2. Будущий caller должен отдельно
  ограничивать ненулевой чётный размер, а не полагаться на этот wrapper.

В 0x292D44 после подготовки buffer nodes вызывается DMA-program 0x2A14D0:

| Вычисляемое поле | Адрес с channel stride=4 |
|---|---|
| first start / end | 0x93010008 / 0x93010018, end=start+bytes−4 |
| second start / end | 0x9301004C / 0x9301005C, та же формула |
| destination | 0x93010028 ← 0x93012004 |
| control | 0x9301006C ← 0xFE или 0xDE по bit 0x100000 engine-control |

Затем engine-control 0x93012000 очищает маску 0x2000 и устанавливает 0x1 (bit 0).
Проверка нулевой длины в raw DMA helper отсутствует. VM фиксирует записи,
не выполняет перенос. Арифметика испытана и на индексах 2/3, но allocator
0x2A2E88 устанавливает только индексы **0/1**; остальные не объявляются
поддержанными аппаратными каналами.

DMA attach регистрирует vector=24+allocated_channel, ISR=0x290C5C,
DSR=0x291238. ISR подтверждает через 0x93010004 и обновляет buffer/state;
DSR связывает отдельные PCM state 4→8→16 с flag notifications. Эти PCM state
не являются состоянием EXITED в TCB eCos, несмотря на совпадение числа 16.

## 5. Условие освобождения потока и прототип приложения

Оригинальный thread-delete wrapper 0x339B18 вызывает destructor и возвращает 1
только при **TCB.state по +0x38 == 0x10**: сразу либо после kill. Если kill
вернулся, но state не стал EXITED, wrapper возвращает 0 без destructor.
Все три ветви проверены с подменёнными kill/destructor. Это уточняет условие,
но не доказывает, за какое время реальный поток завершится. Как показано
этапом 5, stopSound результат delete игнорирует; безопасное освобождение
буферов реального worker остаётся неподтверждённым.

[`examples/native_probe/`](examples/native_probe/README_RU.md) содержит
переносимый прототип, два host backend/test и typed OEM adapter. Он:

- рисует RGB565-полосы и курсор 16×16;
- получает маски ввода по одному шагу и ограничивает max_steps;
- возвращает request обычного выхода, восстанавливает FG, не вызывает gameExit;
- не включает звук, не создаёт поток, не переключает SP и не пишет файлы из core;
- проверен на ошибках размеров/present, раннем выходе и однократном finish.

Адаптер получает функции с восстановленными сигнатурами. Числовые OEM slots
он не разрешает; target-entry, linker layout и NGame-builder не добавлены.
Host-adapter не завершает штатные RTOS threads и не обещает откат полного GUI.
Поэтому это **HOST_TESTED / ARM_OBJECT_ONLY**, без DEVICE_TESTED. CI компилирует
core и adapter как ARMv5TE little-endian soft-float ET_REL, проверяет undefined
symbols, ELF32/ARM/ET_REL/EABI5 и отсутствие hard-float flag. Artifact
`native-probe-armv5-objects` содержит два object и host PPM; object не является
готовой программой/прошивкой.

## Воспроизведение и оставшиеся границы

```sh
make -C research/boot_reconstruction test
python3 research/boot_reconstruction/tools/native_memory_plan.py \
  research/boot_reconstruction/examples/native_memory_profile.json --output work/memory-plan.json
python3 research/boot_reconstruction/tools/audit_stage6.py \
  --rap /path/to/8000_MMI.RAP --output work/stage6.json
# Unicorn 2.1.4, точный локальный RAP; синтетические register cells.
python3 research/boot_reconstruction/tools/emulate_display_pcm.py \
  --rap /path/to/8000_MMI.RAP --output work/display-pcm.json
# clang/LLVM; только objects.
make -C research/boot_reconstruction arm-probe
```

Не хватает идентифицированного рабочего OEM BIN для контрольного запуска,
runtime allocations/stack capture конкретной платы, startup/BSS/cache ABI,
проверки IRQ/GPU completion и фактического завершения потоков. Выбранные пути
и тексты не заменяют эти проверки. Следующий шаг — довести candidate HAL и
условия запуска на базе контрольного BIN; отдельно остаются RedBoot board
variant, bootcfg/customize и размещение секций. Полных собираемых OEM sources
ещё нет. Оригиналы и GZF не менялись, Flash/NAND и аппаратного запуска не было.
