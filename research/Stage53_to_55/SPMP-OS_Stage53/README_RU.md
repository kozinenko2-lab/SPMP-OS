# SPMP-OS Stage 53 — анализ оригинальных Ghidra-проектов

Цель: понять устройство OEM `RedBoot` и `8000_MMI.RAP` для создания самостоятельной SPMP-OS либо безопасной модернизации OEM MMI. **Не является прошивкой.**

Открой `docs/REPORT_RU.md` — подробный русский отчёт с адресами, проверками и планом миграции.

Содержимое:
- `evidence/verified_ghidra_and_boot.json` — проверенные заголовки трёх GZF, CRC и машинные инструкции OEM;
- `evidence/verified_boot_flow_disasm.asm` — листинги приоритетных мест OEM;
- `evidence/ghidra_analysis_ONLY*.csv` — автоматические метки `FUN_...` из GZF, **не проверенные границы функций**;
- `ghidra_scripts/SPMP_ExportArchitecture.java` — экспорт полной таблицы функций/callgraph и приоритетного псевдокода в локальном Ghidra;
- `tools/audit_ghidra_and_boot.py`, `tools/export_symbol_index.py` — повторная независимая проверка GZF (Python3);
- `tests/test_stage53_audit.py` — тесты.

В архиве **нет** копий пользовательских GZF и OEM бинарников: они уже есть у пользователя. Для воспроизведения запусти:

```sh
unzip -q SPMP-OS_Stage52_Ghidra_Analysis_Ready.zip -d /tmp/stage52
python3 tools/audit_ghidra_and_boot.py \
  --ghidra-zip SPMP8k.zip \
  --stage52 /tmp/stage52 \
  --output evidence/reverified.json
```

Формат входного MMI `.gzf`: Ghidra packed Program, не бинарник, не скрипт.

Источник для общей архитектуры native ABI: https://github.com/uli/libspmp8k (не OEM SDK и не доказательство аппаратной совместимости конкретной платы).
