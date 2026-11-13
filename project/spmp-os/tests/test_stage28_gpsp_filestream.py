import json
import pathlib
import subprocess
import unittest

ROOT=pathlib.Path(__file__).resolve().parents[1]

class Stage28FileTests(unittest.TestCase):
    def test_arm_link_and_absent_filestream_symbols(self):
        d=json.loads((ROOT/'build/stage27/gpsp_armv5/audit.json').read_text())
        self.assertEqual(d['passed'],15)
        self.assertTrue(d['linked'] and d['runtime_ready'])
        self.assertLessEqual(len(d['unresolved_symbols']),36)  # Stage 29 resolves additional runtime symbols
        self.assertFalse(any(s.startswith('filestream_') for s in d['unresolved_symbols']))
    def test_elf_arm(self):
        output=subprocess.check_output(['readelf','-h',str(ROOT/'build/stage27/gpsp_armv5/gpsp_interpreter_core.o')],text=True)
        self.assertIn('ARM',output)
        self.assertIn('REL (Relocatable file)',output)
