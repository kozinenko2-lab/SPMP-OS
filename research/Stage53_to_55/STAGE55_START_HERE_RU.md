# SPMP-OS Stage 55 — что находится в архиве

Полный исследовательский комплект Stage 53–55. **Только анализ**; Flash, RedBoot и штатная MMI не изменены.

- `SPMP-OS_Stage53/` — результаты анализа GZF/RedBoot.
- `SPMP-OS_Stage54/` — 35 точек создания потоков.
- `SPMP-OS_Stage55/` — трассировка GUI/eCos, проверенный MMIO `0x90005040`, GhidraScript, тесты.
- `SPMP-OS_Stage55/reference/8000_MMI.RAP` — исходный заводской RAP пользователя для воспроизводимых проверок (НИ В КОЕМ СЛУЧАЕ не устанавливать этот архив как прошивку).

Запуск: `cd SPMP-OS_Stage55 && sh tools/run_stage55.sh reference/8000_MMI.RAP`.

Для экспортирования псевдокода в Ghidra открой сохранённую MMI GZF и запусти `ghidra_scripts/SPMP_Stage55_GUI_DeepExport.java`. Скрипт не редактирует проект.

GZF-файлы здесь не дублируются: они были ранее присланы пользователем в `SPMP8k.zip`.
