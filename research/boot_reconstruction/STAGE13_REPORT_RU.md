# Этап 13: RAM input observer для 16 органов управления

14 ноября 2026. Новый **SPMP_KEYS_RAW.bin** записывает входные RAM state/event
рядом с единственным штатным GetKeys. Цель — найти кнопки, отсутствующие в cooked
V1 логе. Пакет VM_TESTED_ONLY; прежний аппаратный CSV подтверждает только SD0 V1.
[Инструкция и порядок действий](examples/raw_input/README_RU.md).

## Уточнения пользователя

BACK из прежних подсказок — **POWER**: короткое нажатие открывает стандартное
меню «Продолжить / выйти / сохранить / загрузить», длинное выключает консоль.
HOLD — отдельная кнопка: длинное удержание блокирует/разблокирует клавиатуру,
появляется значок ключа. MENU как отдельной кнопки нет. Громкость +/− отдельна
от L/R. Всего 16 органов управления, не 15 из предыдущего предварительного счёта.

Короткий HOLD как действие приложения и SELECT+VOL± как будущая настройка яркости
сохранены как намерения. В новом тесте сочетания измеряются, яркость не меняется,
хуки в RAP не устанавливаются. Штатный GetKeys и системные обработчики остаются;
они могут менять громкость/показывать меню/блокировать клавиатуру.

## Порядок и границы

15 одиночных подсказок без POWER по 3000 native units; затем два сочетания
SELECT+VOL по 4000, HOLD LOCK/UNLOCK по 6000, POWER SHORT по 5000. FREE начинается
после 70000 units от начала guide; предел всей сессии 80000 units / 4000 опросов.
Армирование отдельно ограничено 150 опросами / 3000 units и требует нуля во всех
наблюдаемых слоях. На плате частота native ticks отдельно не измерена.

POWER — последний и только короткий. Для возврата надо выбрать «Продолжить»;
время штатного меню входит в elapsed, и после возврата приложение может закончить
сеанс. Долгого POWER в сценарии нет. Ограничения не являются watchdog и не могут
прервать заблокированный OEM вызов. Блокировка HOLD не заменяется собственной.

## Откуда RAM состояния

State word: **0x556144**, event latch: **0x556148**. Эти ячейки выведены из
оригинальных state getter 0x2D2E40, event setter 0x2D2E50 и collector 0x2D2E6C.
Snapshot в собственном коде делает только два volatile чтения. Между ними IRQ/
producer может изменить данные: снимок не атомарный. Это MMI RAM, не GPIO.

Офлайн generator требует точный SHA RAP. Перед BSS zero сохраняются прежние
85 slots / 9 native prefixes / 7 time/SD/errno prefixes, затем три дополнительных
CRC32 spans и четыре проверки literals, указывающих на нужные RAM words.
Spans 16/20/32 байта выбраны без перекрытия. Несовпадение raw profile даёт 0x908,
до main/BSS/graphics/file calls. Проверки совместимости не заменяют полный SHA
RAM образа. Default SD0 target не включает новый raw preflight hook.

Не вызывается второй collector: он меняет TTL latch. Единственный GetKeys
сохраняет прежние OEM эффекты; дополнительные snapshots их не вызывают.
Прямых записей приложения в input globals, GPIO, MMIO или Flash нет.

## Журнал и интерфейс

Отдельный **/fat20a0/hda0/SPMPRAW.CSV**, CSV V2, 12 колонок:
event, poll, time, prompt, cooked, pressed, released, state_before, event_before,
state_after, event_after, call_units. Числа HEX. Common SD journal helper вынесен
из прежнего input test с параметром path; старый target сохраняет свой путь.
Нет truncate, append через явный seek EOF, bounded partial-write loop, fd0 valid,
close attempt на каждый успешный open. Предел 64 КиБ/сеанс, 1 МиБ/файл.

PRE сохраняет изменения до GetKeys, с NA для неизвестных cooked/after. Затем
snapshot читается снова, выполняется GetKeys и после него ещё snapshot.
SAMPLE пишет изменения любой наблюдаемой маски, TICK — периодическое состояние,
GAP — длительный GetKeys. Если OEM menu завершит приложение без возврата, PRE
может оказаться последней записью. Это не гарантирует сохранение при отключении
питания. END предшествует финальной графике/FG restore/return и не доказывает их.

KEY и S0/E0/S1/E1 видны отдельно на экране. SD ошибки содержат немедленный errno,
UI продолжает работать. Нет звука, собственных потоков, heap или framebuffer.
Decoder сохраняет слои и сеансы, не присваивает коды физическим кнопкам сам.
Кандидаты при удержании кнопок через границу подсказок требуют ручной проверки.

## Проверки

- **29 original ARM RAM pipeline cases**: state getter, event setter в RAM VM,
  16 state bits через настоящий collector/cooked wrapper, latch TTL и menu clear.
  Дополнительные чтения сохраняют latch, collector сбрасывает его после серии
  вызовов. Нижние OS/system boundaries подменены; физические кнопки не измерены.
- **29 compiled ARM926 cases**: скрытые raw с cooked0, все фазы, сочетания,
  cooked pair, short write, wrap/stalled clock, held raw launcher, keys/SD errors,
  PRE до задержавшегося API/GAP, 7 old prefix /3 raw prefix/4 literal failures.
  Snapshot код исполняется, read hook видит RAM accesses, app stores ограничены
  BSS/унаследованным стеком. Preflight rejection не меняет BSS и не читает input RAM.
- **3 original RAP full-wrapper cases**: hidden raw, no SD, short write. Loader,
  header/cipher/footer/API init/normal return исполняют оригинальный ARM. Input
  RAM и app services/OS/graphics/cache/backend подменены.
- Host raw C: hidden state при cooked0, NA в PRE, append/fd0/SD/errno, все фазы,
  POWER/HOLD/chords отдельно, FG restore. Старый journal/host test сохранён.
- Python parser/profile cases добавлены; полный набор **78** (public 73 PASS,
  5 optional private SKIP; с RAP/game/native ELF все 78 PASS). Прежние 20 compiled SD0 cases проходят после общего
  journal helper. OEM Ghidra exports остаются 291.

Evidence имеет префикс raw_input_; example/preview получены в синтетической VM.
Локальный Clang/LLD19 NGame: 9888 байт, SHA
`f08e3ef36a8b9e2ecb23fd9f0fb8d7a96467fe417e3541792d5eb94a9f112ce9`.
CI собирает собственный ELF/BIN в artifact **spmp-keys-raw-vm-tested**, retention
14 дней. Успешные прежние аппаратные проверки не переносятся на этот пакет.

Дальше нужен SPMPRAW.CSV с консоли: отдельно сопоставить state/event/cooked для
POWER, HOLD, остальных ранее пропавших кнопок и volume chords. Если какие-то
кнопки не видны даже здесь, продолжить трассировку producer в исходном RAP.

Опубликованный [RAW artifact 14094488663](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43547095571/artifacts/14094488663)
собран из `8e87181db4e08088fed0f5eef98d6ecb90655026`, CI завершился успешно.
Скачанный ZIP SHA сверён с GitHub, BIN/manifest совпали с repack ELF. Этот точный
ELF/BIN отдельно прошёл три original-RAP сценария. CI BIN — **9840 байт**, SHA
`3c806c3f5f800ed01816f517023e612aa28f6aafab7bb5c4983f738371c85a30`.
Доказательство: `evidence/raw_input_ci_rap_vm.json`. Отличие от локального Clang19
build связано со сборщиком/компилятором. Это ещё не аппаратный тест RAW/HOLD/POWER.
