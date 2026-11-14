# Stage57: ABI CEVA, LCD/scaler и владение памятью

Этап продолжает Stage56 и проверяет exact-build `8000_MMI.RAP`. Оригинальный
RAP в репозиторий не входит, MMIO не выполняется.

```bash
SPMP_MMI_RAP=/private/8000_MMI.RAP \
  python3 -m unittest discover -s tests -v
python3 tools/audit_ceva_abi.py \
  --rap /private/8000_MMI.RAP \
  --output evidence/ceva_abi_exact.json
```

Без приватного RAP выполняются тесты границ входящего PIU-пакета. С RAP
проверяются SHA-256, 15 ARM callsites и 13 точных слов/литералов. Код CEVA и
запросы LCD/scaler не запускаются.
