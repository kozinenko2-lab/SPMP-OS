"""Stage30: real ARM EABI symbols and freestanding integer runtime."""
import json, pathlib, subprocess, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class Stage30Numeric(unittest.TestCase):
    def test_unresolved_is_reduced(self):
        d=json.loads((ROOT/'build/stage27/gpsp_armv5/audit.json').read_text())
        self.assertEqual((d['passed'],d['total']),(15,15))
        self.assertTrue(d['linked'] and d['runtime_ready'])
        unresolved=set(d['unresolved_symbols'])
        self.assertLessEqual(len(unresolved),17)
        for name in ('atoi','strtol','__aeabi_uidiv','__aeabi_uidivmod'):
            self.assertNotIn(name,unresolved)
        for name in ('__aeabi_fmul',):
            self.assertNotIn(name,unresolved)  # Supplied by Stage33 EABI
    def test_arm_abi_exports(self):
        out=subprocess.check_output(['nm','-g',str(ROOT/'build/stage27/gpsp_armv5/gpsp_interpreter_core.o')],text=True)
        self.assertIn('__aeabi_uidivmod',out)
        self.assertIn('strtol',out)
