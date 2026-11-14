#!/usr/bin/env python3
"""Read-only, exact-build audit of the OEM media/CEVA loader and MMPi files.

The tool never executes an IMG, writes target memory, or modifies the RAP.
Private inputs are optional and remain outside the repository; the JSON output
contains paths, hashes, container metadata and verified ARM call relationships.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

RAP_SHA256 = "5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e"
SEGMENT_BASE = 0x00280000
SEGMENT_OFFSET = 0x4C
SEGMENT_BYTES = 3059088

PATH_TABLES = {
    "mpeg4_hd": 0x0031BB38,
    "mpeg4_asp": 0x0031BB50,
    "realvideo_8": 0x0031BB6C,
    "realvideo_9": 0x0031BB84,
    "mjpeg": 0x0031BB9C,
    "h264": 0x0031BBB4,
    "h263": 0x0031BBCC,
    "vc1": 0x0031BBF0,
    "mpeg2": 0x0031BC08,
    "divx3": 0x0031BC20,
    "wmv78": 0x0031BC38,
}

MEDIA_TYPE_MAP = {
    "1": "mpeg4_asp when width<721, otherwise mpeg4_hd",
    "2": "mpeg4_asp",
    "3": "mjpeg",
    "4,5": "h263 or built-in ARM callbacks when available",
    "6": "h264",
    "9,10": "wmv78",
    "11,12": "vc1",
    "13,14": "mpeg2",
    "17": "realvideo_8",
    "18": "realvideo_9",
    "19": "divx3",
}

INCOMING_COMMANDS = {
    0x40: (0x003499F8, "lcd_geometry"),
    0x41: (0x00349B7C, "scale_engine"),
    0x42: (0x00349974, "lcd_clear"),
    0x43: (0x00349CC8, "allocate_arm_memory"),
    0x44: (0x00349D34, "free_arm_memory"),
    0x45: (0x00349DB8, "debug_message"),
    0x46: (0x00349E3C, "read_debug_flags"),
}

CALLS = {
    "media_start_to_video_selector": (0x002E193C, 0x0031B588),
    "selector_to_read_triplet": (0x0031BA64, 0x0034B1A8),
    "selector_to_load_triplet": (0x0031BA98, 0x0034B400),
    "parse_os_ext": (0x0034B454, 0x0034A7B8),
    "start_ceva": (0x0034B494, 0x003491C8),
    "wait_ceva_ready": (0x0034B49C, 0x00348DC8),
    "parse_os_int_mode_0x10": (0x0034B4D0, 0x0034A7B8),
    "send_codec_stage_command": (0x0034B4E8, 0x0034884C),
    "wait_codec_ready": (0x0034B4F0, 0x00348E94),
    "parse_codec_mode_0x10": (0x0034B56C, 0x0034A7B8),
    "jpeg_to_read_triplet": (0x00323434, 0x0034B1A8),
    "jpeg_to_load_triplet": (0x00323444, 0x0034B400),
    "mmpi_to_ceva_segment_writer": (0x0034A6DC, 0x003486B8),
    "segment_writer_to_mailbox": (0x003487C0, 0x0034A034),
    "ceva_start_wrapper_to_entry": (0x00349298, 0x003488F8),
    "piu_reply_wait_to_timed_flag": (0x0034A170, 0x00339F98),
    "piu_reply_wait_to_ack": (0x0034A1DC, 0x00349F28),
    "piu_init_clear_interrupt_mask": (0x0034A424, 0x00349F0C),
    "piu_init_ack_pending": (0x0034A42C, 0x00349F28),
    "piu_init_set_interrupt_mask": (0x0034A43C, 0x00349EF0),
    "piu_init_event_flag": (0x0034A460, 0x00339F10),
    "piu_init_create_interrupt": (0x0034A484, 0x00339C10),
    "piu_init_create_dispatcher_thread": (0x0034A4D0, 0x00339AAC),
    "piu_isr_read_status": (0x00349F54, 0x00349F38),
    "piu_isr_ack_reply": (0x00349F7C, 0x00349F28),
    "piu_isr_ack_command": (0x00349FA4, 0x00349F28),
    "piu_dsr_signal_reply": (0x00349FE0, 0x00339F3C),
    "piu_dsr_wake_dispatcher": (0x0034A00C, 0x00339EFC),
}


class Firmware:
    def __init__(self, data: bytes):
        digest = hashlib.sha256(data).hexdigest()
        if digest != RAP_SHA256:
            raise ValueError(f"unexpected RAP SHA256: {digest}")
        if data[:4] != b"MMPi" or data[0x34:0x38] != b"MMPs":
            raise ValueError("unexpected OEM RAP/MMP layout")
        self.sha256 = digest
        self.segment = data[SEGMENT_OFFSET:SEGMENT_OFFSET + SEGMENT_BYTES]
        if len(self.segment) != SEGMENT_BYTES:
            raise ValueError("truncated first MMI segment")

    def u32(self, address: int) -> int:
        offset = address - SEGMENT_BASE
        if address & 3 or offset < 0 or offset + 4 > len(self.segment):
            raise ValueError(f"address outside aligned segment: 0x{address:08x}")
        return struct.unpack_from("<I", self.segment, offset)[0]

    def cstring(self, address: int, limit: int = 256) -> str:
        offset = address - SEGMENT_BASE
        if offset < 0 or offset >= len(self.segment):
            raise ValueError(f"string outside segment: 0x{address:08x}")
        raw = self.segment[offset:offset + limit]
        end = raw.find(b"\0")
        if end < 0:
            raise ValueError(f"unterminated string: 0x{address:08x}")
        raw = raw[:end]
        if any(ch < 32 or ch > 126 for ch in raw):
            raise ValueError(f"non-ASCII string: 0x{address:08x}")
        return raw.decode("ascii")

    def pointer_string(self, pointer_address: int) -> str:
        return self.cstring(self.u32(pointer_address))

    def bl_target(self, address: int) -> int | None:
        word = self.u32(address)
        if ((word >> 24) & 0xF) != 0xB or ((word >> 28) & 0xF) == 0xF:
            return None
        displacement = word & 0xFFFFFF
        if displacement & 0x800000:
            displacement -= 0x1000000
        return (address + 8 + (displacement << 2)) & 0xFFFFFFFF


def parse_mmpi(data: bytes) -> dict:
    if len(data) < 52 or data[:4] != b"MMPi":
        raise ValueError("missing MMPi header")
    declared = struct.unpack_from("<I", data, 4)[0]
    if declared != len(data) or (len(data) - 16) & 3:
        raise ValueError("invalid MMPi size/alignment")
    at = 16
    descriptors = []
    total_segments = 0
    total_payload = 0
    selected_entry = None
    while at < len(data):
        if len(data) - at < 36 or data[at:at + 4] != b"MMPd":
            raise ValueError(f"invalid MMPd at 0x{at:x}")
        record_size, version, reserved, flags, count, entry, aux0, aux1 = struct.unpack_from(
            "<8I", data, at + 4
        )
        if record_size != 36:
            raise ValueError(f"unsupported MMPd size at 0x{at:x}")
        descriptor_offset = at
        at += 36
        mode = (flags >> 16) & 3
        if (flags & 0x000F0000) == 0 and entry != 0xFFFFFFFF:
            selected_entry = entry
        destinations = []
        descriptor_payload = 0
        if count > (len(data) - at) // 24:
            raise ValueError(f"impossible segment count at 0x{descriptor_offset:x}")
        for _ in range(count):
            if len(data) - at < 24 or data[at:at + 4] != b"MMPs":
                raise ValueError(f"invalid MMPs at 0x{at:x}")
            segment_bytes = struct.unpack_from("<I", data, at + 4)[0]
            destination = struct.unpack_from("<I", data, at + 8)[0]
            if segment_bytes < 24 or segment_bytes & 3 or segment_bytes > len(data) - at:
                raise ValueError(f"invalid MMPs size at 0x{at:x}")
            destinations.append(destination)
            descriptor_payload += segment_bytes - 24
            at += segment_bytes
        total_segments += count
        total_payload += descriptor_payload
        descriptors.append({
            "offset": f"0x{descriptor_offset:08x}",
            "version": f"0x{version:08x}",
            "reserved": f"0x{reserved:08x}",
            "flags": f"0x{flags:08x}",
            "mode": mode,
            "segment_count": count,
            "entry": f"0x{entry:08x}",
            "aux": [f"0x{aux0:08x}", f"0x{aux1:08x}"],
            "payload_bytes": descriptor_payload,
            "destination_min": f"0x{min(destinations):08x}" if destinations else None,
            "destination_max": f"0x{max(destinations):08x}" if destinations else None,
        })
    if at != len(data) or not descriptors:
        raise ValueError("MMPi traversal did not end at file boundary")
    return {
        "bytes": len(data),
        "checksum_words_unverified": [
            f"0x{struct.unpack_from('<I', data, 8)[0]:08x}",
            f"0x{struct.unpack_from('<I', data, 12)[0]:08x}",
        ],
        "descriptor_count": len(descriptors),
        "segment_count": total_segments,
        "payload_bytes": total_payload,
        "selected_entry": f"0x{selected_entry:08x}" if selected_entry is not None else None,
        "descriptors": descriptors,
    }


def extract_path_map(fw: Firmware) -> dict:
    result = {}
    for family, table in PATH_TABLES.items():
        values = [fw.pointer_string(table + i * 4) for i in range(6)]
        result[family] = {
            "rom": {"os_int": values[0], "os_ext": values[1], "codec": values[2]},
            "fallback": {"os_int": values[3], "os_ext": values[4], "codec": values[5]},
        }
    return result


def inspect_images(root: Path) -> dict:
    candidates = list(root.glob("*.IMG"))
    game = root / "GAME"
    if game.is_dir():
        candidates.extend(game.glob("KGBVIDEO*.IMG"))
    result = {}
    for path in sorted(candidates):
        data = path.read_bytes()
        row = parse_mmpi(data)
        row["sha256"] = hashlib.sha256(data).hexdigest()
        result[path.relative_to(root).as_posix()] = row
    return result


def extract_incoming_commands(fw: Firmware) -> dict:
    table = 0x0055B7E0
    expected_slots = set(INCOMING_COMMANDS)
    for opcode in range(256):
        value = fw.u32(table + opcode * 4)
        if opcode not in expected_slots and value != 0:
            raise ValueError(f"unexpected PIU callback at opcode 0x{opcode:02x}: 0x{value:08x}")
    result = {}
    for opcode, (handler, role) in INCOMING_COMMANDS.items():
        actual = fw.u32(table + opcode * 4)
        if actual != handler:
            raise ValueError(
                f"PIU callback mismatch at opcode 0x{opcode:02x}: "
                f"0x{actual:08x} != 0x{handler:08x}"
            )
        result[f"0x{opcode:02x}"] = {"handler": f"0x{handler:08x}", "inferred_role": role}
    return result


def audit(rap: Path, image_root: Path | None = None) -> dict:
    fw = Firmware(rap.read_bytes())
    verified_calls = {}
    for name, (site, expected) in CALLS.items():
        actual = fw.bl_target(site)
        if actual != expected:
            raise ValueError(f"BL mismatch at 0x{site:08x}: {actual!r} != 0x{expected:08x}")
        verified_calls[name] = {"callsite": f"0x{site:08x}", "target": f"0x{actual:08x}"}
    result = {
        "research_only": True,
        "source_rap_sha256": fw.sha256,
        "verified_calls": verified_calls,
        "media_type_map": MEDIA_TYPE_MAP,
        "video_triplets": extract_path_map(fw),
        "jpeg_relative_triplet": {
            "os_ext": fw.pointer_string(0x003234A4),
            "os_int": fw.pointer_string(0x003234A8),
            "codec": fw.pointer_string(0x003234AC),
        },
        "loader_sequence": [
            "read OS_INT, OS_EXT and codec files; each fallback path is tried before the ROM path",
            "parse OS_EXT with mode argument 0 and keep its selected entry",
            "start CEVA; argument 30 produces up to 31 reads, each followed by a delay of 10 OEM time units",
            "parse/copy OS_INT with mode argument 0x10",
            "send command bytes 81 01 00 00; the second argument-30 wait likewise performs up to 31 reads",
            "parse/copy the codec image with mode argument 0x10",
            "optionally free the three aligned file buffers when context flag bit 0 is set",
        ],
        "mmpd_contract": {
            "record_bytes": 36,
            "flags_offset": 16,
            "segment_count_offset": 20,
            "entry_offset": 24,
            "entry_accept_condition": "(flags & 0x000f0000) == 0 and entry != 0xffffffff",
            "mode_argument": "(flags >> 16) & 3",
            "segment_record": "MMPs: 24-byte header; +4 total bytes; +8 destination",
        },
        "ceva_command_contract": {
            "segment_write_command": {
                "opcode": "0x90",
                "bytes_1_to_3": "00 10 00 for the observed caller",
                "mode_nonzero_at_offset_4": True,
                "destination_offset": 8,
                "payload_bytes_offset": 12,
                "source_pointer_offset": 16,
            },
            "codec_stage_command": "81 01 00 00",
            "start_register_writes": {
                "0x9053200c": "zeroed; later polled for ready-state transition",
                "0x90532044": "zeroed",
                "0x90532804": "zeroed",
                "0x90532808": "zeroed",
                "0x90530468": "0 -> OR 1 -> 0x101 start sequence",
                "0x9053046c": "selected OS_EXT entry",
            },
            "synchronization": "OEM code masks IRQ/FIQ around a CP15 cache-drain/synchronization sequence before mailbox publication",
        },
        "piu_reply_irq_contract": {
            "registers": {
                "0x90532030": "ARM-to-CEVA command pointer",
                "0x90532034": "ARM response pointer for a CEVA-to-ARM command",
                "0x9053203c": "CEVA reply pointer; OEM returns pointer OR 0x10000000",
                "0x90532040": "CEVA-to-ARM command pointer",
                "0x90532048": "interrupt mask; helper functions atomically set or clear selected bits",
                "0x9053204c": "interrupt status/acknowledge; read for pending bits and written to acknowledge them",
            },
            "interrupt": {
                "vector": 31,
                "priority": 12,
                "reply_status_bit": "0x08",
                "command_status_bit": "0x10",
                "isr": "0x00349f48 reads status, increments per-bit pending counters, acknowledges bits and returns 2",
                "dsr": "0x00349fbc signals reply event bit 1 and wakes the command dispatcher from the pending counters",
            },
            "reply_wait": {
                "function": "0x0034a124",
                "polling_mode": "busy-waits for status bit 0x08 and acknowledges it",
                "rtos_mode": "waits for event bit 1 with an absolute deadline of current time + 0x0bb8 OEM time units",
                "result": "reads the pointer from 0x9053203c and maps it through the 0x10000000 alias",
            },
            "initialization": {
                "function": "0x0034a3f4",
                "mask_and_ack": "clear/ack 0x38, then enable status bit 0x10",
                "dispatcher_entry": "0x0034a2c4",
                "dispatcher_name": "dspCmdDispatcher",
                "dispatcher_priority": 8,
                "dispatcher_stack_bytes": 8192,
            },
            "incoming_request_layout": {
                "opcode_offset": 0,
                "no_reply_offset": 1,
                "payload_bytes_u16_offset": 2,
                "payload_offset": 4,
            },
            "incoming_handlers": extract_incoming_commands(fw),
            "handler_table": "0x0055b7e0; all 256 entries checked, only opcodes 0x40..0x46 are populated",
            "response_path": "handler first writes status/ack bit 0x02, records the response pointer, cleans/drains cache state, then publishes the pointer through 0x90532034; notification semantics are not yet proven",
            "ownership_note": "The original has one synchronous reply path plus a permanent CEVA-to-ARM command dispatcher; buffers and callbacks are shared global state.",
        },
        "safety": [
            "Static exact-build analysis only; no firmware or IMG code was executed.",
            "The OEM codec path passes parser modes 0/0x10, so MMPi checksum words are not authenticated there.",
            "The wait-ready code reads 0x9053200c; no SPMP-OS code should touch this image-specific MMIO before board validation.",
            "Several OEM timeout/error branches can deliberately spin forever under debug flags; a new provider must replace them with bounded failures.",
            "The PIU ISR/DSR and 0x10000000 pointer alias are exact-build observations, not a portable address map or permission to install an SPMP-OS interrupt handler.",
            "Paths under /fat20a2/hda2/xyz are OEM development fallbacks, not the user SD layout selected for SPMP-OS.",
        ],
    }
    if image_root is not None:
        result["private_image_inventory"] = inspect_images(image_root)
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rap", required=True, type=Path)
    parser.add_argument("--image-root", type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    result = audit(args.rap, args.image_root)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"PASS: {len(result['verified_calls'])} callsites, {len(result['video_triplets'])} codec families")
    if args.image_root is not None:
        print(f"PASS: {len(result['private_image_inventory'])} private MMPi files traversed")


if __name__ == "__main__":
    main()
