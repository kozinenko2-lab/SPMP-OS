# Stage 54 — карта потоков MMI на основе ARM и Ghidra

Получены 35 прямых вызовов wrapper создания потоков `0x00339AAC`, 34 имени и точки входа. Полный отчёт: **REPORT_RU.md**.

- `evidence/thread_creation_35.csv` — таблица адресов с проверяемым происхождением каждого имени/точки входа.
- `evidence/thread_creation_35.json` — детальная машинно-читаемая карта.
- `evidence/ghidra_priority_entries.csv` — адреса для навигации и переименования в Ghidra.
- `evidence/ghidra_entry_prologue_audit.json` — 35 подтверждённых ARM-прологов, сравнение с автоматическими именами Ghidra;
- `evidence/boot_chain_confirmed.dot` — граф раннего запуска с различением прямых BL и передачи указателя.
- `tools/trace_mmi_threads.py` — независимый Python3 анализатор OEM RAP; ожидает исходный SHA-256 и не меняет входной файл.
- `tests/test_stage54.py` — 17 регрессионных проверок, включая отказ при изменённом образе.
- `ghidra_scripts/SPMP_InspectThreadEntries.java` — только просмотр точек входа в Ghidra; необязательное переименование выключено по умолчанию.

**Не установщик и не прошивка.** RedBoot и `8000_MMI.RAP` не изменены. Входной RAP не вложен в архив: пользователь уже предоставил его отдельно.
