"""Stage29: actual cross-linked ARMv5 gpSP symbol audit."""
import json
import pathlib
import subprocess
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
AUDIT = ROOT / 'build/stage27/gpsp_armv5/audit.json'
MODULE = ROOT / 'build/stage27/gpsp_armv5/gpsp_interpreter_core.o'

class RuntimeLinkTests(unittest.TestCase):
    def test_all_gpsp_sources_compiled(self):
        d = json.loads(AUDIT.read_text())
        self.assertEqual(d['passed'], 15)
        self.assertEqual(d['total'], 15)
        self.assertTrue(d['linked'])
        self.assertTrue(d['runtime_ready'])

    def test_memory_abi_symbols_resolved(self):
        d = json.loads(AUDIT.read_text())
        unresolved = set(d['unresolved_symbols'])
        self.assertLessEqual(len(unresolved), 21)
        for name in ('memcpy','memmove','memset','memcmp','strlen','strcmp',
                     'strcpy','strcat','strrchr','__aeabi_memcpy','__aeabi_memcpy4',
                     '__aeabi_memcpy8','__aeabi_memmove','__aeabi_memmove4',
                     '__aeabi_memset','__aeabi_memset4','__aeabi_memclr','__aeabi_memclr4'):
            self.assertNotIn(name, unresolved, name)
        self.assertNotIn('__aeabi_fmul', unresolved)  # Stage33 genuine software floating point
        self.assertNotIn('time', unresolved)       # Stage32 RTC with explicit fallback

    def test_armv5_relocatable(self):
        p = subprocess.run(['readelf','-h',str(MODULE)],capture_output=True,text=True,check=True)
        self.assertIn('ARM',p.stdout)
        self.assertIn('REL (Relocatable file)',p.stdout)
        a = subprocess.run(['readelf','-A',str(MODULE)],capture_output=True,text=True,check=True)
        self.assertIn('Tag_CPU_arch: v5TE',a.stdout)

if __name__ == '__main__':
    unittest.main()
