# SPMP-OS Stage 30 — gpSP ARMv5: числовые операции и ABI деления

## Реальные изменения

1. `src/runtime/spmp_numeric.c`: freestanding `strtol`, `atoi`, побитовое беззнаковое 32-битное деление `spmp_udivmod32` без стандартной библиотеки. Обработка оснований 2–36, знака, префикса `0x`, отсутствия цифр, насыщения на LONG_MIN/LONG_MAX. Нулевой делитель требует проверки вызывающим кодом; helper возвращает 0, а не маскирует аппаратную ошибку исключением.
2. `src/runtime/spmp_aeabi_uidivmod.S`: ARM EABI `__aeabi_uidivmod`, возвращает частное через r0 и остаток через r1. `__aeabi_uidiv` реализован на C; ни одна функция не использует ARMv6/ARMv7 инструкции.
3. `tools/build_gpsp_armv5.py`: компилирует оба файла и связывает с реальным gpSP. `15/15` файлов gpSP собираются для ARMv5TE (интерпретатор, без JIT). Число внешних символов снизилось с 21 до 17.
4. `tests/stage30_numeric_harness.c`, `tests/test_stage30_numeric.py`, `make stage30-check`: 400027 арифметических и лексических проверок, ASan/UBSan, проверка ELF, EABI-символов и зависимостей. Обновлена устаревшая проверка Stage29, чтобы она разрешала дальнейшее уменьшение числа зависимостей.

## Проверки

- `make stage30-check`: успешно.
- `make stage28-check stage27-check`: успешно (файловый HAL и 100 циклов heap-адаптера).
- ARM ELF32/EABI5: частично связанный перемещаемый объект `build/stage27/gpsp_armv5/gpsp_interpreter_core.o`.
- **17 внешних символов**: `__aeabi_d2lz`, `__aeabi_dmul`, `__aeabi_f2uiz`, `__aeabi_fadd`, `__aeabi_fcmpge`, `__aeabi_fdiv`, `__aeabi_fmul`, `__aeabi_fsub`, `__aeabi_ui2d`, `__aeabi_ui2f`, `__aeabi_uldivmod`, `__aeabi_unwind_cpp_pr0`, `__aeabi_unwind_cpp_pr1`, `localtime`, `printf`, `sscanf`, `time`.
- Отдельная ARM аппаратная проверка настоящей инструкции `__aeabi_uidivmod` не выполнялась: её ABI проверен по ассемблерному коду, линковке и отсутствию внешнего символа. Общая функция деления проверена на ПК.

## Ограничения

ARM soft-float, 64-битное EABI-деление, unwinding, `sscanf`, `printf` и RTC остаются зависимостями. Сам gpSP ещё не связан в запускаемый SPMP-OS-модуль; SD/FAT, LCD, звук, Power и RAM-карта Defender остаются аппаратно непроверенными. Не изменять RedBoot/FSP до надёжного механизма восстановления.
