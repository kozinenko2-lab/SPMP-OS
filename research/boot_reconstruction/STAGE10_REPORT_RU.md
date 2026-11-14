# Этап 10: интерактивный input test и журнал SD

14 ноября 2026. После пользовательского подтверждения one-frame NGame создан
отдельный **SPMP_INPUT_TEST.bin**: подсказки для кнопок, маски/переходы/курсор,
небольшое меню и SD CSV. Новый пакет имеет статус VM_TESTED_ONLY. Аппаратное
свидетельство этапа 9 относится к прежнему BIN с полосами.

[Инструкция пакета](examples/input_test/README_RU.md) описывает запуск, меню,
ошибки и разбор журнала. Исходники — `examples/input_test/`; основной probe
сохранён отдельно, его конфигурация не стала интерактивной автоматически.

## Что делает приложение

Сначала отображается экран, затем предпринимается открытие журнала. Поэтому
до файлового вызова уже есть видимый кадр. В начале требуется отпустить кнопку
запуска: 150 попыток/3000 native time units, затем обычный выход при удержании.
Это исключает запись удерживаемой кнопки запуска как первого UP sample.

Далее 13 четырёхсекундных подсказок UP/DOWN/LEFT/RIGHT/A/B/X/Y/L/R/START/SELECT/BACK.
Первое ненулевое состояние после нулевого сохраняется как SAMPLE текущего
prompt. Зажатая кнопка не назначается заново следующему prompt. Все изменения
состояния записываются отдельно как KEY с pressed/released masks.

После подсказок включается LIVE/MAP/EXIT, если есть три различные ненулевые
кандидатные маски UP/DOWN/A. Меню использует их, а не заранее заданные коды.
MAP показывает четыре направления и A. Удержание кнопки не повторяет выбор меню. При неполной калибровке остаётся FREE.
Выбор EXIT пишет MENU/END и возвращает управление обычным `return`.

Предел сеанса — 60000 units штатного native time, независимо от меню — 3000
основных опросов. Задержка — два native ticks, nominally 20 ms. Функция времени
в этой RAP возвращает low32 tick delta ×10. Физическая длительность tick пока
не измерялась: «60 секунд» — номинальная интерпретация OEM API. Пределы не
прерывают зависание внутри API/driver. Курсор и строки обновляются по изменениям,
динамическая перерисовка ограничена 100 time units. Битовая сетка показывает
все 32 бита, последнее ненулевое состояние остаётся в LAST после отпускания.

Шрифт 5×7 и preview собственные. Нет OEM шрифтов/ресурсов игры, собственного
framebuffer/heap, звука или потоков. Графика использует ранее проверенные
InitGraph/origin/double-buffer/dimensions/FG/FillRect/Paint. При выходе
предпринимается восстановление FG. Это не сертификация RTOS cleanup.

## Почему файловый binding отдельный

`native_fs_open` по +0x13C дописывает имя к строке из `0x6769F4`. Передача ему
абсолютного SD path дала бы неверную конкатенацию. Кроме того, его stat-error
может оставить открытый fd. Здесь используются проверяемые прямые eCos services:

| Функция | Адрес | ABI |
|---|---|---|
| Native time | `0x2D3254`, slot +124 | `uint32_t(void)`, low tick delta ×10 |
| Thread delay | `0x339BE8`, slot +11C | low/high words в r0/r1, без используемого result |
| eCos open | `0x2A5ED4` | path, flags, mode → signed fd |
| eCos write | `0x2EB610` | fd, buffer, count → signed bytes |
| eCos seek32 | `0x2EB69C` | fd, signed offset, whence → position / -1 |
| eCos close | `0x2EB640` | fd → 0 / -1 |

Новые локальные typedef находятся в `input_test.h`. `input_binding.c` добавляет
шесть prefix CRC32 поверх прежних 85 slots/девяти callbacks/memory gates.
Они проверяются **до BSS zero** через условный SPMP_INPUT_BUILD hook и повторно
при binding. Несовпадение extra prefix даёт `0x907`, без file/graphics calls.
Default probe target не включает этот hook. CRC — проверка совместимости,
не cryptographic identity всего RAM-образа. Офлайн generator требует точный
original-RAP SHA `5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.

Сохранены `input_profile.json` и generated real/synthetic headers без OEM code
bytes. Новый пакет рассчитан на эту MMI: другая версия требует отдельного
проверенного профиля, даже если физически похожа и имеет другие кнопки.

## SD path и сохранность сеансов

Единственный путь записи — **`/fat20a1/hda1/SPMPKEYS.CSV`**. Stock game-base
ветка `0x432000` собирает `/fat20a1/hda1/GAME/`; storage selection читается
`0x3E2F08`. Другие ветки используют fat20a0/hda0 и fat20a2/hda2. Выбран явный SD
путь этой прошивки, без fallback во внутреннюю память; аппаратная доступность
этого mount ещё должна подтвердиться файлом на карте.

Каждая запись: open flags **0xA** (write/create, без truncate), seek(0,SEEK_END=2),
bounded write loop, попытка close. Это явное добавление к EOF, не предположение
о неизвестном APPEND bit. fd0 допускается. Обработка короткой записи — максимум
8 вызовов; ноль/отрицательный/слишком большой result, seek/close error отключают
дальнейший logging этого запуска. Экран и опрос продолжаются.

Новые данные ограничены 64 КиБ на сеанс, полный файл — 1 МиБ. Старые записи
не обнуляются, не удаляются и не заменяются. Начало сеанса имеет новую строку
и метку `#SPMP_INPUT_TEST_V1`; parser терпит частичный хвост после ошибки.
Успешный close учитывается как сохранённая запись, но power-loss durability,
filesystem integrity при аппаратном сбое и реальные driver timeouts не измерены.

