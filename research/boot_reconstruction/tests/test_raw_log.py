import json,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(BASE/'tools'))
from raw_log_decode import decode
from make_raw_profile import header,produce
META='#SPMP_RAW_INPUT_V2,app=test\n'
PRE='PRE,00000001,00000020,HOLD_SHORT,NA,NA,NA,00000040,00000400,NA,NA,00000000\n'
SAMPLE='SAMPLE,00000001,00000021,HOLD_SHORT,00000000,00000000,00000000,00000040,00000400,00000040,00000000,00000001\n'
END='END,00000002,00000040,FREE,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000\n'
class RawLogTests(unittest.TestCase):
 def test_pre_preserves_unknown_after_and_arm_is_not_a_physical_candidate(self):
  r=decode(META+PRE.replace('HOLD_SHORT','ARM')+PRE)
  self.assertIsNone(r['sessions'][0]['events'][0]['cooked'])
  self.assertIsNone(r['sessions'][0]['events'][0]['event_after'])
  self.assertNotIn('ARM',r['sessions'][0]['observed_masks_by_prompt'])
  self.assertEqual(r['sessions'][0]['observed_masks_by_prompt']['HOLD_SHORT']['state_before'],['0x00000040'])
 def test_hidden_raw_mask_is_separate_from_cooked_output(self):
  r=decode(META+SAMPLE+END);s=r['sessions'][0]
  self.assertTrue(s['ended']);self.assertNotIn('cooked',s['observed_masks_by_prompt']['HOLD_SHORT'])
  self.assertEqual(s['observed_masks_by_prompt']['HOLD_SHORT']['event_before'],['0x00000400'])
 def test_sessions_do_not_merge_power_hold_or_volume_chords(self):
  r=decode(META+SAMPLE+END+META+SAMPLE.replace('HOLD_SHORT','POWER_SHORT')+SAMPLE.replace('HOLD_SHORT','SELECT_VOL_PLUS'))
  self.assertEqual(len(r['sessions']),2)
  self.assertNotIn('POWER_SHORT',r['sessions'][0]['observed_masks_by_prompt'])
  self.assertIn('POWER_SHORT',r['sessions'][1]['observed_masks_by_prompt']);self.assertIn('SELECT_VOL_PLUS',r['sessions'][1]['observed_masks_by_prompt'])
 def test_partial_or_unknown_completed_fields_are_rejected(self):
  r=decode(META+SAMPLE.replace('00000040','NA')+PRE[:50]+'\n'+PRE.replace('NA,NA,NA','00000000,NA,NA'))
  self.assertEqual(r['ignored_or_partial_lines'],3);self.assertEqual(r['sessions'][0]['events'],[])
 def test_generated_profiles_match_and_unknown_firmware_rejected(self):
  r=json.loads((BASE/'evidence/raw_input_profile.json').read_text())
  self.assertEqual(header(r),(BASE/'examples/raw_input/raw_profile.h').read_text())
  self.assertEqual(header(r,True),(BASE/'tests/raw_profile_synthetic.h').read_text())
  self.assertEqual(r['state_address'],0x556144);self.assertEqual(r['event_address'],0x556148)
  with self.assertRaises(ValueError):produce(b'unknown firmware')
