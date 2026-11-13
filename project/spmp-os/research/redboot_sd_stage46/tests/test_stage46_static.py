import unittest,sys,hashlib
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from audit_cmd17_path import audit,W,SHA,get,target
RAP=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/mnt/data/8000_MMI.RAP')
class Audits(unittest.TestCase):
 @classmethod
 def setUpClass(cls):cls.data=RAP.read_bytes()
 def test_sha(self):self.assertEqual(hashlib.sha256(self.data).hexdigest(),SHA)
 def test_machine(self):self.assertEqual(audit(self.data)['verified_words'],len(W))
 def test_copy_mutation(self):
  b=bytearray(self.data); off=0x4c+0x2bb258-0x280000;b[off]^=1
  with self.assertRaises(ValueError):audit(bytes(b))
 def test_calls(self):
  d=audit(self.data)
  self.assertEqual([x['low_6_bits'] for x in d['command_calls']],[18,18])
 def test_loop(self):
  d=audit(self.data);self.assertEqual(len(d['poll_loops']),4)
  self.assertEqual(sum(x['local_bound'].startswith('none') for x in d['poll_loops']),3)
 def test_not_cmd17(self):
  self.assertIn('NOT proven',audit(self.data)['cmd17_status'])
if __name__=='__main__':
 # argv needs only source file passed to setUpClass
 unittest.main(argv=[sys.argv[0]])
