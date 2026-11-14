import json,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from input_log_decode import decode
from make_input_profile import header,produce
class InputLogTests(unittest.TestCase):
 def test_sessions_preserve_candidate_codes_and_do_not_merge_devices(self):
  sample='#SPMP_INPUT_TEST_V1,source=test\nevent,poll_hex,time_hex,prompt,mask_hex,pressed_hex,released_hex\nSAMPLE,00000001,00000020,UP,00000001,00000001,00000000\nEND,00000002,00000040,UP,00000000,00000000,00000000\n'
  r=decode(sample+sample.replace('UP,00000001','UP,00080000'),'user label')
  self.assertEqual(len(r['sessions']),2)
  self.assertEqual(r['sessions'][0]['candidate_mappings'],{'UP':['0x00000001']})
  self.assertEqual(r['sessions'][1]['candidate_mappings'],{'UP':['0x00080000']})
  self.assertTrue(all(s['ended'] for s in r['sessions']))
 def test_partial_records_do_not_become_mappings(self):
  r=decode('old content\n#SPMP_INPUT_TEST_V1,source=test\nSAMPLE,00000001,00000020,UP,123\nKEY,00000001,00000020,UP,00000001,00000001,00000000\n')
  self.assertEqual(r['ignored_or_partial_lines'],2)
  self.assertEqual(r['sessions'][0]['candidate_mappings'],{})
  self.assertFalse(r['sessions'][0]['ended'])
 def test_input_headers_match_public_metadata(self):
  r=json.loads((BASE/'evidence/input_profile.json').read_text())
  self.assertEqual(header(r),(BASE/'examples/input_test/input_profile.h').read_text())
  self.assertEqual(header(r,True),(BASE/'tests/input_profile_synthetic.h').read_text())
  self.assertEqual(r['open_flags'],10);self.assertEqual(r['seek_end_whence'],2)
 def test_unknown_original_rejected(self):
  with self.assertRaises(ValueError):produce(b'other image')
