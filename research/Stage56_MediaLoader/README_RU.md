# Stage56: штатный загрузчик мультимедийных модулей

Этот каталог фиксирует статически восстановленную цепочку MMI → CEVA для
оригинального `8000_MMI.RAP`. Оригинальный RAP и файлы `IMAGE/*.IMG` сюда не
включаются.

Повторный локальный аудит:

```bash
python3 tools/audit_media_loader.py \
  --rap /private/8000_MMI.RAP \
  --image-root /private/IMAGE \
  --output evidence/media_loader_exact.json
python3 -m unittest discover -s tests -v
```

Без `--image-root` проверяются exact-SHA RAP, 28 ARM callsites, таблица путей,
JPEG-тройка, переходы PIU IRQ/reply и все 256 слотов таблицы входящих команд.
С каталогом `IMAGE` дополнительно выполняется только структурный
обход всех `MMPd/MMPs`; код модулей не запускается.

`ghidra_scripts/SPMP_ExportPiuDispatcher.java` экспортирует фиксированные
диапазоны PIU/dispatcher из exact-build проекта, не создавая функций и меток.
Проверяемые выдержки сохранены в `evidence/piu_dispatcher_excerpt.asm`, а
сокращённая модель управления — в `evidence/piu_dispatcher_model.c`.
