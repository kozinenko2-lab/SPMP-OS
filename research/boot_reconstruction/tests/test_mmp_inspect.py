"""Synthetic public tests, plus optional exact-build validation with local RAP."""
import hashlib, os, struct, subprocess, tempfile, unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
RAP=os.environ.get('SPMP_MMI_RAP')
SHA='5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e'
def sums(b):
    words=struct.unpack('<%dI'%((len(b)-16)//4),b[16:]);q=(len(b)-16)//16
    return ((sum(words[:q])+sum(words[2*q:3*q]))&0xffffffff,
            (sum(words[q:2*q])+sum(words[3*q:]))&0xffffffff)
def seal(b):
    struct.pack_into('<I',b,4,len(b));struct.pack_into('<II',b,8,*sums(b));return b
def fixture():
    b=bytearray(b'MMPi'+b'\0'*12)
    b.extend(b'MMPd'+struct.pack('<8I',36,0x01000001,0,0x61000001,1,0x280040,0,1))
    b.extend(b'MMPs'+struct.pack('<5I',28,0x280000,0x280000,0,1)+b'\x12\x34\x56\x78')
    return seal(b)
def run(b):
    with tempfile.NamedTemporaryFile() as f:
        f.write(b);f.flush()
        return subprocess.run([str(ROOT/'build/mmp_inspect'),f.name],capture_output=True,text=True)
class InspectorTests(unittest.TestCase):
    def test_synthetic_image(self):
        r=run(fixture());self.assertEqual(r.returncode,0,r.stderr)
        self.assertIn('count 1 entry 00280040',r.stdout)
        self.assertIn('destination 00280000 bytes 4',r.stdout)
    def test_modified_payload(self):
        b=fixture();b[-1]^=1;r=run(b)
        self.assertNotEqual(r.returncode,0);self.assertIn('checksum mismatch',r.stderr)
    def test_truncation(self):
        self.assertNotEqual(run(fixture()[:-4]).returncode,0)
    def test_invalid_segment_with_correct_checksum(self):
        b=fixture();struct.pack_into('<I',b,0x38,0xfffffff0);r=run(seal(b))
        self.assertNotEqual(r.returncode,0);self.assertIn('MMPs length',r.stderr)
    def test_checksum_remainder_runs(self):
        for n in (0,4,8,12,16,20):
            with self.subTest(n=n):
                b=seal(bytearray(b'MMPi'+b'\0'*12+bytes(range(n))))
                r=run(b);self.assertNotEqual(r.returncode,0)
                self.assertNotIn('checksum mismatch',r.stderr)
    def test_zero_payload_segment(self):
        b=fixture()[:-4];struct.pack_into('<I',b,0x38,24)
        r=run(seal(b));self.assertEqual(r.returncode,0,r.stderr)
    @unittest.skipUnless(RAP,'Set SPMP_MMI_RAP to test a local original; it is not published')
    def test_exact_original(self):
        b=Path(RAP).read_bytes();self.assertEqual(hashlib.sha256(b).hexdigest(),SHA)
        r=run(b);self.assertEqual(r.returncode,0,r.stderr)
        self.assertIn('dcedaee9 a8f5c6b5',r.stdout)
        self.assertIn('count 4 entry 00280040',r.stdout)
        self.assertEqual(r.stdout.count('MMPs '),4)
        self.assertIn('destination 9d807180 bytes 2024',r.stdout)
if __name__=='__main__':unittest.main(verbosity=2)
