# Этап 8: собственный startup и проверка собранного ARM-кода

Созданы исходники startup и linker script для будущего MMI-hosted приложения
SPMP-OS. Они используют подтверждённые рабочим BIN правила: ARM entry
`0xA00000`, единственный определённый launch argument в `r0`, собственная
очистка BSS и обычный return в MMI. Два ELF собраны и исполнены на ARM926 VM.
Новый код не воспроизводит исходник игры и не заменяет RedBoot/RAP.

## Что собрано

| Файл | Назначение |
|---|---|
| `examples/native_startup/entry.S` | Новый startup ARMv5TE |
| `examples/native_startup/research.ld` | Проверяемое размещение секций |
| `examples/native_startup/application.c`, `startup.h` | Непрозрачный API argument, возврат UNBOUND/NO_API |
| `tests/startup_fixture.c` | VM fixture с переносимым probe и typed adapter |
| `tools/startup_elf.py` | Проверка ELF32/ARM/EABI5/soft-float, ARMv5TE attributes, segments, symbols, BSS/footer reserve |
| `tools/emulate_startup.py` | Исполнение собранных ELF и перехват только synthetic callbacks |
| `tools/check_startup_link_limits.py` | Отрицательные linker/ELF-проверки |

`application.elf` содержит startup и минимальный C entry. Он сохраняет `r0` как
число и не разыменовывает его даже при ненулевом/недопустимом адресе. Передача
реальных OEM function pointers пока не реализована. `fixture.elf` использует
тот же startup, но main из `tests/`: связан с существующими `probe.c` и
`oem_adapter.c`. Таблица **типизированных** callbacks внедряется VM по символу
в `.data`, отдельно от непрозрачного raw OEM pointer. Никакие numeric OEM slots
не приводятся к function pointers и не вызываются.

Публичных OEM Ghidra exports по-прежнему 284; typed native declarations — 28.
Новые ELF, карты и traces получены только из собственного кода.

## Контракт startup

1. Сохранить r4–r11 и LR на стеке вызывающего, запомнить SP сохранённого frame.
2. Временно округлить SP вниз до 8-байтной границы для нового AAPCS C-кода.
3. Сохранить `r0`, очистить `[__bss_start__,__bss_end__)` 32-битными нулями.
4. Передать исходный `r0` в `spmp_app_main`; r1–r3 не читать как launch inputs.
5. Восстановить точный SP frame, r4–r11/LR; `BX LR` возвращает в ARM либо Thumb.

Исходная MMI и игра используют старые APCS-подобные frame sequences. Они не
доказывают, что каждый caller предоставляет 8-byte SP. Поэтому проверены SP%8
равные 0 **и** 4; отдельный новый стек не назначается. Caller должен предоставить
word-aligned пригодный стек. Сохранение IRQ state, cache, RTOS ownership и
обработка аппаратного прерывания этой VM-проверкой не сертифицируются.

`.data` не копируется из ROM: для будущего BIN её initialized bytes должны
находиться в загружаемом span вместе с кодом. Startup очищает только BSS.
Heap не нужен переносимому probe и здесь не выделяется. Startup не вызывает
InitRes, gameExit, scheduler, CP15/MMIO и не меняет режим процессора.

C-приложение возвращает свой status в r0. Это допустимо для ранее изученного
MMI BIN caller, который игнорирует result и идёт по обычному cleanup path.
Возврат UNBOUND не означает запуск UI или готовый HAL.

## Linker layout и будущий footer

Linker имеет отдельные RX и RW PT_LOAD, вход первым байтом, статические
проверки границ и запрет undefined symbols. Выбран размер окна 128 КиБ от
подтверждённого entry; это **исследовательский лимит**, не найденная свободная
RAM и не размер реального Game allocation. BSS и initialized data вычисляются
по собранному коду. Адреса heap/BSS рабочей игры сюда не перенесены.

Обнаружен важный для будущего packing случай: у короткого приложения footer,
добавленный сразу после `.data`, мог бы пересечь собственный BSS. Linker
резервирует после content 8-byte-aligned промежуток в 16 байт, затем выравнивает
BSS на 16. Маркер, padding payload и контейнер не формируются. Резерв проверяется
как самостоятельное условие; нельзя считать ELF уже готовым NGame payload.

Для **локальной** сборки Clang/LLD 19.1.7 получено:

| Объект | Span initialized content от entry | BSS | Runtime end | Future footer reserve |
|---|---:|---|---|---|
| `application.elf` | 132 байта | `[0xA000A0,0xA000B0)` | `0xA000B0`, 176 байт от entry | `[0xA00088,0xA00098)` |
| `fixture.elf` | 2276 байт | `[0xA00900,0xA0092C)` | `0xA0092C`, 2348 байт от entry | `[0xA008E8,0xA008F8)` |

