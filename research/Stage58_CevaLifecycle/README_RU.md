# Stage58: RedBoot MMU alias и жизненный цикл CEVA

Этап проверяет две семьи кода RedBoot, общую таблицу MMU и exact-build
`8000_MMI.RAP`. Оригинальные бинарники в репозиторий не входят; firmware-код,
CEVA и MMIO не исполняются.

```bash
python3 tools/audit_ceva_lifecycle.py \
  --rap /private/8000_MMI.RAP \
  --redboot-code /private/redboot_family_1_segment_00030000.bin \
  --redboot-code /private/redboot_family_2_segment_00030000.bin \
  --redboot-table /private/segment_00060000.bin \
  --output evidence/ceva_lifecycle_exact.json
python3 -m unittest discover -s tests -v
```

Auditor принимает повторяющиеся `--redboot-code` и `--redboot-table`: в
локальном exact-аудите ему переданы все пять вариантов. Без приватных файлов
публичные тесты проверяют декодирование ARMv5 section descriptor и fixture
последовательности установки TTBR.

`ghidra_scripts/SPMP_ScanMmuAlias.java` — read-only поиск констант alias и
CP15-инструкций. Он не создаёт функций, меток и ссылок в проекте Ghidra.
