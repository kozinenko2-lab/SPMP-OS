# Этап 9: проверяемая native-привязка и собственный NGame

14 ноября 2026 г. Добавлены source-built NGame packer, совместимость с конкретной
MMI и preflight до очистки BSS. Собственный диагностический пакет проходит
оригинальный загрузчик RAP в ARM926 VM: загрузка → entry → typed API calls →
обычный return → MMI cleanup path. Аппаратный запуск не выполнялся.

Это прототип на базе SPMP-OS: один кадр полос/курсора и автоматический возврат,
либо выход на первом input poll с маской `0x80000`. Полный UI SPMP-OS сюда ещё
не перенесён. RedBoot/RAP/NAND/ROMFS остаются неизменными.

## Новые исходники

| Файл | Результат |
|---|---|
| `examples/native_startup/native_binding.c/.h` | Preflight, memory gates и выдача девяти typed callbacks |
| `examples/native_startup/native_app.c` | Один bounded кадр существующего probe, обычный return |
| `examples/native_startup/binding_profile.h` | 85 ожидаемых slots и девять code CRC32, без OEM code bytes |
| `tools/make_binding_profile.py` | Exact-SHA offline extraction и проверка generated references |
| `tools/ngame_pack.py` | Flatten PT_LOAD initialized bytes, footer, prefix transform, собственный header |
| `tools/emulate_native_package.py` | Public synthetic VM и private original-RAP chain |
| `tests/binding_profile_synthetic.h` | References к собственным byte patterns для CI |
| `tests/test_native_package.py` | Проверки provenance, typed contracts и наблюдённых ARM outcomes |

У общего `entry.S` добавлен условный `SPMP_NATIVE_PREFLIGHT`: bound target
вызывает preflight до zero loop и переходит к восстановлению стека при отказе.
Default UNBOUND и fixture targets этапа 8 сохраняют прежнее поведение.
`startup_elf.py` допускает merge/string attributes для read-only `.rodata`;
записываемые/исполняемые либо неизвестные allocated sections по-прежнему запрещены.

`ArmVM` принимает optional CPU model до первого mapping и optional initial SP.
Это позволяет исполнить RAP на ARM926 и на реально выделенном в **синтетической
RAM** stack block; defaults прежних тестов не менялись.

## Как проверяется привязка

