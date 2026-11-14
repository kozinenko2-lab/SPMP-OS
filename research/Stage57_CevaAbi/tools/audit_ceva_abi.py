#!/usr/bin/env python3
"""Exact-build, read-only audit of the OEM CEVA/ARM ownership ABI."""
import argparse
import importlib.util
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STAGE56 = ROOT / "Stage56_MediaLoader" / "tools" / "audit_media_loader.py"
SPEC = importlib.util.spec_from_file_location("stage56_audit", STAGE56)
S56 = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(S56)

CALLS = {
    "ready_poll_delay": (0x00348E10, 0x00339BE8),
    "ready_timeout_reset": (0x00348E4C, 0x00348D70),
    "ready_timeout_dump": (0x00348E5C, 0x00348D00),
    "boot_poll_delay": (0x00348EE0, 0x00339BE8),
    "boot_timeout_reset": (0x00348F34, 0x00348D70),
    "lcd_clear_to_init": (0x00349990, 0x002A3E5C),
    "scale_wait_idle": (0x00349BE0, 0x002E66C0),
    "scale_execute_before_reply": (0x00349C24, 0x003497C0),
    "scale_execute_after_reply": (0x00349C9C, 0x003497C0),
    "ceva_malloc": (0x00349D00, 0x00299588),
    "ceva_free": (0x00349D78, 0x002995D0),
    "response_channel_prepare": (0x00349904, 0x003497B0),
    "incoming_copy_header": (0x0034A274, 0x00298C6C),
    "incoming_copy_payload": (0x0034A2A0, 0x00298C6C),
    "scale_descriptor_submit": (0x003498E4, 0x002E6958),
}

WORDS = {
    "ceva_ready_magic": (0x00348E84, 0x43585244),
    "ceva_boot_magic": (0x00348F58, 0x43584254),
    "boot_state_register": (0x00348F60, 0x90532044),
    "arm_command_register": (0x0034A118, 0x90532030),
    "arm_response_register": (0x00349970, 0x90532034),
    "ceva_reply_register": (0x0034A220, 0x9053203C),
    "ceva_command_register_final_add_instruction": (0x0034A254, 0xE2833D81),
    "incoming_buffer": (0x0034A3E0, 0x00640C60),
    "next_rtos_object": (0x0034A528, 0x00640D60),
    "general_heap_control_malloc": (0x002995C8, 0x00514250),
    "general_heap_control_free": (0x00299608, 0x00514250),
    "debug_flags_address": (0x00349EE8, 0x0056A92C),
    "debug_flags_initial_value": (0x0056A92C, 0x03008000),
}


def validate_request(data: bytes, capacity: int = 256) -> dict:
    if capacity < 4 or len(data) < 4:
        raise ValueError("truncated CEVA request header")
    payload = struct.unpack_from("<H", data, 2)[0]
    total = 4 + payload
    if total > capacity:
        raise ValueError("CEVA request exceeds the fixed ARM buffer")
    if total > len(data):
        raise ValueError("truncated CEVA request payload")
    if data[0] < 0x40 or data[0] > 0x46:
        raise ValueError("unsupported CEVA-to-ARM opcode")
    return {"opcode": data[0], "no_reply": data[1] != 0, "payload_bytes": payload, "total_bytes": total}


def audit(rap: Path) -> dict:
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
    return {
        "research_only": True,
        "source_rap_sha256": fw.sha256,
        "verified_calls": calls,
        "verified_words": words,
        "ready_states": {
            "ceva_ready": {"register": "0x9053200c", "magic": "0x43585244", "source_literal": "'CXRD'"},
            "codec_booted": {"register": "0x90532044", "magic": "0x43584254", "source_literal": "'CXBT'"},
            "poll_contract": "argument N gives at most N+1 reads; each read is followed by delay(10, 0)",
            "media_loader_argument": 30,
            "media_loader_max_reads": 31,
        },
        "incoming_buffer": {
            "address": "0x00640c60",
            "next_object": "0x00640d60",
            "capacity_bytes": 256,
            "header_bytes": 4,
            "safe_payload_limit": 252,
            "oem_bounds_check": False,
        },
        "request_layout": {
            "opcode_u8": 0,
            "no_reply_u8": 1,
            "payload_bytes_u16_le": 2,
            "payload": 4,
        },
        "lcd_geometry_request": {
            "minimum_total_bytes": 36,
            "width_u32": 4,
            "height_u32": 8,
            "format_or_mode_u32": 32,
            "clamp": "min(requested dimension, active LCD dimension)",
            "reconfigure_order": ["display path 0 disable", "rebuild display path", "apply format/mode", "display path 0 enable"],
        },
        "scale_request": {
            "minimum_total_bytes": 52,
            "requested_width_u32": 4,
            "requested_height_u32": 8,
            "descriptor_u16_at_4_from_request": 20,
            "descriptor_word_18_from_request": 28,
            "descriptor_words_08_0c_10_from_request": [32, 36, 40],
            "descriptor_word_20_from_request": 44,
            "execute_before_reply_u32": 48,
            "internal_descriptor_bytes": 60,
            "internal_mode": 9,
        },
        "memory_ownership": {
            "opcodes": {"0x43": "allocate", "0x44": "free"},
            "allocator": "OEM general heap control pointer at 0x00514250; not a dedicated CEVA pool",
            "alignment_observed": "allocator wrapper diagnoses non-4-byte-aligned non-null results",
            "returned_pointer": "raw ARM pointer is placed in response payload +4",
            "inbound_pointer_use": "CEVA-owned pointers are read by ARM after OR 0x10000000",
        },
        "cache_contract": {
            "outbound": "mask IRQ/FIQ, repeat ARM926 test-and-clean D-cache, drain write buffer, restore prior IRQ/FIQ mask, publish MMIO pointer",
            "inbound": "copy through pointer OR 0x10000000; no explicit invalidate appears in the dispatcher",
            "alias_status": "the alias is consistent with an uncached/coherent view but that MMU mapping remains an inference",
        },
        "safety": [
            "No MMIO or CEVA code was executed.",
            "A replacement must reject payloads above 252 bytes before copying into the 256-byte OEM-sized buffer.",
            "All waits must be bounded; OEM debug flags may turn timeouts into infinite loops.",
            "Do not return arbitrary general-heap pointers to CEVA until alias, cache, lifetime and free ownership are implemented together.",
        ],
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rap", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    result = audit(args.rap)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"PASS: {len(result['verified_calls'])} callsites, {len(result['verified_words'])} words")


if __name__ == "__main__":
    main()
