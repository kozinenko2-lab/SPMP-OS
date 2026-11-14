import importlib.util
import os
import struct
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
SPEC=importlib.util.spec_from_file_location("audit_ceva_commands",ROOT/"tools"/"audit_ceva_commands.py")
MODULE=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(MODULE)


class Stage59Tests(unittest.TestCase):
 def test_command_packet(self):
  self.assertEqual(MODULE.command_packet(0xC1,0x0055866C),bytes.fromhex("c10004006c865500"))
  self.assertEqual(struct.unpack("<BBHI",MODULE.command_packet(0xC0,0x01234560)),(0xC0,0,4,0x01234560))
  with self.assertRaises(ValueError):MODULE.command_packet(0x43,0)

 def test_checked_alias_boundaries(self):
  self.assertEqual(MODULE.checked_uncached_alias(0x00800000,0x60,0x00800000,0x00900000),0x10800000)
  self.assertEqual(MODULE.checked_uncached_alias(0x008fffA0,0x60,0x00800000,0x00900000),0x108fffA0)
  for args in [(0x007ffffc,0x60,0x00800000,0x00900000),(0x008ffffc,8,0x00800000,0x00900000),
               (0x00800002,0x60,0x00800000,0x00900000),(0x0ffffffc,8,0,0x10000000),
               (0x00800000,0,0x00800000,0x00900000)]:
   self.assertEqual(MODULE.checked_uncached_alias(*args),0)

 @unittest.skipUnless(os.environ.get("SPMP_MMI_RAP"),"set exact private firmware input for full audit")
 def test_exact_oem_contract(self):
  result=MODULE.audit(Path(os.environ["SPMP_MMI_RAP"]))
  self.assertEqual(result["source_rap_sha256"],MODULE.S56.RAP_SHA256)
  self.assertEqual(len(result["verified_calls"]),17)
  self.assertEqual(len(result["verified_words"]),29)
  self.assertEqual(len(result["verified_strings"]),15)


if __name__=="__main__":unittest.main(verbosity=2)
