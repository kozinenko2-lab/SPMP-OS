# Реестр исследованных исходников (сводка)

Входных архивов: 10. Исходники не включены в пакет Stage 3.

| Архив | Объектов | Исходных файлов | Вывод |
|---|---:|---:|---|
| `libspmp8k-master.zip` | 1690 | 866 | ARM926 / eCos host ABI, Doom+z26 embedded |
| `xpectrum-spmp8000-master.zip` | 212 | 89 | ZX Spectrum ARM source; libgame dependency |
| `tgemu-spmp8000-master.zip` | 58 | 48 | PC Engine ARM source; libgame dependency |
| `Frodo4-spmp8000-master.zip` | 187 | 118 | C64 ARM source; libgame dependency |
| `wiituka-spmp8000-master.zip` | 127 | 76 | Amstrad CPC ARM source; libgame dependency |
| `xrick-spmp8000-master.zip` | 86 | 71 | Rick Dangerous source; original assets licensing caveat |
| `scummvm-spmp8000-master.zip` | 4657 | 3859 | ScummVM tree; no confirmed SPMP backend in archive |
| `psx4all-dingoo-master.zip` | 377 | 297 | Dingoo PS1 port; MIPS dynarec |
| `psx4all-dingoo-master-1.zip` | 375 | 297 | Variant of Dingoo PS1 port; compare before reuse |
| `actsemi-master.zip` | 108 | 93 | Actsemi homebrew SDK, MIPS32 incompatible with SPMP ARM |

Отдельные варианты psx4all-Dingoo не идентичны по содержимому. Оба ориентированы на архитектуру MIPS, не ARM926.

Ограничение: анализ сделан по предоставленным ZIP без компиляции всех исходников. Проверка запуска на физической консоли отсутствует.

Детали, включая сравнение ABI и GPL, приведены в `docs/NATIVE_PORTS_ROADMAP_RU.md`.
