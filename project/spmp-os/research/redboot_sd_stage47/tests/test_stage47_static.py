#!/usr/bin/env python3
import sys, pathlib, unittest
from importlib.util import spec_from_file_location, module_from_spec
TOOL=pathlib.Path(__file__).resolve().parents[1]/'tools'/'audit_stage47_sd.py'
spec=spec_from_file_location('audit',TOOL);audit=module_from_spec(spec);spec.loader.exec_module(audit)
class TestAudit(unittest.TestCase):
 def test_hash_pinned(self):self.assertEqual(len(audit.SHA),64)
 def test_opcode_count(self):self.assertGreaterEqual(len(audit.W),35)
 def test_command_count(self):self.assertEqual(sum(map(len,audit.CALLS.values())),9)
 def test_signed_branch(self):
  self.assertEqual(audit.branch_target(0x2bb014,audit.W[0x2bb014]),0x2b9b78)
 def test_no_pseudo_cmd17(self):self.assertNotIn('CMD17',audit.CALLS)
 def test_corrupt_opcode_rejected(self):
  # Entire RAP cannot be reconstructed from small test fixture; check
  # precise branch decoder and opcode identity instead.
  with self.assertRaises(ValueError):audit.branch_target(0x2bb014,0)
 def test_all_dispatch_pinned(self):
  for v in audit.CALLS.values():
   for mov,add,bl,imm,delta in v:
    self.assertEqual(audit.branch_target(bl,audit.W[bl]),0x2b9b78)
    self.assertLess(imm+delta,1<<16)
if __name__=='__main__':unittest.main()
