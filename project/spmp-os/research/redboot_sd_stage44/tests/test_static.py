import importlib.util,os,unittest
from pathlib import Path
MOD=Path(__file__).resolve().parents[1]/'tools/recover_stage44.py'
spec=importlib.util.spec_from_file_location('stage44_static',MOD)
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
RAP=Path(os.environ.get('SPMP_RAP','/mnt/data/8000_MMI.RAP'))
class StaticTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.raw=RAP.read_bytes()
  cls.result=m.analyze(cls.raw)
 def test_fat_table_count(self):self.assertEqual(len(self.result['fat_fileops']),10)
 def test_fat_write(self):self.assertEqual(self.result['fat_fileops'][1]['address'],'0x00283fc4')
 def test_fsync_nop(self):self.assertIn('return zero',self.result['fsync_implementation']['effect'])
 def test_timeout(self):self.assertEqual(self.result['sd_poll']['limit_comparison'],50000)
 def test_command_markers(self):self.assertTrue(all(x['found'] for x in self.result['sd_command_diagnostics']))
 def test_shasafe(self):
  x=bytearray(self.raw);x[-1]^=1
  with self.assertRaises(ValueError):m.analyze(x)
 def test_call_decoder(self):self.assertEqual(m.bl_target(0x284090,m.readword(self.raw,0x284090)),0x2a9834)
 def test_file_offset(self):self.assertEqual(self.result['fat_fileops_table_rap_offset'],'0x00293d28')
if __name__=='__main__':unittest.main()