Офлайн generator требует точный SHA оригинального RAP:
`5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.
85 slots извлекаются из инструкций API initializer, первые 32 байта девяти
выбранных функций читаются из этого же образа и сворачиваются CRC32.
Публикуются адреса/контрольные суммы; исходные code bytes не включены в header.

Preflight выполняет следующие gates до startup zero loop:

1. `r0 == 0x5EFB74`, затем все 85 RAM slots равны ожидаемым значениям.
2. CRC32 девяти выбранных code prefixes соответствует generated profile.
3. `native_kind == 6`; allocation base ненулевая, 16-aligned, не выше entry;
   capacity не завёрнута, в пределах configured pool, строго больше file bytes,
   а code/BSS помещаются в переданный диапазон.
4. Shell stack owner из `0x6769B0` пригоден; C SP находится в его 1 MiB extent,
   с выбранным запасом не менее 8 КиБ до нижней границы. Stack allocation
   0x100010 не пересекает основной блок.
5. Два virtual framebuffer pointer ненулевые, 16-aligned, достаточны для
   320×240×2; в pool, не пересекают allocation/stack/друг друга.

Address constants, размер shell stack и configured pool получены на этапах 5–6.
Здесь нет нового guessed heap/stack address. На private VM выполнены настоящий
pool constructor, семь prefix allocations и main allocation; их результаты
сверены с прежней моделью. Это не capture адресов физической консоли.

До BSS очистки сохраняются регистры на унаследованном стеке. Поэтому preflight
не обещает запуск из произвольной среды/невалидного SP. В C binder output
обнуляется, проверки повторяются, и только после успеха публикуются callbacks:
InitGraph, SetDisplayOrigin, EnableDoubleBuffer, dimensions, Get/SetFG,
FillRect, Paint, GetKeys. Все девять уже имеют typed contracts этапов 4–6.

Runtime CRC32/table match — проверка **совместимости выбранных интерфейсов**,
не полный SHA RAM-образа и не проверка всех инструкций функций. Изменения вне
контрольных prefixes могут остаться незамеченными. Контекст, IRQ/cache и
lifetime потоков требуют аппаратной проверки. Полного runtime identity proof
или исправления слабых мест OEM-прошивки не заявляется.

| Status | Причина отказа |
|---|---|
| `0x901` | Несовпадение raw API argument |
| `0x902` | Slot mismatch |
| `0x903` | Code-prefix CRC mismatch |
| `0x904` | Kind/allocation/capacity |
| `0x905` | Stack owner/extent/запас |
| `0x906` | Framebuffer pointers/overlap |

На preflight rejection BSS остаётся нетронутым, graphics/input calls отсутствуют,
SP/r4–r11 восстановлены. MMI caller игнорирует application status и всё равно
выполняет свой normal-return cleanup; status виден в VM/debugger, не в GUI.
Изменение live state между двумя проверками не сертифицировано как атомарное.

## Упаковка NGame

Packer принимает проверенный собственный ELF и переносит только file-backed
PT_LOAD bytes по их виртуальным offsets от `0xA00000`. Промежутки нулевые;
BSS не сериализуется. В linker reserve помещается `SUNPLUS556GAME\0` с остатком
16-байтного footer span. Первые min(payload,1024) байт преобразуются mode=0,
обратным восстановленному OEM decoder. Затем создаётся собственный header:
`NGame1.0`, length=128, имя `SPMP Probe`, tail=0, LCM byte=5.

После упаковки обязательны structural check и побайтный decode round-trip.
Ни payload, ни header, ни JPEG/EXIF/звук игры не копируются.
Неизвестные header metadata остаются нулевыми; index=5 — наблюдение рабочего
контрольного файла, не доказательство универсальных требований LCD/frontend.

Для **локальной сборки Clang/LLD 19.1.7**:

| Поле | Значение |
|---|---|
| Файл | `SPMP_PROBE_VM.bin` |
| Container | 3656 байт, `0xE48` |
| Payload | 3528 байт, `0xDC8` |
| BSS | `[0xA00DD0,0xA00E04)`, 52 байта |
| Runtime span от entry | 3588 байт |
| Container SHA-256 | `b64181b362fa6a697b6375a0128df1e4be546f9a96f1088fd12ed2e1e76acf18` |
| Payload SHA-256 | `3ee11de24afa9f82b040fbea4fc20eb7425563112aa1c741fc1f4729173d33d6` |

Header+payload initial read временно продолжается за runtime/BSS, как у
контрольной игры. Gates проверяют allocation и для file bytes, и для BSS.
Числа зависят от компилятора; CI manifest относится к своему конкретному BIN.
128 КиБ в linker остаётся research budget, не новым объявлением свободной RAM.

## Что исполнялось в VM

**116 public synthetic scenarios:** успешный кадр, quit-first, FillRect error
при двух SP alignments; каждый из 85 slots, каждый из девяти prefixes,
неверные raw pointer/kind/base/capacity/stack/frames и недостаточный stack room.
Для rejected cases проверены нетронутый BSS, отсутствие API calls, guard и
восстановление SP/r4–r11. CI использует отдельный header с CRC собственных
byte patterns; никаких приватных OEM instructions для него не требуется.

**Шесть private RAP scenarios:** normal frame, quit-first, slot corruption,
code corruption, bad magic и bad footer. В normal cases оригинальный RAP:

- исполняет file wrapper с контролем strict capacity, header parser, все memcpy,
  prefix cipher и footer compare; decoder output побайтно равен собственному payload;
- создаёт исходную API table, вызывает наш entry и binder с настоящими RAP
  prefix bytes, затем выполняет negative-mode `native_exit_control`;
- возвращает 0 из BIN wrapper; последовательность commands — D06,D08,D07,D07.

Bad magic/footer дают `0xFFFFFFFE` из BIN wrapper без entry. Slot/code mismatch
возвращают `0x902`/`0x903` из приложения; wrapper игнорирует их и завершает
normal-return path. Это уточняет, почему одного wrapper status недостаточно
для диагностики будущих несовместимостей на устройстве.

Подменены cache/CP15, open/stat/read/close backends, general-heap scratch,
clock/delay, sound start/stop, suspend, command/message backends и все девять
graphics/input API entry. Оригинальные API initializer, memcpy и native exit
исполняются. LCD/GPU, IRQ/cache, реальный input/audio/RTOS и завершение потоков
этим прогоном не проверены. Прежние cleanup caveats этапов 5–6 сохраняются.

Доказательства: `evidence/native_binding_profile.json`, `native_binding_vm.json`,
`native_package.json`, `native_package_rap_vm.json`. Public package manifest и
private RAP test совпадают по container/payload/ELF SHA локальной сборки.

## Повторение и доставка

```sh
make -C research/boot_reconstruction test
make -C research/boot_reconstruction arm-native-check ARM_VM_PYTHON=/path/to/venv/bin/python
# Для локального original-RAP прогона; пакет/игра производителя не нужны.
/path/to/venv/bin/python research/boot_reconstruction/tools/emulate_native_package.py \
  --native-elf research/boot_reconstruction/build/native_startup/native.elf \
  --rap "$RAP" --output /tmp/native-rap-result.json
