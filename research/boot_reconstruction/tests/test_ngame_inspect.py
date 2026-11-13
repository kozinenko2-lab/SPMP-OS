import hashlib,json,random,struct,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from ngame_inspect import transform,inspect,FOOTER,KEY

def fixture(payload_bytes=64,extra=b''):
    # Diagnostic byte pattern with marker, no program or valid entry instructions.
    payload=bytearray(payload_bytes)
    payload[-16:-16+len(FOOTER)]=FOOTER
    prefix=min(payload_bytes,1024);payload[:prefix]=transform(payload[:prefix],mode=0)
    header=bytearray(128);header[:5]=b'NGame';header[0x6d]=7
    struct.pack_into('<I',header,0x78,len(extra))
    return header+payload+extra
class NGameTests(unittest.TestCase):
    def test_original_arm_vectors(self):
        evidence=json.loads((BASE/'evidence/native_arm_emulation.json').read_text())
        rng=random.Random(0x8000)
        cases=[(bytes.fromhex('0123456789abcdef'),bytes.fromhex('133457799bbcdff1'))]
        cases.extend((bytes(rng.randrange(256) for _ in range(n)),bytes(rng.randrange(256) for _ in range(8))) for n in [8,16,24,64,128,1024])
        self.assertEqual(len(evidence['cases']),14)
        for vector in evidence['cases']:
            with self.subTest(case=vector['case'],mode=vector['mode']):
                data,key=cases[vector['case']]
                self.assertEqual(hashlib.sha256(data).hexdigest(),vector['input_sha256'])
                self.assertEqual(hashlib.sha256(transform(data,key,vector['mode'])).hexdigest(),vector['output_sha256'])
    def test_short_structural_fixture(self):
        result=inspect(fixture());self.assertEqual(result['status'],'structural_match')
        self.assertEqual(result['payload_bytes'],64);self.assertEqual(result['lcm_index'],7)
    def test_resource_tail_excluded(self):
        result=inspect(fixture(extra=b'not executable resource'))
        self.assertEqual(result['status'],'structural_match');self.assertEqual(result['payload_bytes'],64)
    def test_wrong_key_fails_short_footer(self):
        data=fixture();payload=bytearray(64);payload[-16:-16+len(FOOTER)]=FOOTER
        data[128:]=transform(payload,b'wrongkey',0)
        self.assertEqual(inspect(data)['status'],'bad_decoded_footer')
    def test_unencoded_short_footer_fails(self):
        data=fixture();data[128:]=transform(data[128:])
        self.assertEqual(inspect(data)['status'],'bad_decoded_footer')
    def test_bounds(self):
        self.assertEqual(inspect(b'NGame')['status'],'truncated_header')
        bad=fixture();bad[:5]=b'WRONG'
        self.assertEqual(inspect(bad)['status'],'bad_magic')
        bad=fixture();struct.pack_into('<I',bad,0x78,0xffffffff)
        self.assertEqual(inspect(bad)['status'],'unsafe_extra_size')
        bad=fixture()[:-1]
        self.assertEqual(inspect(bad)['status'],'unsafe_partial_cipher_block')
        self.assertEqual(inspect(fixture()[:128])['status'],'unsafe_payload_size')
    def test_transform_is_reversible(self):
        data=bytes(range(64));self.assertEqual(transform(transform(data,KEY,0),KEY,1),data)
    def test_no_executable_certification(self):
        # Long raw bytes can pass this OEM structural predicate because the footer
        # lies outside the transformed prefix. No CPU/ABI compatibility is implied.
        bad=bytearray(128+2048);bad[:5]=b'NGame'
        bad[-16:-16+len(FOOTER)]=FOOTER
        self.assertEqual(inspect(bad)['status'],'structural_match')
    def test_reject_unsupported_key_and_partial_block(self):
        with self.assertRaises(ValueError):transform(b'1234567')
        with self.assertRaises(ValueError):transform(b'12345678',b'1234567890123456')
if __name__=='__main__':unittest.main(verbosity=2)
