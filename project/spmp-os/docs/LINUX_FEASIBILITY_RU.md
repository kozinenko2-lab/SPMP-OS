# Linux на SPMP8000: исследовательская ветка, без подготовки прошивки

Существует серия из девяти патчей Zoltan Devai для ARM Linux, опубликованная осенью 2011 года. Например, патч «ARM: SPMP8000: Add machine base files»: https://lists.infradead.org/pipermail/linux-arm-kernel/2011-October/068709.html . В нём описаны общие файлы mach-spmp8000, timer, pinmux, clock, ADC, PWM и плата Letcool. Это не готовая сборка для Defender и не свидетельство о наличии полностью работающих драйверов экрана, SD и аудио.

Встречается конфигурация ARMv5/ARM926EJ-S (с MMU) для семейства SPMP8000. Для данного экземпляра Defender необходимо подтвердить конкретный тип процессора и регистры, а не принимать из другого устройства. Если это действительно ARMv5TEJ, современные ARMv7 hard-float бинарники PortMaster не совместимы: нужен ARM EABI soft-float и отдельная пересборка.

## Минимальная цепочка доказательства

* Проверить CPU ID через безопасно загруженный ARM RAM-probe или подтверждённые технические данные.
* Получить serial boot log и согласовать адреса ядра, ATAG/DTB, параметры RAM, MMU и machine id.
* Поднять *только* ранний UART printk/earlycon без замены RedBoot.
* После подтверждённого старта ядра создать initramfs с BusyBox; затем отдельными этапами LCD, input, MMC/FAT и audio.

Не включать Linux в флеш-образ без независимой и проверенной схемы отката. Linux — исследование драйверов и предельных возможностей; основное изделие — DefenderOS.

Дополнительные материалы:
- https://github.com/AloysHF/SPMP8000Emu — HLE-эмуляция игр, не RedBoot-системы.
- https://github.com/uli/tgemu-spmp8000 — пример переноса эмулятора PC Engine под eCos SPMP8000.
- https://www.emulinks.de/spmp8k/docs/html/index.html — сведения о libgame / libspmp8k.
