# SPMP-OS Stage 32 — восстановленная единая база + gpSP runtime

Дата: 2026-11-11. Цель: Sunplus SPMP8000 (ARM926EJ-S / ARMv5TE), Defender MultiMix Magic.

**Это набор исходников и проверяемых модулей, НЕ образ для прошивки, НЕ ELF-программа для физического устройства.** Штатные RedBoot, FSP, NAND и Flash не модифицировались. Аппаратное исполнение на Defender пока не подтверждено.

## Источники проекта (из Library)

- `SPMP-OS_Stage31_gpSP_ARMv5_64bit_EABI.zip`: основной код OS, HAL, оболочка, адаптеры ядер, gpsp, PicoDrive (MD/CD/32X), SNES и сквозные тесты. Не начинали работу заново.
- `SPMP-OS_Stage10_Themes_v9_DevMerged_34.zip`: **34 независимые темы** в `features/multimedia/theme_v9/`, ничего из ядра Stage31 не перезаписано.
- `SPMP-OS_Multimedia_MP3_Player_v4.zip`: независимый libmad MP3-проигрыватель и ARMv5 артефакты в `features/multimedia/mp3_player/`.
- `SPMP-OS_Multimedia_Audio_v6.zip`: независимый fixed-point микшер, интерфейсы WMA/AMR и тесты в `features/multimedia/audio_v6/`.
- `SPMP-OS_PSX4ALL_Stage14_Final_Research.zip`: экспериментальный PlayStation core, отдельно в `research/psx4all_stage14/`; НЕ пригоден для запуска на плате без дальнейшей интеграции.
- `gpsp-master.zip`, `SPMP-OS_Stage22_Full_PicoDrive_MD_CD_32X.zip`: дополнительные исходники и контрольные предыдущие этапы использованы для сопоставления; основной Stage31 уже содержит их интеграцию.
- `Defender_RedBoot_Research_Package.zip`, `Defender_MultiMix_FSP_All_Extracted.zip`: изучены как аппаратные источники, **сырые оригинальные бинарники намеренно НЕ включены** в этот комплект. В `research/redboot_reference/REPORT_RU.md` оставлен отчёт; оригинальные архивы сохраняются в Library.

Параллельные независимые модули в `features/multimedia/` — это ещё не единое приложение. После проверки LCD/SD/DAC HAL их необходимо связать с активной оболочкой и менеджером приложений. Изменение надстройки Stage10 не равно аппаратной интеграции Stage32.

## Stage32: новый код gpSP

- `src/runtime/spmp_gpsp_services.c`: freestanding форматированный вывод `printf` для реально зарегистрированного лог-приёмника, форматы `%x`, `%X`, `%u`, `%d`, `%i`, `%c`, `%s`, `%%` и ширина поля; `sscanf` реализует точное применяемое gpSP правило `%08x %04hx` и базовые целочисленные форматы, а не выдуманную полноценную libc.
- `src/runtime/spmp_gpsp_time.c`: `time/localtime` для RTC эмулятора GBA. Валидный UTC Unix-time читается из подключённого HAL callback, с целочисленным переводом по григорианскому календарю; часовой сдвиг настраиваемый. Без аппаратного RTC явно предусмотрено **виртуальное время 2000-01-01 UTC** (детерминированное игровое время, не утверждение о часах платы). `time_t` ARMv5 32-битный.
- `include/spmp_gpsp_services.h`: интерфейс связывания логирования и часов без неподтверждённого UART/MMIO.
- `tools/build_gpsp_armv5.py` подключает адаптеры при компиляции gpSP; `tests/stage32_services_harness.c` и `tests/test_stage32_gpsp_services.py` проверяют поведение.

Фактическое состояние частичной ARMv5 линковки: 15/15 исходных модулей gpSP, **12 внешних символов** вместо 16 в Stage31. Из них 10 — soft-float `__aeabi_*`, 2 — unwind обработчики `__aeabi_unwind_cpp_pr0/pr1`. Это по-прежнему ARM ELF32 **ET_REL**, а НЕ запускаемый ELF, FSP или прошивка. Нельзя заменять оставшиеся функции пустыми заглушками.

## Проверено в этой среде

```sh
make stage32-check
make stage31-check
cd features/multimedia/theme_v9 && sh tests/run.sh
cd features/multimedia/audio_v6 && make host check
cd features/multimedia/mp3_player && make check
```

- Stage32: тесты сканирования кодов, логирования, RTC, ARMv5 cross-compilation **успешны**.
- Stage31: **200100** сравнений 64-битного деления, тесты Stage29–31 **успешны**.
- Themes v9: 34 темы и два режима отрисовки, 68 кадров, ARMv5 ET_REL, тесты **успешны**.
- Audio v6: микшер, ASF и тесты **успешны**.
- MP3 v4: тестовые MP3 и сравнение PCM с FFmpeg **успешны**.

## Следующие критические работы

1. Добавить лицензионно совместимую **полноценную** soft-float реализацию ARM EABI и правильное поведение unwind. Проверить не только список символов, но и численную точность, исключения и ABI.
2. Восстановить **подтверждённый** RAM map и безопасный путь исполнения из RAM через исходную eCos/FSP. Не прошивать Flash.
3. Подключить настоящий SD/FAT HAL, LCD RGB565, клавиши, DAC/DMA и RTC callback. Без этого невозможно считать gpSP или меню запускаемыми на плате.
4. Встроить независимые темы, MP3, Audio v6 и экспериментальные эмуляторы в приложение SPMP-OS без конфликтов их C-runtime, загрузчика и памяти.
5. Реализовать ранее запланированный двухпанельный файловый менеджер, экранную клавиатуру/редактор текста и Settings поверх настоящего файлового HAL.

Отдельно: поддержка Sega CD/32X в дереве PicoDrive не удалена. Их функциональность на Defender ещё не проверена. PSX4ALL и SNES также не следует объявлять аппаратно работающими без испытаний.