CSV: event, poll_hex, time_hex, prompt, mask_hex, pressed_hex, released_hex.
События ARM/PROMPT/KEY/SAMPLE/MENU/END/END_ERROR. Маски являются **cooked MMI
GetKeys**, не raw GPIO. Volume/system actions частично поглощает штатная MMI.
Prompt labels — инструкция оператору, не физическое распознавание кнопок.

`input_log_decode.py` выдаёт отдельные сессии и candidate mappings; имя устройства
задаёт оператор. Карты разных устройств не сливаются и не применяются к OS
автоматически. При переносе SD между устройствами сохраняйте файлы с понятными
именами на ПК: проверенного аппаратного ID этот тест не считывает.

## Проверки

- **20 original ARM service cases:** time low/high/wrap, delay tuple forwarding,
  absolute open с fd0, FAT create flags, signed/partial writes, SEEK_END position,
  close errors. Подставлены только нижние OS/filesystem backend.
- **19 source-built ARM926 scenarios:** guided mode, learned MAP/EXIT, short writes,
  wrap/stalled clock, held launcher, no SD, seek/write/zero/close errors, file
  limit и каждый из шести extra prefix mismatches. Все PASS. Часы/SD/графика
  синтетические; прежние данные файла сохраняются, close предпринимается для
  каждого успешного open, preflight rejection не меняет BSS.
- **Три full RAP wrapper cases:** guided, no-SD и short-write. Исполняются
  оригинальные loader/header/memcpy/cipher/API-init/normal return; приложения
  получают реальную таблицу и code prefixes. App graphics/time/delay/file API
  entry и внешние OS/cache/audio backend подменены. Все три PASS.
- **Host C tests:** append preservation, fd0, transitions, partial/zero/negative/
  excessive write result, seek/close errors, file/session caps, timer wrap,
  poll cap при остановившемся clock; GUI продолжает работать при IO failure.
- **73 Python tests:** 68 PASS / 5 optional SKIP без private inputs.
  С RAP/контрольной игрой/native ELF — 73 PASS без SKIP.
  Parser sessions/partial rows и generated header checks добавлены отдельно.

Результаты — `evidence/input_services_arm.json`, `input_vm.json`, `input_rap_vm.json`,
`input_package.json`, `input_build.json`. `example_synthetic.csv`, decoded JSON и
preview получены только в VM. Реальные кнопки/журнал SD/интерактивная графика
нового пакета ещё не имеют hardware confirmation.

Ghidra 12.2/JDK25 заново проверила три исходных GZF и mapped bytes. Добавлены
fatfs_open, fatfs_fo_write и shell_storage_selection: **287 selected exports**
(256 MMI + 31 RedBoot). Остальные C/ASM exports не изменились. Псевдокод остаётся
результатом анализа, не полным собираемым исходником производителя.

## Воспроизведение и доставка

```sh
make -C research/boot_reconstruction test
make -C research/boot_reconstruction arm-input-check ARM_VM_PYTHON=/path/to/venv/bin/python
/path/to/venv/bin/python research/boot_reconstruction/tools/audit_input_services.py \
  --rap "$RAP" --output /tmp/input-services.json
/path/to/venv/bin/python research/boot_reconstruction/tools/emulate_input.py \
  --elf research/boot_reconstruction/build/input_test/input.elf \
  --rap "$RAP" --output /tmp/input-rap.json
```

Локальный Clang/LLD 19.1.7 пакет — 9536 байт; SHA
`543672e04823bf47d68af4e0cdcdb773bd34c900fc000d62d62f318d8314dccc`.
CI собирает свой ELF/BIN и кладёт конкретный SHA в соседний manifest. Artifact
**spmp-input-test-vm-tested**, retention 14 дней: BIN, ELF/map, manifest,
public traces, preview/example и самостоятельный CSV decoder.

Опубликованный пакет: [artifact 14088835633](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43532969624/artifacts/14088835633),
source commit `103549d395752d1d8f2c9e004f720238a8b7091f`. CI завершился успешно.
Скачанный ZIP SHA сверён с GitHub, BIN побайтно совпал с повторно упакованным ELF.
Пакет CI — **9488 байт**, SHA
`a04a104a548658ac9d5923ddab15a0c07b3f006c84bf0db5315ede7e0903af7c`.
Этот ELF/BIN отдельно прошёл те же три full-RAP сценария;
доказательство и provenance — `evidence/input_ci_rap_vm.json`.
Размер и SHA отличаются от локального Clang19 build из-за версии компилятора.
Это не аппаратная проверка SD: шесть дополнительных app services также подменены.

Никаких модификаций original RedBoot/RAP, NAND/ROMFS или Flash image нет.
Следующая аппаратная проверка — запуск, кнопки по подсказкам, наличие CSV на SD,
выход и повторный сеанс с сохранением предыдущих строк.
