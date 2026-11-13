# SPMP-OS Stage 29 — ARMv5 gpSP: libc/EABI memory runtime

## Подтверждённые изменения

Добавлен `src/runtime/spmp_minilibc.c` — самостоятельные функции памяти и строк для ARMv5 freestanding без POSIX/Linux. Поддерживаются `memcpy`, `memmove`, `memset`, `memcmp`, `memchr`, `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat`, `strrchr`, а также девять EABI helper-обёрток (`__aeabi_memcpy`, `__aeabi_memcpy4`, `__aeabi_memcpy8`, `__aeabi_memmove`, `__aeabi_memmove4`, `__aeabi_memset`, `__aeabi_memset4`, `__aeabi_memclr`, `__aeabi_memclr4`). У `__aeabi_memset*` аргументы идут (адрес, размер, байт). Копирование с перекрытием реализовано отдельно.

`tools/build_gpsp_armv5.py` теперь компилирует модуль с `-fno-builtin`, добавляет его к ARMv5-сборке gpSP и проверяет успешное частичное связывание. 15/15 исходных C/C++ компонентов gpSP компилируются; результат — ARM ELF32/EABI5 relocatable. Количество неопределённых символов снизилось с 36 до **21**. Список оставшихся точных имён см. `build/stage27/gpsp_armv5/audit.json` и `STAGE29_STATUS.json`.

Тест `tests/stage29_minilibc_harness.c`: **53 898** проверок памяти/строк/EABI с ASan/UBSan. `tests/test_stage29_gpsp_minilibc.py`: 3 проверки архитектуры, компиляции и unresolved-символов. `make stage29-check stage28-check` проходит и сохраняет работоспособность файлового HAL и менеджера RAM, включая 100 циклов его выделения/освобождения. Устаревшее предположение Stage28 об обязательных 36 external symbols исправлено.

## Неразрешённые зависимости и ограничения

21 символ: `__aeabi_d2lz`, `__aeabi_dmul`, `__aeabi_f2uiz`, `__aeabi_fadd`, `__aeabi_fcmpge`, `__aeabi_fdiv`, `__aeabi_fmul`, `__aeabi_fsub`, `__aeabi_ui2d`, `__aeabi_ui2f`, `__aeabi_uidiv`, `__aeabi_uidivmod`, `__aeabi_uldivmod`, `__aeabi_unwind_cpp_pr0`, `__aeabi_unwind_cpp_pr1`, `atoi`, `localtime`, `printf`, `sscanf`, `strtol`, `time`.

**Не реализованы поддельными заглушками** функции float/long division, форматированного ввода, часов и unwinding: требуются подходящая ARM EABI/soft-float библиотека и HAL времени. Не завершено финальное связывание исполняемого модуля и тест на реальном Defender. Работа остальных ядер не модифицировалась. Никаких записей в RedBoot/заводскую прошивку не выполнялось.

## Команды

```sh
make stage29-check stage28-check
```

При тестировании памяти на ПК используется `-fno-builtin`, чтобы компилятор не оптимизировал реализации `memcpy`/`memset` в рекурсивные вызовы самих себя.
