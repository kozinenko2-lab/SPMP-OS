"""Verify Stage31 ARMv5 64-bit division is linked and unrelated imports remain explicit."""
import json, pathlib, subprocess, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'build/stage27/gpsp_armv5'
class Division(unittest.TestCase):
    def test_real_arm_core(self):
        d=json.loads((OUT/'audit.json').read_text())
        self.assertEqual((d['passed'],d['total']),(15,15))
        self.assertTrue(d['linked'] and d['runtime_ready'])
        self.assertLessEqual(len(d['unresolved_symbols']),16)
        self.assertNotIn('__aeabi_uldivmod',d['unresolved_symbols'])
        self.assertNotIn('__aeabi_fmul',d['unresolved_symbols'])  # Stage33 EABI
        self.assertNotIn('time',d['unresolved_symbols'])  # Stage32 virtual/host clock facade
    def test_arm_eabi_symbol(self):
        p=subprocess.run(['nm',str(OUT/'gpsp_interpreter_core.o')],check=True,text=True,capture_output=True)
        self.assertRegex(p.stdout,r'(?m)^\w+ T __aeabi_uldivmod$')
        self.assertRegex(p.stdout,r'(?m)^\w+ T spmp_udivmod64$')
        a=subprocess.run(['readelf','-A',str(OUT/'gpsp_interpreter_core.o')],check=True,text=True,capture_output=True)
        self.assertIn('Tag_CPU_arch: v5TE',a.stdout)
if __name__=='__main__':unittest.main()
