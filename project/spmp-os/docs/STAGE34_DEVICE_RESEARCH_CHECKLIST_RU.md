# Аппаратная инвентаризация Defender MultiMix Magic — НЕ инструкция по прошивке

Заполняется только по реальному устройству, а не по документации от JXD/Letcool:

| Параметр | Статус | Источник |
|---|---|---|
| Маркировка CPU на плате | неизвестно | фото платы/маркировка |
| Ревизия PCB | неизвестно | фото |
| Объём/чип DRAM | неизвестно | маркировка/документация |
| Хранилище и заводская разметка | неизвестно | read-only backup |
| Подтверждённая свободная область RAM | **нет** | данные штатной ОС |
| Размер стека задачи/OS memory ownership | **нет** | eCos/FSP исследование |
| Взаимодействие native launcher с возвратом | **нет** | код, сначала host анализ |
| Службы cache clean/invalidate | **нет** | код ARM/eCos, ревизия CPU |
| Путь к FAT (`fatfs_*`) | предварительные строки | оригинальный FSP бинарник |
| LCD RGB565 entrypoint | **нет** | native HAL SDK/firmware |
| Звук PCM и частота | **нет** | native HAL SDK/firmware |
| UART pins / TTL напряжение | **нет** | измерение, документация |

Чтение UART: сначала определить **уровень сигналов** и GND. Логировать загрузку пассивно. Не использовать ISP/Flash commands, `fis`, `fconfig -i`, `flash` и любые `iopoke`, не выдавать тестовый ELF за загрузочный образ.

Для построения RAM bridge нужны именно **положительно подтверждённые** callback'и `confirm_slot_owned`, `map_slot`, `clean_dcache`, `invalidate_icache`. Ни одно из них нельзя заменить константой `return 1` в рабочей сборке.

Сторонние сведения: eCos RedBoot RAM boot различается от платы к плате (https://doc.ecoscentric.com/ref/arm-kb9200-setup.html). Открытый проект SPMP8000Emu документирует HLE NativeGE/emuIf/eCos и формат NGame1.0 (https://github.com/AloysHF/SPMP8000Emu). Ни одно из этих двух описаний не даёт проверенной карты RAM именно Defender MultiMix Magic.
