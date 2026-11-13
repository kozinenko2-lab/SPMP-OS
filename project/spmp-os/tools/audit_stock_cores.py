#!/usr/bin/env python3
"""Read-only audit of stock SPMP emulator binaries, never changes IMG/Flash.

Example: python3 tools/audit_stock_cores.py --game-dir /path/IMAGE/GAME --out reports/stock_emulators.json
"""
import argparse
import hashlib
import json
from pathlib import Path

MODULES = {
    'NES': ('N1.BIN',),
    'GB/GBC': ('G1.BIN',),
    'GBA': ('G2.BIN', 'GPSP.BIN', 'G_2C.BIN'),
    'SNES': ('S1.BIN',),
    'Mega Drive': ('M1.BIN',),
}


def audit(root: Path):
    results = []
    for platform, names in MODULES.items():
        for name in names:
            path = root / name
            if not path.is_file():
                results.append({'platform': platform, 'name': name, 'found': False})
                continue
            data = path.read_bytes()
            results.append({
                'platform': platform,
                'name': name,
                'found': True,
                'bytes': len(data),
                'sha256': hashlib.sha256(data).hexdigest(),
                'header_16_hex': data[:16].hex(),
                'gpSP_string_found': b'gpSP' in data,
                'ELF_header': data.startswith(b'\x7fELF'),
                'NGame_header': data.startswith(b'NGame1.0'),
            })
    return results


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--game-dir', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    if not args.game_dir.is_dir():
        p.error(f'not a directory: {args.game_dir}')
    result = audit(args.game_dir)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps({
        'description': 'Stock binary inventory only. No native replacement compiled.',
        'modules': result
    }, indent=2, ensure_ascii=False) + '\n', encoding='utf8')
    for row in result:
        print(f"{row['platform']:12} {row['name']:10} " +
              (f"{row['bytes']:>9} bytes {row['sha256'][:16]}" if row['found'] else 'missing'))
    print(f'Report: {args.out}')

if __name__ == '__main__':
    main()
