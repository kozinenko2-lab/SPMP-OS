# Stage59: команды C0/C1/C2 и ABI MJPEG-кодировщика

Этап завершает идентификацию 0x60-байтной группы команд около
`0x003494d4`. Exact-build проверка связывает её с видеозаписью и тремя IMG:
`mjpgenc_os_ext.img`, `mjpgenc_os_int.img`, `mjpgenc_codec.img`.

- `C1` — `EncodeInit`;
- `C0` — `Encode1Frame`;
- `C2` — закрытие/освобождение кодировщика.

Пакет команды занимает 8 байт: opcode, нулевой байт, `u16le(4)` и 32-битный
указатель на control block. После ответа OEM копирует `0x60` байт из
`pointer | 0x10000000` обратно в cached-адрес. Нулевой ответ mailbox приводит
к бесконечному циклу; цикл повторной инициализации при нулевом поле `+0x58`
тоже не имеет верхней границы. Новая реализация обязана вводить timeout.

Рядом есть другая группа C0/C1/C2 с control block `0x44` и путями JPEG
decoder. Совпадение opcode не делает структуры совместимыми.

```bash
python3 tools/audit_ceva_commands.py --rap /private/8000_MMI.RAP \
  --output evidence/ceva_commands_exact.json
python3 -m unittest discover -s tests -v
```

Auditor только читает exact-build RAP. Он не запускает IMG, CEVA или MMIO.
Скрипт Ghidra экспортирует только явно указанные диапазоны инструкций и не
меняет базу проекта.
