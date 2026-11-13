# Локальные входные файлы

Эти пути игнорируются Git и не должны включаться в публичные коммиты:

```text
firmware/originals/8000_MMI.RAP
firmware/originals/Images_RedBoot_201000427.img
ghidra/user_GZF/*.gzf
```

Ожидаемые SHA-256:

- MMI: `5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`
- RedBoot: `bc00123babe27af9af313ec8b19737f2b6fd3c533b901728a4f0112b55c9d108`

Проверка после размещения собственных оригиналов, из корня репозитория:

```sh
python3 research/boot_reconstruction/tools/audit_boot.py . --output research/boot_reconstruction/build/local_audit.json
SPMP_MMI_RAP="$PWD/firmware/originals/8000_MMI.RAP" make -C research/boot_reconstruction test
SPMP_MMI_RAP=firmware/originals/8000_MMI.RAP python3 -m unittest discover -s research/Stage53_to_55/SPMP-OS_Stage55/tests -v
python3 tools/verify_integrity.py --private
```

Ghidra в текущей среде не установлена. Нового экспорта баз или декомпиляции
из них при этом импорте не выполнялось.
