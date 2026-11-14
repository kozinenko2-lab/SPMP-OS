import importlib.util
import os
import struct
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("audit_ceva_abi", ROOT / "tools/audit_ceva_abi.py")
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def packet(opcode, no_reply, payload):
    return bytes((opcode, no_reply)) + struct.pack("<H", len(payload)) + payload


class Stage57Tests(unittest.TestCase):
    def test_request_boundaries(self):
        row = MODULE.validate_request(packet(0x41, 0, bytes(252)))
        self.assertEqual(row["total_bytes"], 256)
        self.assertFalse(row["no_reply"])
        with self.assertRaises(ValueError):
            MODULE.validate_request(packet(0x41, 0, bytes(253)))

    def test_truncation_rejected(self):
        with self.assertRaises(ValueError):
            MODULE.validate_request(b"\x40\x00\x01")
        with self.assertRaises(ValueError):
            MODULE.validate_request(b"\x40\x00\x08\x00ABCD")
        with self.assertRaises(ValueError):
            MODULE.validate_request(b"\x47\x00\x00\x00")

    @unittest.skipUnless(os.environ.get("SPMP_MMI_RAP"), "set SPMP_MMI_RAP for exact private audit")
    def test_exact_oem_contract(self):
        result = MODULE.audit(Path(os.environ["SPMP_MMI_RAP"]))
        self.assertEqual(result["source_rap_sha256"], MODULE.S56.RAP_SHA256)
        self.assertEqual(len(result["verified_calls"]), 15)
        self.assertEqual(len(result["verified_words"]), 13)
        self.assertEqual(result["ready_states"]["media_loader_max_reads"], 31)
        self.assertEqual(result["incoming_buffer"]["safe_payload_limit"], 252)


if __name__ == "__main__":
    unittest.main(verbosity=2)
