import os, pathlib, sys, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import audit_stage48_init as sd
RAP=pathlib.Path(os.environ.get('SPMP48_RAP','/mnt/data/8000_MMI.RAP'))
class AuditTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.raw=RAP.read_bytes()
    def test_30_calls(self):
        d=sd.analyze(self.raw)
        self.assertEqual(d['verified_dispatch_calls'],30)
        self.assertEqual(d['cmd17_direct_calls_in_scanned_range'],0)
        self.assertEqual(sum(d['command_counts'].values()),30)
    def test_initialization(self):
        d=sd.analyze(self.raw)
        self.assertEqual(d['command_counts']['ACMD41'],2)
        self.assertEqual(d['command_counts']['CMD55'],2)
        self.assertEqual(d['command_counts']['CMD8'],1)
    def test_tampered_instruction_rejected(self):
        altered=bytearray(self.raw)
        off=sd.ROM_OFFSET+0x2ba4e8-sd.ROM_BASE
        altered[off]^=0x04
        with self.assertRaises(ValueError):sd.analyze(altered)
    def test_other_image_rejected(self):
        with self.assertRaises(ValueError):sd.analyze(self.raw[:-1])
    def test_bl_decoder(self):
        self.assertEqual(sd.bl_target(0x2ba4e8,sd.word(self.raw,0x2ba4e8)),sd.SENDER)
        self.assertEqual(sd.bl_target(0x2ba4e8,0),None)
    def test_upper_bits_preserved(self):
        d=sd.analyze(self.raw)
        self.assertEqual(next(c for c in d['calls'] if c['pc']=='0x2ba4e8')['oem_command_word'],'0x80')
        self.assertEqual(next(c for c in d['calls'] if c['pc']=='0x2bb014')['oem_command_word'],'0x1592')
if __name__=='__main__':unittest.main()
