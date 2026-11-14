import importlib.util
import os
import struct
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("audit_media_loader", ROOT / "tools/audit_media_loader.py")
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def descriptor(flags, entry, segments):
    data = bytearray(b"MMPd" + struct.pack("<8I", 36, 0x01000001, 0, flags, len(segments), entry, 0, 0))
    for destination, payload in segments:
        assert len(payload) % 4 == 0
        data += b"MMPs" + struct.pack("<5I", 24 + len(payload), destination, 0, 0, 0) + payload
    return data


def fixture():
    body = descriptor(0x61000200, 0x00100000, [(0x1000, b"ABCD")])
    body += descriptor(0x61010200, 0xFFFFFFFF, [(0x2000, b"12345678")])
    data = bytearray(b"MMPi" + b"\0" * 12 + body)
    struct.pack_into("<I", data, 4, len(data))
    struct.pack_into("<II", data, 8, 0x11223344, 0x55667788)
    return data


class Stage56Tests(unittest.TestCase):
    def test_two_descriptor_image(self):
        row = MODULE.parse_mmpi(fixture())
        self.assertEqual(row["descriptor_count"], 2)
        self.assertEqual(row["segment_count"], 2)
        self.assertEqual(row["payload_bytes"], 12)
        self.assertEqual(row["selected_entry"], "0x00100000")
        self.assertEqual([x["mode"] for x in row["descriptors"]], [0, 1])

    def test_corruption_rejected(self):
        cases = []
        bad = fixture()[:-4]
        cases.append(bad)
        bad = fixture(); bad[52] = ord("X")
        cases.append(bad)
        bad = fixture(); struct.pack_into("<I", bad, 56, 0xFFFFFFF0)
        cases.append(bad)
        for data in cases:
            with self.subTest(size=len(data)):
                with self.assertRaises(ValueError):
                    MODULE.parse_mmpi(data)

    @unittest.skipUnless(os.environ.get("SPMP_MMI_RAP"), "set SPMP_MMI_RAP for exact private audit")
    def test_exact_oem_loader(self):
        result = MODULE.audit(Path(os.environ["SPMP_MMI_RAP"]))
        self.assertEqual(result["source_rap_sha256"], MODULE.RAP_SHA256)
        self.assertEqual(len(result["verified_calls"]), 28)
        self.assertEqual(result["video_triplets"]["h264"]["rom"]["codec"], "/Rom/Image/h264_codec.img")
        self.assertEqual(result["jpeg_relative_triplet"]["codec"], "/jpgdec.img")
        self.assertEqual(result["ceva_command_contract"]["segment_write_command"]["opcode"], "0x90")
        self.assertEqual(result["piu_reply_irq_contract"]["interrupt"]["vector"], 31)
        self.assertEqual(result["piu_reply_irq_contract"]["reply_wait"]["function"], "0x0034a124")
        handlers = result["piu_reply_irq_contract"]["incoming_handlers"]
        self.assertEqual(list(handlers), [f"0x{x:02x}" for x in range(0x40, 0x47)])
        self.assertEqual(handlers["0x43"]["inferred_role"], "allocate_arm_memory")


if __name__ == "__main__":
    unittest.main(verbosity=2)
