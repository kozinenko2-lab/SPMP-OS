import sys, unittest, hashlib
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(HERE/'tools'))
from audit_sd_poll import audit, word, branch_target, SHA, CHECK
class TestSDPoll(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.p=TARGET
        cls.b=cls.p.read_bytes()
    def test_hash(self):self.assertEqual(hashlib.sha256(self.b).hexdigest(),SHA)
    def test_verified_instruction_count(self):self.assertGreaterEqual(len(CHECK),28)
    def test_poll_audit(self):
        r=audit(self.b)
        self.assertEqual(r['oem_counter']['threshold'],50000)
        self.assertEqual(len(r['verified_callers']),4)
        self.assertIn('return 0',r['known_gap'])
    def test_abi_offsets(self):
        self.assertEqual(word(self.b,0x2b9b58),0x92b0b014)
        self.assertEqual(word(self.b,0x2b9b5c),0x00555f18)
    def test_corrupted_rejected(self):
        x=bytearray(self.b);x[0x1000]^=1
        with self.assertRaisesRegex(ValueError,'SHA'):audit(x)
    def test_branch_check(self):
        for pc in (0x2b9bc4,0x2b9c74,0x2b9c98,0x2ba494):
            self.assertEqual(branch_target(pc,word(self.b,pc)),0x2b9a38)
        with self.assertRaises(ValueError):branch_target(0x2b9b58,0x92b0b014)
if __name__=='__main__':
    TARGET=Path(sys.argv.pop())
    unittest.main()
