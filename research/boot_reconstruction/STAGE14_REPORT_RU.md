# Этап 14: карта пользовательской консоли и первый SPMP OS MINI

HOLD отложена по прямому указанию пользователя. Основная работа продолжена:
измеренная карта ввода подключена к исходникам SPMP-OS, собран минимальный
MMI-hosted RAM NGame **SPMP_OS_MINI.bin**. В нём INFORMATION, CONTROLS и EXIT,
собственное короткое POWER-меню CONTINUE/EXIT, журнал SPMPOS.CSV.
Аппаратный статус нового приложения — **VM_TESTED_ONLY**.
[Инструкция](examples/mmi_shell/README_RU.md).

## Полученные аппаратные данные

Сохранён исходный [SPMPRAW.CSV](evidence/hardware/raw_input_user_console_01.csv):
39377 байт, SHA256 `e00c4027be44a774947e72110a4e52e66df4b222de0825841dfd0f1762c562b4`.
Два полных сеанса, 201 и 199 событий, оба END, 0 пропущенных строк.
Нумерация опросов/время монотонны, cooked pressed/released согласованы.
Это пользовательские данные с устройства, не аппаратная трасса исполнения.

Оба сеанса одинаково связывают подсказки с масками состояния **0x556144**:

| Кнопка | state | Кнопка | state |
|---|---:|---|---:|
| UP | 0x1 | DOWN | 0x4 |
| LEFT | 0x8 | RIGHT | 0x2 |
| A | 0x20 | B | 0x80 |
| X | 0x10 | Y | 0x40 |
| L | 0x400 | R | 0x100 |
| START | 0x1000 | SELECT | 0x2000 |
| VOL+ | 0x800 | VOL− | 0x200 |

В отдельном event **0x556148**: START pulse=0x800, POWER short=0x1000.
Одинаковый числовой бит в разных полях не означает одну кнопку:
START state1000 и POWER event1000 — разные пространства сигналов.
В обоих сеансах зарегистрированы SELECT+VOL+ state2800 и SELECT+VOL− state2200.
Удержание START остаётся state1000 после окончания cooked-пульса.
Профиль одной консоли [обновлён](device_profiles/defender_multimix_magic_user_console_01.json),
auto_apply=false. Для MINI он выбран явно; универсальность не заявляется.

Пользователь сообщил, что физические нажатия регистрировались внутри RAW_R1,
блокировка проявилась после выхода, а штатное меню короткого POWER не появилось.
Предыдущий V1 это меню показывал. Это ограничивает положительный статус RAW_R1
записью/наблюдением ввода; корректность его системного UI не подтверждена.
[Сводка и дословное сообщение](evidence/raw_input_hardware_log.json).

## Что уточнено в оригинальном RAP

