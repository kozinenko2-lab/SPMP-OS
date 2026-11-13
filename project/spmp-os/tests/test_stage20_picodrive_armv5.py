import json
import subprocess
import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class Stage20ArmPicoDrive(unittest.TestCase):
    def test_compilation_manifest(self):
        r=json.loads((ROOT/'build/picodrive_armv5_expanded/report.json').read_text())
        self.assertEqual(r['total'],31)
        self.assertEqual(r['compiled'],31)
        self.assertTrue(r['partial_link_ok'])
        self.assertGreater(r['unresolved_count'],0) # not a bootable emulator
    def test_elf_attributes(self):
        obj=ROOT/'build/picodrive_armv5_expanded/picodrive_expanded_armv5_unresolved.o'
        r=subprocess.check_output(['readelf','-h',str(obj)],text=True)
        self.assertIn('ELF32',r)
        self.assertIn('ARM',r)
        self.assertIn('Version5 EABI',r)
        self.assertIn('REL (Relocatable file)',r)
    def test_no_armv7_neon_in_disassembly(self):
        # Inspect emitted attributes rather than presume performance.
        obj=ROOT/'build/picodrive_armv5_expanded/picodrive_expanded_armv5_unresolved.o'
        r=subprocess.check_output(['readelf','-A',str(obj)],text=True)
        self.assertIn('arm926ej-s',r)
        self.assertNotIn('NEON',r)
if __name__=='__main__':unittest.main()
