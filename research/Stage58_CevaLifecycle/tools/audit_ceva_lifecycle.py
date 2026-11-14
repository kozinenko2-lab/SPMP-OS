#!/usr/bin/env python3
"""Read-only exact-build audit of RedBoot MMU tables and the OEM CEVA lifecycle."""
import argparse
import hashlib
import importlib.util
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STAGE56 = ROOT / "Stage56_MediaLoader" / "tools" / "audit_media_loader.py"
SPEC = importlib.util.spec_from_file_location("stage56_audit", STAGE56)
S56 = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(S56)

TABLE_SHA256 = "80a4560940f1313613a249ed01bce202961ad6ceeaa9809f3aa034f6d334e45b"
TABLE_BYTES = 0x4400
CODE_FAMILIES = {
    "b2790a6a8c4b9a3217e7bfa3430693eaae4b28f0a15e6fd232fd9c71e2fcc4c2": {
        "family": "redboot_1_2_4", "load": 0x00030000, "install": 0x0003D1E4,
    },
    "c8f6d2799eeda3e5560322aa4ca3c61911028bf53532264da0d7b5af4bcb5d3c": {
        "family": "redboot_3_5", "load": 0x00030000, "install": 0x0003B4D8,
    },
}

MMU_INSTALL_WORDS = (
    0xE59F3024, 0xEE110F10, 0xE3C00001, 0xEE010F10, 0xE3A00000,
    0xEE070F17, 0xEE080F17, 0xEE023F10, 0xE3A00001, 0xEE030F10,
    0xE12FFF1E, 0x00060000,
)

CALLS = {
    "start_ceva_core": (0x00349298, 0x003488F8),
    "normal_stop_close": (0x00349364, 0x00348F70),
    "normal_stop_power_1": (0x00349384, 0x002A02D0),
    "normal_stop_power_2": (0x00349394, 0x002A02D0),
    "force_stop_close": (0x00349464, 0x00348F70),
    "force_stop_power_1": (0x00349478, 0x002A02D0),
    "force_stop_power_2": (0x00349488, 0x002A02D0),
    "close_mailbox": (0x00348FFC, 0x0034A034),
    "media_loader_normal_stop": (0x0034B634, 0x00349300),
    "media_normal_stop": (0x002DF668, 0x00349300),
    "alternate_normal_stop": (0x0033659C, 0x00349300),
    "command_c0_mailbox": (0x00349510, 0x0034A034),
    "command_c1_mailbox": (0x003495E0, 0x0034A034),
    "command_c2_mailbox": (0x00349674, 0x0034A034),
}

WORDS = {
    "start_refcount_literal": (0x003492F4, 0x0055B7A8),
    "close_refcount_literal": (0x00349058, 0x0055B7A8),
    "normal_stop_refcount_literal": (0x003493F0, 0x0055B7A8),
    "force_stop_refcount_literal": (0x003494C4, 0x0055B7A8),
    "close_opcode_84_instruction": (0x00348F80, 0xE3E0307B),
    "close_active_bit_test_instruction": (0x00348FB8, 0xE3130C01),
    "normal_stop_clear_bit_instruction": (0x00349378, 0xE3C33C01),
    "normal_stop_refcount_decrement": (0x003493A0, 0xE2433001),
    "force_stop_refcount_decrement": (0x00349490, 0xE2433001),
    "c0_uncached_alias_instruction": (0x0034953C, 0xE3851201),
    "c1_uncached_alias_instruction": (0x00349608, 0xE3841201),
    "c2_uncached_alias_instruction": (0x003496A0, 0xE3851201),
}


