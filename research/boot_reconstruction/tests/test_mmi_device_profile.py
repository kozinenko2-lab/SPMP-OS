import copy,hashlib,json,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(BASE/'tools'))
from make_mmi_device_profile import produce,LABELS
from make_shell_profile import header,synthetic_prefix
from raw_log_decode import decode

class DeviceProfileTests(unittest.TestCase):
 def setUp(self):
  self.profile=json.loads((BASE/'device_profiles/defender_multimix_magic_user_console_01.json').read_text())
 def test_profile_repeats_in_each_complete_hardware_session(self):
  blob=(BASE/'evidence/hardware/raw_input_user_console_01.csv').read_bytes()
  self.assertEqual(hashlib.sha256(blob).hexdigest(),self.profile['raw_source_log']['sha256'])
  data=decode(blob.decode('ascii'));self.assertEqual(data['ignored_or_partial_lines'],0)
  self.assertEqual([len(s['events']) for s in data['sessions']],[201,199])
  for s in data['sessions']:
   self.assertTrue(s['ended'])
   for label in LABELS:
    masks=s['observed_masks_by_prompt'][label]
    self.assertIn(self.profile['raw_state_layer']['candidate_mappings'][label],masks['state_before'])
   self.assertIn('0x00001000',s['observed_masks_by_prompt']['POWER_SHORT']['event_before'])
   for label,mask in [('SELECT_VOL_PLUS','0x00002800'),('SELECT_VOL_MINUS','0x00002200')]:
    self.assertIn(mask,s['observed_masks_by_prompt'][label]['state_before'])
 def test_selected_profile_header_is_current(self):
  self.assertEqual(produce(self.profile),(BASE.parents[1]/'project/spmp-os/include/spmp_mmi_user01_profile.h').read_text())
  self.assertFalse(self.profile['auto_apply'])
 def test_state_duplicates_rejected_but_event_namespace_is_separate(self):
  p=copy.deepcopy(self.profile);p['raw_state_layer']['candidate_mappings']['X']=p['raw_state_layer']['candidate_mappings']['A']
  with self.assertRaises(ValueError):produce(p)
  self.assertEqual(self.profile['raw_state_layer']['candidate_mappings']['START'],self.profile['raw_event_layer']['candidate_mappings']['POWER_SHORT'])
  self.assertIn('0x1000u',produce(self.profile))
 def test_shell_profiles_and_literal_fixtures(self):
  r=json.loads((BASE/'evidence/mmi_shell_profile.json').read_text())
  self.assertEqual(header(r),(BASE/'examples/mmi_shell/shell_profile.h').read_text())
  self.assertEqual(header(r,True),(BASE/'tests/shell_profile_synthetic.h').read_text())
  for p in r['prefixes']:
   self.assertEqual(int.from_bytes(synthetic_prefix(p['address'])[12:16],'little'),p['value'])
