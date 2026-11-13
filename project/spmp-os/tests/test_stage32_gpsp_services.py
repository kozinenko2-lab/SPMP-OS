import json
import pathlib
import subprocess
import unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]

class Stage32Tests(unittest.TestCase):
    def test_01_armv5_services_and_gpsp(self):
        subprocess.run(['python3','tools/build_gpsp_armv5.py'],cwd=ROOT,check=True)
        p=ROOT/'build/stage27/gpsp_armv5/audit.json'
        j=json.loads(p.read_text())
        self.assertEqual(j['passed'],15)
        self.assertTrue(j['linked'])
        for n in ('printf','sscanf','time','localtime'):
            self.assertNotIn(n,j['unresolved_symbols'])
        self.assertEqual(len(j['unresolved_symbols']),0)  # Stage33 resolved remaining EABI symbols
        for n in ('spmp_gpsp_services.c','spmp_gpsp_time.c'):
            self.assertTrue(j[n]['ok'],j[n]['stderr'])
        result=subprocess.run(['readelf','-h',str(ROOT/'build/stage27/gpsp_armv5/gpsp_interpreter_core.o')],capture_output=True,text=True,check=True).stdout
        self.assertIn('ARM',result)
        self.assertIn('REL (Relocatable file)',result)

if __name__=='__main__': unittest.main()
