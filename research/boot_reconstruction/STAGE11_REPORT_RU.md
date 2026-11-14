# Этап 11: исправление SD path и числовая диагностика ошибок

**Позднейшее подтверждение:** пользователь прислал полный SD0_R2 CSV;
[этап 12](STAGE12_REPORT_RU.md) фиксирует сохранение одного сеанса на SD.
Ниже — состояние проверок на момент выпуска исправленного пакета.

14 ноября 2026. Пользователь сообщил о `SPMP_INPUT_TEST.bin`: «Тест работает,
но SD Open Error. Ничего не сохранилось на флешку». Это подтверждает запуск
input-приложения, но не сохранение журнала и не точность каждой карты кнопок.
[Свидетельство](evidence/input_hardware_sd_open_error.json).

Исправленный пакет — **SPMP_INPUT_SD0.bin**, заголовок экрана **SPMP INPUT SD0**.
Единственный путь — **`/fat20a0/hda0/SPMPKEYS.CSV`**. Аппаратный статус этого
исправления пока VM_TESTED_ONLY; успешная запись должна подтвердиться на консоли.

## Причина и доказательство

Предыдущая версия ошибочно назвала `/fat20a1/hda1` разделом SD. Ветка браузера
`0x432000` лишь обрабатывает storage selection 1; это не идентификация носителя.
Тесты этапа 10 подменяли resolver и принимали именно этот заранее заданный путь,
поэтому не могли обнаружить ошибку выбора раздела. Ошибка была в нашем logger.

Нужно следовать регистрации физических драйверов:

| Номер | Реальная регистрация в RAP | Точка выбора | Путь |
|---|---|---|---|
| 0 | `fat_sd_init`, `0x288348` | r0=0 перед `0x2831E4`, callback `0x2884C0` | `/fat20a0/hda0` |
| 1 | `fat_msdc_init`, `0x288C44` | r0=1 в `0x288C78` | `/fat20a1/hda1` |
| 2 | `fat_nf_init`, `0x2886F0` | r0=2 в `0x28873C` | `/fat20a2/hda2` |

`0x2831E4` записывает callback в `0x513CAC + id*4`. SD-функция также
регистрирует read/write callbacks с id0. Построитель mount-команды `0x4382B4`
выбирает `/fat20a0` для id0, а FAT path translation принимает `hda0/...`.
NAND имеет id2: logger туда не обращается.

В сообщениях штатной `mmi_system_init_retry` есть неоднозначный текст про SD,
но её фактический аргумент равен 2. Диагностическая строка не доказывает тип
носителя. В ходе этого этапа гипотеза о разделе 2 была проверена и отвергнута
по регистрации драйверов **до публикации и запуска на устройстве**.

Новый `audit_sd_mount.py` исполняет оригинальные ARM-инструкции регистрации SD,
mount builder, eCos open, resolver/prefix match, FAT open/path conversion и errno.
В модели установлен только подтверждённый SD mount0. Старый путь id1 получает
-1/errno2 до FAT backend; исправленный путь id0 достигает backend с именем
`/hda0/SPMPKEYS.CSV` и flags0xA. Физический контроллер/диск не эмулируется.
Это проверка связи оригинального кода и модели mount state, не снимок платы.

## Что изменилось в пакете

- Новый файл/название SD0 различают исправленный и прежний BIN.
- Путь исправлен на SD0; не добавлены перебор разделов, внутренний fallback,
  mount/unmount или запись в NAND. Сохранились append, seek EOF, fd0 support,
  пределы 64 КиБ/сеанс и 1 МиБ/файл, close после успешного open.
- Ошибки показывают исходный eCos errno в HEX: например **SD OPEN E 00000002**.
  Значение снимается сразу после отрицательного результата, до close. Ошибка
  close не заменяет уже сохранённую причину seek/write failure. Нулевая либо
  некорректная положительная длина записи даёт WRITE E 00000000: это нарушение
  контракта записи, не прочитанный errno.
- `0x299A14` возвращает адрес errno текущего потока: `TCB + 0x88`, с проверкой
  eCos allocated-index bit. Callback добавлен к семи prefix CRC32 до BSS zero.
  Wrapper читает только возвращённое слово; ручной записи TCB на консоли нет.
- Формат CSV V1 совместим с прежним decoder. Метаданные добавляют `app=SD0_R2`
  и полный path. Подсказки/маски/курсор/меню и обычный возврат сохранены.

## Проверки

- **10 original ARM SD/mount/resolver/errno cases**: регистрация SD0, статическая
  проверка NF2/MSDC1, builders0/2, отвержение старого пути и принятие нового,
  четыре errno значения. OS/physical FAT boundaries явно подменены.
- **20 original ARM service cases**: прежние time/delay/open/write/seek/close
  проверки повторены с исправленным SD0 profile.
- **20 compiled ARM VM cases**: к прежним сценариям добавлено отвержение
  повреждённого errno prefix, проверяется errno для каждого IO failure.
- **Три full RAP wrapper cases**: guided, no-SD и partial write; оригинальный
  loader принимает новый контейнер и возвращает управление. App callbacks,
  cache/OS/file backends в этой проверке подменены.
- **Host C**: append/partial/error/limits, удержание меню; новые проверки
  немедленного errno snapshot и сохранения первой ошибки при close failure.
- **73 Python**: public 68 PASS / 5 optional SKIP; private 73 PASS.
- Ghidra 12.2 проверяет исходные три GZF и mapped bytes; **291 selected exports**
  (260 MMI +31 RedBoot). Добавлены storage_mount_command, ecos_errno_location,
  fatfs_path_translate, fat_sd_register. Прежняя mmi_subsystem_init — это
  выявленная здесь NF init routine; историческое имя экспорта сохранено.

Артефакты проверки имеют префикс `evidence/input_sd0_`; данные этапа 10 сохранены
как исторические. Исходники — `examples/input_test/`, profile generator по-прежнему
требует точный SHA предоставленного RAP.

```sh
make -C research/boot_reconstruction test
make -C research/boot_reconstruction arm-input-check ARM_VM_PYTHON=/path/to/venv/bin/python
/path/to/venv/bin/python research/boot_reconstruction/tools/audit_sd_mount.py \
  --rap "$RAP" --output /tmp/sd-mount.json
```

Локальный Clang/LLD19 build — 9840 байт, SHA
`a32c6453fb95daaa1b42962bf2fe69f229b63431429df405f77b7c6238265b7c`.
CI artifact **spmp-input-sd0-vm-tested** содержит BIN, ELF/map, manifest, VM traces,
пример CSV, preview и decoder. После запуска ожидаются **SD LOG OK** и файл
`SPMPKEYS.CSV` в корне карты. Если ошибка останется, нужен полный текст с восьмью
HEX-цифрами. RedBoot/RAP/Flash/NAND/ROMFS не изменялись.

Опубликованный [SD0 artifact 14090758906](https://github.com/kozinenko2-lab/SPMP-OS/actions/runs/43538932112/artifacts/14090758906)
собран из `79e5d1f270510243c57793421f16841dceb9b725`; CI завершился успешно.
Скачанный ZIP прошёл проверку SHA GitHub, BIN совпал с repack ELF/manifest и
отдельно прошёл все три original-RAP сценария. CI BIN — **9728 байт**, SHA
`ad9ad0c793c58ae3a8b628c28ccf4b02d88a634fcaf1bac5ddfb60576ec5a081`.
Доказательство: `evidence/input_sd0_ci_rap_vm.json`. Физическая SD по-прежнему
не проверена; это проверка точных опубликованных байтов в VM.
