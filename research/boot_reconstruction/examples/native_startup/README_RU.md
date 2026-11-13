# Собственный startup: ARM-эксперимент для SPMP-OS

Статус: **ARM_VM_TESTED_ONLY / ANALYSIS_ELF_ONLY**. Здесь собираются два ELF
из собственного исходного кода. Это следующий шаг после рабочего контрольного
BIN из [этапа 7](../../STAGE7_REPORT_RU.md). Контейнера NGame и запускаемого
пакета для консоли нет; API-адреса MMI не привязаны.

- `entry.S`: вход ARM по подтверждённому адресу `0xA00000`, передача только `r0`
  в `spmp_app_main`, очистка linker-defined BSS, сохранение r4–r11/LR, временное
  выравнивание существующего SP до 8 байт и точное восстановление SP при return.
- `research.ld`: `.text/.rodata/.data/.bss`, word-aligned BSS, отсутствие
  file/BSS overlap, отдельный 16-байтный reserve для будущего footer. Ни footer,
  ни NGame header здесь не записываются. 128 КиБ — выбранный бюджет исследования,
  который не является измеренным свободным участком памяти устройства.
- `application.c`: записывает непрозрачное значение `r0` и возвращает
  `SPMP_STARTUP_UNBOUND` / `SPMP_STARTUP_NO_API`. Таблицу MMI не читает.
- `../../tests/startup_fixture.c`: отдельная VM-программа вместо `application.c`.
  Она связывает тот же startup с `probe.c` и typed adapter. VM передаёт ей
  подставные callbacks через символ в `.data`; это тест, не firmware binder.

Startup не выделяет heap/stack, не переключает режим процессора, не трогает
IRQ/CP15/MMIO и не вызывает gameExit. `.data` загружается вместе с кодом;
startup очищает только `.bss`. C++ constructors, TLS, libc и unwind не поддержаны.
Возвращается status C-приложения; исследованный MMI BIN caller его игнорирует.

```sh
# clang, ld.lld, llvm-nm; optional Unicorn 2.1.4 для ARM VM.
python3 -m venv /tmp/spmp-startup-vm
/tmp/spmp-startup-vm/bin/pip install -r research/boot_reconstruction/requirements-emulation.txt
make -C research/boot_reconstruction arm-startup-check \
  ARM_VM_PYTHON=/tmp/spmp-startup-vm/bin/python
```

В `build/native_startup/`: `application.elf`, `fixture.elf`, карты и JSON traces.
CI сохраняет их в artifact **native-startup-armv5-analysis** на 14 дней.
ELF не является форматом штатного NGame loader. Не переименовывайте его в BIN
в расчёте на загрузку: header/transform/footer и packing ещё не реализованы.

Проверяются 30 сценариев ARM926, три отрицательных linker-сценария и семь
повреждённых ELF. В VM нет оригинальной прошивки/игры, физических MMIO и RTOS.
Прототип использует synthetic typed callbacks; успешный FillRect/paint здесь
не подтверждает GPU или LCD. Размеры секций и глубина стека зависят от компилятора.

Далее нужны проверяемая привязка API к конкретному RAP, подтверждение реального
allocation/стека, IRQ/cache/LCD и целостная проверка будущего контейнера.
Полный [отчёт этапа 8](../../STAGE8_REPORT_RU.md).