Источник — точный RAP SHA `5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.
Существующие Ghidra exports collector/core и ARM-сверка остаются основой:
[collector C](decompiled/mmi/native_key_event_collector.c),
[collector ARM](decompiled/mmi/native_key_event_collector.asm),
[core ARM](decompiled/mmi/native_cooked_key_core.asm).
Новых Ghidra exports на этом этапе нет, суммарно остаётся 291.

- Collector `0x2d2e6c` даёт отдельное событие либо преобразует held-state,
  считает TTL (четыре вызова) и учитывает gate. Это RAM-потребитель, без
  вызова pause/menu/volume. Опрос должен вызывать его ровно один раз.
- Cooked GetKeys/core дополнительно фильтрует игровые маски, объединяет L с
  VOL− и R с VOL+, вызывает системные действия. Поэтому MINI использует state
  для 14 кнопок и collector только для POWER; cooked GetKeys не вызывается.
- Gate **0x556aa0**, getter **0x2d8b78**, подавляет ввод при блокировке.
  Это не доказанный «GUI foreground active». GUI lock **0x561078**,
  getter **0x3c3264**, — отдельный флаг.
- При gate1 оригинальный collector очищает state/event. POWER1000 тогда
  исчезает без pause/menu/delay; при gate0 cooked-путь вызывает системные
  command/gui/message/pause/delay. Это **возможное объяснение** RAW_R1, но
  исходный V2 CSV не записывал gate/GUI lock, поэтому причина не установлена.
- Сообщения 0x573/574 меняют gate, а при native-active не доходят до обычной
  GUI-очереди. Это ещё одна причина отдельно учитывать оба флага.

Изолированный [аудит оригинального ARM](evidence/input_system_gates_arm.json):
15 сценариев, OS/GUI/pin backends подменены; это не проверка GPIO консоли.
Дизассемблер новых getter — [input_system_gates.asm](evidence/input_system_gates.asm).
Стоковый HOLD reader не является чистым getter: настраивает GPIO/MMIO.
В новом приложении он не используется. Идентификация HOLD сейчас не продолжается.

## Реализация в SPMP-OS

Новый переносимый `spmp_mmi_input` валидирует профиль, хранит pressed/released,
не теряет удержание START, отделяет POWER от L/R и состояний, выдаёт одно
POWER-событие на TTL-пульс. Читает до/после collector оба lock-флага; при любом
установленном флаге блокирует действия, после снятия требует отпустить кнопки.
Не записывает чужие RAM-флаги. SELECT+VOL± пока только chord-события.

`spmp_mmi_shell` — отдельная проверяемая модель меню. Настоящий NGame напрямую
линкует эти два модуля из `project/spmp-os/`, существующие startup, графический
adapter и bounded SD journal. POWER вызывает собственное меню MINI, а не
объявляется исправлением штатного меню RAW_R1. Save/load здесь ещё нет.

До BSS сохранены прежние ABI/table/memory checks и добавлены два CRC/literal
профиля gate/GUI getter. Флаги только читаются. Собственные записи ограничены
BSS/унаследованным стеком; исходный collector имеет собственные RAM side effects.
Повторной полной отрисовки неподвижного HOME нет. SD-сбой не мешает выходу,
лог дописывается только на SD0, лимиты и закрытие сохранены от предыдущего logger.

## Проверки

- Host C: 14 кнопок, раздельные пространства, START hold, POWER TTL pulse,
  lock/rearm, сочетания и переходы меню — PASS.
- Compiled ARM926 MINI: **23 сценария** — навигация, POWER continue/exit,
  held-launcher, оба lock-флага, L/R/volume, short write, SD faults, clock wrap,
  time/poll caps, отказы несовместимых префиксов/указателей до BSS — PASS.
- Original RAP loader/cipher/footer/API-init → MINI → normal return:
  **3 сценария**, включая POWER continue/exit и no SD — PASS.
  Оригинальный collector выполняется, upstream RAM/часы/SD/графика подменены.
- Python: **82** (public 77 PASS/5 optional SKIP); аппаратный CSV и генерация
  профиля включены. Старые 29 raw ARM-сценариев проверяются отдельно.
- ARMv5TE/EABI5/soft-float, нет unresolved symbols; startup сохраняет SP/r4–r11.
  [Сборка](evidence/mmi_shell_build.json), [public VM](evidence/mmi_shell_vm.json),
  [original RAP](evidence/mmi_shell_rap_vm.json).

Точный опубликованный [CI пакет](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43552586927/artifacts/14097250186)
скачан и проверен отдельно: ZIP SHA, BIN=repack ELF, manifest identity,
3 original-RAP случая — PASS. BIN 10896 байт, SHA256
`8d9a4f0f495707a87cd2a91189c686ae72e5eee4e8cc70f7efe5460104f54631`.
[Доказательства CI сборки](evidence/mmi_shell_ci_rap_vm.json).

## Дальше

1. Подтвердить запуск MINI, навигацию, POWER continue/exit и обычный возврат
   на консоли; использовать SPMPOS.CSV для расхождений, без нового HOLD-теста.
2. Подключить read-only просмотр SD к MMI-hosted backend, затем компоненты
   файлового менеджера и существующего интерфейса SPMP-OS по одному.
3. Продолжить ABI graphics/audio и запуск эмуляторов; SELECT+VOL яркость
   реализовывать после проверки штатного сервиса, без угаданных MMIO.
4. RedBoot: выбрать фактический board variant и bootcfg/customize перед
   самостоятельной загрузкой. Мини-оболочка использует уже восстановленный
   NGame-путь MMI; загрузчик, NAND и заводская прошивка не изменяются.

Полных собираемых исходников оригинальных RedBoot/RAP пока нет. MINI — первый
прикладной результат восстановленного ABI, не законченная прошивка SPMP-OS.
