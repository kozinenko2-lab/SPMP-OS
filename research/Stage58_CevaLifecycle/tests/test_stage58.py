import importlib.util
import os
import struct
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("audit_ceva_lifecycle", ROOT / "tools" / "audit_ceva_lifecycle.py")
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class Stage58Tests(unittest.TestCase):
    def test_section_descriptor_decode(self):
        cached = MODULE.decode_section(0x02300C1E)
        self.assertEqual(cached["physical_base"], 0x02300000)
        self.assertEqual(cached["ap"], 3)
        self.assertTrue(cached["cacheable"])
        self.assertTrue(cached["bufferable"])
        alias = MODULE.decode_section(0x02300C12)
        self.assertFalse(alias["cacheable"])
        self.assertFalse(alias["bufferable"])

    def test_non_section_rejected(self):
        with self.assertRaises(ValueError):
            MODULE.decode_section(0x00064011)

    def test_mmu_install_fixture(self):
        info = MODULE.CODE_FAMILIES[next(iter(MODULE.CODE_FAMILIES))]
        size = info["install"] - info["load"] + 4 * len(MODULE.MMU_INSTALL_WORDS)
        data = bytearray(size)
        struct.pack_into("<" + "I" * len(MODULE.MMU_INSTALL_WORDS), data,
                         info["install"] - info["load"], *MODULE.MMU_INSTALL_WORDS)
        words = tuple(struct.unpack_from("<I", data, info["install"] - info["load"] + i * 4)[0]
                      for i in range(len(MODULE.MMU_INSTALL_WORDS)))
        self.assertEqual(words[-1], 0x00060000)

    @unittest.skipUnless(os.environ.get("SPMP_MMI_RAP") and os.environ.get("SPMP_REDBOOT_CODES")
                         and os.environ.get("SPMP_REDBOOT_TABLES"),
                         "set exact private firmware inputs for full audit")
    def test_exact_oem_contract(self):
        codes = [Path(p) for p in os.environ["SPMP_REDBOOT_CODES"].split(os.pathsep)]
        tables = [Path(p) for p in os.environ["SPMP_REDBOOT_TABLES"].split(os.pathsep)]
        result = MODULE.audit(Path(os.environ["SPMP_MMI_RAP"]), codes, tables)
        self.assertEqual(result["source_rap_sha256"], MODULE.S56.RAP_SHA256)
        self.assertEqual(len(result["verified_calls"]), 14)
        self.assertEqual(len(result["verified_words"]), 12)
        self.assertEqual(len(result["redboot_code_segments"]), 5)
        self.assertEqual(len(result["redboot_page_tables"]), 5)
        self.assertEqual({row["family"] for row in result["redboot_code_segments"]},
                         {"redboot_1_2_4", "redboot_3_5"})


if __name__ == "__main__":
    unittest.main(verbosity=2)