def _u32(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 4 > len(data):
        raise ValueError(f"word outside input at offset 0x{offset:x}")
    return struct.unpack_from("<I", data, offset)[0]


def decode_section(descriptor: int) -> dict:
    if descriptor & 3 != 2:
        raise ValueError("not an ARMv5 first-level section descriptor")
    return {
        "physical_base": descriptor & 0xFFF00000,
        "ap": (descriptor >> 10) & 3,
        "domain": (descriptor >> 5) & 15,
        "cacheable": bool(descriptor & 8),
        "bufferable": bool(descriptor & 4),
    }


def validate_page_table(data: bytes) -> dict:
    digest = hashlib.sha256(data).hexdigest()
    if len(data) != TABLE_BYTES or digest != TABLE_SHA256:
        raise ValueError(f"unexpected RedBoot table identity: {len(data)} bytes, {digest}")
    expected = {
        0x000: 0x00064011, 0x001: 0x00100C1E, 0x0FF: 0x0FF00C1E,
        0x100: 0x00000C12, 0x1FF: 0x0FF00C12,
        0x2E0: 0x2E000C12, 0x2FF: 0x2FF00C12,
        0x900: 0x90000C12, 0x9FF: 0x9FF00C12,
        0xD00: 0xD0000C1E, 0xFFF: 0xFFF00C1E,
    }
    for index, value in expected.items():
        actual = _u32(data, index * 4)
        if actual != value:
            raise ValueError(f"L1[{index:#x}]={actual:#010x}, expected {value:#010x}")
    for index in range(256):
        group = index // 16
        expected_l2 = (group << 16) | (0xFF1 if group == 0 else 0xFFD)
        actual = _u32(data, 0x4000 + index * 4)
        if actual != expected_l2:
            raise ValueError(f"L2[{index:#x}]={actual:#010x}, expected {expected_l2:#010x}")
    cached = decode_section(_u32(data, 0x001 * 4))
    alias = decode_section(_u32(data, 0x100 * 4))
    return {
        "sha256": digest,
        "bytes": len(data),
        "ttbr_physical": "0x00060000",
        "coarse_l2_physical": "0x00064000",
        "ranges": [
            {"virtual": "0x00000000-0x000fffff", "physical": "0x00000000-0x000fffff", "kind": "coarse_l2", "cache": "first 64 KiB uncached; remainder cached+buffered"},
            {"virtual": "0x00100000-0x0fffffff", "physical": "identity", "kind": "section", "cache": "cached+buffered"},
            {"virtual": "0x10000000-0x1fffffff", "physical": "0x00000000-0x0fffffff", "kind": "section", "cache": "uncached+unbuffered"},
            {"virtual": "0x2e000000-0x2fffffff", "physical": "identity", "kind": "section", "cache": "uncached+unbuffered"},
            {"virtual": "0x90000000-0x9fffffff", "physical": "identity", "kind": "section", "cache": "uncached+unbuffered"},
            {"virtual": "0xd0000000-0xffffffff", "physical": "identity", "kind": "section", "cache": "cached+buffered"},
        ],
        "cached_identity_descriptor": cached,
        "uncached_alias_descriptor": alias,
        "alias_rule": "for physical RAM below 0x10000000, VA = PA | 0x10000000 selects the uncached, unbuffered mapping",
    }


def validate_redboot_code(data: bytes) -> dict:
    digest = hashlib.sha256(data).hexdigest()
    family = CODE_FAMILIES.get(digest)
    if family is None:
        raise ValueError(f"unexpected RedBoot code-segment SHA-256: {digest}")
    offset = family["install"] - family["load"]
    actual = tuple(_u32(data, offset + i * 4) for i in range(len(MMU_INSTALL_WORDS)))
    if actual != MMU_INSTALL_WORDS:
        raise ValueError(f"MMU install sequence mismatch for {family['family']}")
    return {
        "sha256": digest, "family": family["family"],
        "install_address": f"0x{family['install']:08x}",
        "ttbr_literal_address": f"0x{family['install'] + 0x2c:08x}",
        "ttbr_value": "0x00060000",
        "operations": ["disable MMU", "invalidate unified cache", "invalidate unified TLB", "write TTBR", "set DACR to 1"],
    }


def audit(rap: Path, code_segments: list[Path], table_segments: list[Path]) -> dict:
    fw = S56.Firmware(rap.read_bytes())
    calls = {}
    for name, (site, expected) in CALLS.items():
        actual = fw.bl_target(site)
        if actual != expected:
            raise ValueError(f"BL mismatch at 0x{site:08x}: {actual!r} != 0x{expected:08x}")
        calls[name] = {"callsite": f"0x{site:08x}", "target": f"0x{actual:08x}"}
    words = {}
    for name, (address, expected) in WORDS.items():
        actual = fw.u32(address)
        if actual != expected:
            raise ValueError(f"word mismatch at 0x{address:08x}: 0x{actual:08x} != 0x{expected:08x}")
        words[name] = {"address": f"0x{address:08x}", "value": f"0x{actual:08x}"}
    code = [validate_redboot_code(path.read_bytes()) for path in code_segments]
    table_details = [validate_page_table(path.read_bytes()) for path in table_segments]
    tables = [{"sha256": row["sha256"], "bytes": row["bytes"]} for row in table_details]
    return {
        "research_only": True,
        "source_rap_sha256": fw.sha256,
        "verified_calls": calls,
        "verified_words": words,
        "redboot_code_segments": code,
        "redboot_page_tables": tables,
        "redboot_page_table_mapping": table_details[0],
        "ceva_lifecycle": {
            "reference_counter": "u32 at 0x0055b7a8",
            "start": "only the zero-to-one transition initializes PIU and starts CEVA; repeated start returns without incrementing in this wrapper",
            "normal_stop": "when the counter is one, send opcode 0x84, clear 0x100 in 0x90530468, run two power/clock calls, then decrement",
            "force_stop": "decrement repeatedly to zero; close and power down only on the one-to-zero transition",
            "transport_lifetime": "the recovered stop paths do not destroy the once-created PIU thread/IRQ objects",
            "response_copy": "C0/C1/C2 replies copy 0x60 bytes through pointer | 0x10000000",
        },
        "new_provider_policy": [
            "PIU transport lifetime and codec-core session lifetime must remain separate.",
            "Reject new allocations after BEGIN_STOP.",
            "Release only pointers tracked for this session; keep failed releases tracked so cleanup is retryable.",
            "Do not complete STOPPING until every tracked allocation has been released.",
            "Every wait and mailbox exchange must have a finite timeout.",
        ],
        "limits": [
            "No MMIO, CEVA module or firmware code was executed.",
            "The supplied five RedBoot images are candidates; this audit does not identify the user's board variant.",
            "The mapping proves the alias for the covered physical range, not for arbitrary pointers outside it.",
        ],
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rap", required=True, type=Path)
    parser.add_argument("--redboot-code", required=True, action="append", type=Path)
    parser.add_argument("--redboot-table", required=True, action="append", type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    result = audit(args.rap, args.redboot_code, args.redboot_table)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"PASS: {len(result['verified_calls'])} callsites, {len(result['verified_words'])} words, "
          f"{len(result['redboot_code_segments'])} code segments, {len(result['redboot_page_tables'])} page tables")


if __name__ == "__main__":
    main()
