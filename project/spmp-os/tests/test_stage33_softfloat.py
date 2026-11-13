"""Stage33 ARMv5EABI and host IEEE-754 oracle validations."""
import json
import pathlib
import subprocess
import unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'build/stage27/gpsp_armv5'
REQUIRED=('__aeabi_d2lz','__aeabi_dmul','__aeabi_f2uiz','__aeabi_fadd',
          '__aeabi_fcmpge','__aeabi_fdiv','__aeabi_fmul','__aeabi_fsub',
          '__aeabi_ui2d','__aeabi_ui2f')

class Stage33(unittest.TestCase):
    def test_cross_compilation_and_all_imports_resolved(self):
        subprocess.run(['python3','tools/build_gpsp_armv5.py'],cwd=ROOT,check=True)
        audit=json.loads((OUT/'audit.json').read_text())
        self.assertEqual((audit['passed'],audit['total']),(15,15))
        self.assertTrue(audit['linked'])
        self.assertTrue(audit['spmp_aeabi_softfloat.c']['ok'])
        self.assertEqual(audit['unresolved_symbols'], [])
        self.assertFalse(audit.get('device_tested',False))
    def test_ARMv5_softfp_correct_EABI(self):
        obj=OUT/'spmp_aeabi_softfloat.o'
        elf=subprocess.check_output(['readelf','-h',str(obj)],text=True)
        attrs=subprocess.check_output(['readelf','-A',str(obj)],text=True)
        self.assertIn('Machine:                           ARM',elf)
        self.assertIn('REL (Relocatable file)',elf)
        self.assertIn('Version5 EABI',elf)
        self.assertIn('Tag_CPU_arch: v5TE',attrs)
        self.assertNotIn('Tag_ABI_VFP_args',attrs)
        syms=subprocess.check_output(['nm','-g',str(OUT/'gpsp_interpreter_core.o')],text=True)
        for sym in REQUIRED: self.assertRegex(syms,rf'(?m)^\w+ T {sym}$')
        unresolved=subprocess.check_output(['nm','-u',str(OUT/'gpsp_interpreter_core.o')],text=True)
        self.assertEqual(unresolved.strip(),'')
    def test_host_ieee754_oracle(self):
        p=subprocess.run([str(ROOT/'build/stage33_softfloat_harness')],cwd=ROOT,text=True,capture_output=True)
        self.assertEqual(p.returncode,0,p.stderr[:3000])
        self.assertIn('PASS',p.stdout)

if __name__=='__main__':unittest.main()
