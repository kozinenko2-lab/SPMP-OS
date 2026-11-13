#!/usr/bin/env python3
"""Pack an ARM ELF32LE file for future SD boot experiments; NEVER FLASH this."""
import argparse
import binascii
import struct
from pathlib import Path


def pack(elf: bytes, board: int) -> bytes:
    if board <= 0 or board > 0xFFFFFFFF:
        raise ValueError("board ID must be confirmed, nonzero uint32")
    if not (52 <= len(elf) <= 8 * 1024 * 1024):
        raise ValueError("ELF size exceeds policy limits")
    if elf[:7] != b'\x7fELF\x01\x01\x01' or int.from_bytes(elf[18:20], 'little') != 40:
        raise ValueError("expected little endian 32-bit ARM ELF")
    header = b'SPMPDB01' + struct.pack('<6I', 1, 48, len(elf), binascii.crc32(elf), board, 1) + bytes(16)
    assert len(header) == 48
    return header + elf


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('elf', type=Path)
    p.add_argument('--board-id', type=lambda n: int(n, 0), required=True,
                   help='only after verifying real board profile; not guessed')
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    a.output.write_bytes(pack(a.elf.read_bytes(), a.board_id))
    print('Prepared for offline verification only:', a.output)
    print('Not a RedBoot / PRAW / NAND flashing image.')


if __name__ == '__main__':
    main()
