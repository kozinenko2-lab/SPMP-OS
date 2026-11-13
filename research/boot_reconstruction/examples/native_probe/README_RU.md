# Минимальный прототип экрана, ввода и возврата

Статус: **HOST_TESTED**, сборка ARM — **ARM_OBJECT_ONLY**. Код рисует три
RGB565-полосы и курсор, опрашивает ввод по шагам и запрашивает обычный возврат.
Он не содержит target-entry, привязки числовых адресов OEM, linker image или
контейнера NGame. Аппаратного запуска нет.

![Результат host-прогона, 320×240](preview.png)

`preview.png` получен из `build/native_probe_frame.ppm`, который рисует host-код
этого примера. Это диагностический шаблон, не снимок консоли или готовый UI SPMP-OS.

## Файлы и контракты

- `probe.c/.h` — переносимое ядро без libc, файлов, собственного потока и звука.
- `host.c` — два массива пикселей, копирование при present, scripted input,
  ограничение шагов и проверки нормального/ошибочного завершения.
- `oem_adapter.c/.h` — адаптер к типизированным функциям InitGraph, display
  origin, double buffer, dimensions, FG, FillRect, Paint и getKeys.
- `adapter_test.c` — проверка адаптера с **host-функциями**, имеющими OEM-сигнатуры.

Адаптер получает таблицу уже типизированных функций от вызывающего. Resolver
85 числовых слотов не предоставляется: устанавливать живые указатели можно
после проверки образа, ABI и условий запуска. Он проверяет полноту переданной
таблицы, затем вызывает InitGraph → SetDisplayOrigin(0,0) → EnableDoubleBuffer(1).
В VM эта последовательность настройки проверена отдельно от host-приложения.

Кадр перерисовывается на одном `probe_step`. Возврат по quit-mask или max_steps
восстанавливает FG и вызывает finish один раз. В OEM-адаптере finish не вызывает
gameExit, не меняет SP и не обещает восстановить GUI/завершить RTOS-worker:
это остаётся обязанностью проверенного обычного пути возврата приложения.

Стандартная конфигурация примера назначает 1/2/4/8 движению, 0x20000 — выходу.
Это выбранное назначение масок. Физические кнопки Defender ещё не сопоставлены
трассировкой GPIO. У оригинального getKeys есть системные побочные действия,
описанные на этапе 4; host-script их не воспроизводит.

Ошибка begin требует очистки частичной инициализации самим backend. Адаптер не
сертифицирует возможность отката произвольной ошибки OEM InitGraph. FillRect
OEM скрывает timeout GPU, поэтому успешный status ещё не удостоверяет завершение
рисования. Эти ограничения запрещают считать тесты готовым HAL для устройства.

## Сборка

Из корня репозитория:

```sh
make -C research/boot_reconstruction test
research/boot_reconstruction/build/native_probe_host \
  --frame research/boot_reconstruction/build/native_probe_frame.ppm

# clang + llvm-nm + llvm-readelf; создаются только два ET_REL object.
make -C research/boot_reconstruction arm-probe
```

ARM: little-endian, ARMv5TE, EABI5, soft-float. Проверяются ELF32/ARM/ET_REL/EABI5,
отсутствие hard-float flag и undefined symbols в обоих object; это не связывание полноценной программы и не проверка
вызовов на ARM. Пример раскладки памяти:
[`../native_memory_profile.json`](../native_memory_profile.json).
Его размеры payload/BSS/heap заданы для иллюстрации и не измерены по этим object.
CI сохраняет object и host PPM в artifact `native-probe-armv5-objects` на 14 дней.

Для device-варианта ещё нужны рабочий OEM BIN как контрольный образец,
startup/BSS/linking, проверка image identity при binding, cache/IRQ ABI,
живые границы allocation/стека и подтверждённое завершение штатных потоков.