Это **виртуальные span** содержимого; размер файла ELF больше из-за заголовков,
file offsets и symbol table. Другой компилятор может получить другие размеры.
CI проверяет собственную сборку по символам и поведению, не по этим адресам.

## ARM-проверки

В VM выбран именно CPU **ARM926**. Отображены только research code/data,
синтетическое окно унаследованного стека и фиксированные VM trap addresses.
Оригинальные RAP, RedBoot и игра не загружаются. Store hook запрещает ARM-записи
вне BSS/стека, fetch hook запрещает выход за `.text` и явные synthetic callbacks.
SP также должен всё время оставаться в исходном stack window.

| Группа | Сценарии | Результат |
|---|---:|---|
| Минимальное приложение | 12: SP%8=0/4 × raw API=0/0x5EFB74/0xFFFFFFFC × ARM/Thumb return | PASS |
| Связанный ARM probe | 18: два SP alignment × девять сценариев | PASS |
| Недопустимый link layout | 3 | Reject, как ожидалось |
| Повреждённый ELF | 7 | Reject, как ожидалось |

В 30 positive ARM-прогонах проверены обнуление BSS **до** main, forwarding r0,
сохранность data/padding и BSS boundary guard, выравнивание SP перед C/API,
точное восстановление SP/r4–r11 и возврат нужным instruction set. Callback traps
намеренно портят caller-saved r1–r3/r12, чтобы не скрыть ошибки ARM ABI.

Девять сценариев probe: bounded no keys (3 кадра), выход на первом poll,
движение/выход (2 кадра, cursor 158×118), неверные dimensions, ошибка keys,
FillRect, Paint, отсутствующий callback и нулевой лимит шагов. Проверены status,
число кадров, нормальное/ошибочное завершение и восстановление FG.
Pixels не рисуются физическим LCD: callbacks только записывают параметры.

На этих локальных путях максимальное смещение SP — 208 байт. Это измерение
данных сценариев/компилятора, не достаточный стек для SPMP-OS целиком. Окно
в 8 КиБ ниже входного SP — ограничение VM; его наличие на плате не установлено.

Отрицательные link cases: переполнение research budget, смещённый entry,
столкновение footer reserve с BSS. ELF cases: неверные machine/CPU/entry,
hard-float, RWX segment, memory span за бюджетом и усечённые program headers.
Статический аудит также запрещает relocation/undefined symbols, неподдержанные
allocated sections, constructors/TLS/dynamic layout в этом эксперименте.

## Воспроизводимость

```sh
make -C research/boot_reconstruction test
python3 -m venv /tmp/spmp-startup-vm
/tmp/spmp-startup-vm/bin/pip install -r research/boot_reconstruction/requirements-emulation.txt
make -C research/boot_reconstruction arm-startup-check \
  ARM_VM_PYTHON=/tmp/spmp-startup-vm/bin/python
```

Нужны clang/ld.lld/llvm-nm. Для альтернативных имён предусмотрены `ARM_CC`,
`ARM_LD`, `ARM_NM`. Local toolchain получен из официальных Debian LLVM 19
packages в пользовательский каталог; system/sudo configuration не менялись.
CI устанавливает свои инструменты и выполняет полный link/VM audit.

Сохранены `evidence/startup_build.json` (compiler/source hashes),
`evidence/startup_arm_vm.json` (30 traces), `evidence/startup_negative_checks.json`
и `evidence/startup_entry.asm` (дизассемблер собственного entry). Публичные
Python-тесты прежние: 61, без private inputs — 58 PASS / 3 SKIP; три C-модели,
host probe и typed adapter также проходят. Новые ARM/link checks выполняются
отдельной CI-командой, поэтому не требуют компилятора для обычного `make test`.

В GitHub artifact `native-startup-armv5-analysis` на 14 дней сохраняются
собственные ELF/maps/layout/traces и версии инструментов. Это **analysis ELF**,
не NGame, Flash image или подтверждение запуска на устройстве.

## Что осталось для запуска на консоли

Нужны image-specific API binding с проверкой identity/slot contracts,
подтверждённые runtime allocation и stack extent, окончательный packing и
проверка cache/IRQ/LCD/cleanup на плате. Сейчас точный startup проверен на
собранном ARM-коде, а связь с живой MMI и запуск интерфейса остаются незавершёнными.
Отдельный RedBoot/SD boot по-прежнему требует определения board variant и
доработки соответствующих ветвей. Оригинальные образы не менялись.