```

`make_binding_profile.py --check` проверяет generated headers/JSON по приватному
RAP, не изменяя его. Исходный RAP проверен также после VM-прогонов. Базовые
30 ARM startup и negative link/ELF checks этапа 8 сохранены в CI.
Public tests: 69 Python — 64 PASS/5 optional SKIP; с локальными RAP, рабочей игрой
и новым ELF — все 69 PASS. Три C-модели, host probe и typed adapter — PASS.

CI artifact **native-probe-ngame-vm-tested**, retention 14 дней, содержит
собственный BIN/ELF/map/manifest и public traces. Synthetic ELF не публикуется
в этом artifact. [Памятка пакета](examples/native_startup/NGAME_README_RU.md).
Этот файл — RAM-приложение под MMI, не deployable Flash image.

Далее нужны проход frontend metadata/icon/LCM, подтверждение runtime условий
и реальной графики/cache/IRQ/cleanup на плате. Полная оболочка SPMP-OS и
самостоятельный RedBoot/SD boot остаются следующими задачами. OEM Ghidra exports
и typed declarations пока остаются 284/28.

## Проверен точный опубликованный CI artifact

Для source revision `ac3d59b` завершился CI run `43510486747`.
[Artifact 14076639604](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43510486747/artifacts/14076639604)
скачан, ZIP SHA сверена с GitHub metadata, BIN побайтно воспроизведён из соседнего
ELF. Размер CI пакета — **3704 байта**; SHA-256:
`9dcef4e8593ef44b93c69d44d570e78e947bae9e35905107a89f286f2fc35782`.

Шесть original-RAP сценариев повторены для **этого** ELF/BIN, все PASS.
Доказательство — `evidence/native_package_ci_rap_vm.json`: artifact/run/source
revision, ZIP/container/payload/ELF SHA, outcomes и явные backend mocks.
Локальный пакет 3656 байт выше относится к Clang 19; опубликованный CI пакет
проверен отдельно, поэтому изменение compiler output не подменяет доказательство.
Данный artifact доступен 14 дней. Это по-прежнему VM_TESTED, без hardware run.
Проверка return заканчивается после native_bin_loader; финальное освобождение
GameThread исследовалось отдельно на этапе 5.
