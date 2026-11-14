"""Public provenance checks plus optional reproduction on the private RAP."""
import json,os,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from audit_boot import H

class CopyProofTests(unittest.TestCase):
 def test_host_arm_and_sanitizer_case_parity(self):
  reports=[json.loads((BASE/'evidence'/n).read_text()) for n in ('file_copy_host.json','file_copy_arm.json','file_copy_sanitizers.json')]
  cases=[{c['case']:c for c in d['cases']} for d in reports]
  self.assertEqual([d['case_count'] for d in reports],[50,50,50])
  self.assertEqual(set(cases[0]),set(cases[1]));self.assertEqual(set(cases[0]),set(cases[2]))
  for name in cases[0]:
   self.assertEqual([c[name]['result'] for c in cases],[cases[0][name]['result']]*3)
  self.assertTrue(all(c['max_transfer']<=2048 for d in reports for c in d['cases']))
  self.assertTrue(all(c['max_stack_bytes']<=8192 for c in reports[1]['cases']))
  for c in cases:
   self.assertTrue(c['normal']['published']);self.assertFalse(c['collision']['published'])
   self.assertEqual(c['cleanup_error']['cleanup_error'],-13);self.assertTrue(c['cleanup_error']['owned_stage'])
   self.assertEqual(c['unsupported']['callbacks'],0)
   self.assertTrue(c['cleanup_error']['cleanup_retry_pass'])
 def test_original_mutation_wrappers_do_not_establish_linux_errno_or_durability(self):
  d=json.loads((BASE/'evidence/file_mutations_arm.json').read_text())
  self.assertEqual(d['source_sha256'],H['8000_MMI.RAP']);self.assertEqual(d['case_count'],22)
  self.assertEqual(len(d['cases']),22)
  self.assertEqual([c['backend_calls'] for c in d['cases'] if c['case']=='fatfs_fsync_noop'],[0,0])
  self.assertIn(200,[c['wrapper_return_u32'] for c in d['cases'] if c['case']=='fatfs_rename'])
  self.assertIn(302,[c['wrapper_return_u32'] for c in d['cases'] if c['case']=='fatfs_mkdir'])
  self.assertFalse(next(c for c in d['cases'] if c['case']=='rename_driver_dispatch' and not c['same_volume'])['driver_called'])
 @unittest.skipUnless(os.environ.get('SPMP_MMI_RAP'),'Optional private original RAP')
 def test_original_arm_wrappers_reproduce(self):
  from audit_file_mutations import audit
  self.assertEqual(audit(Path(os.environ['SPMP_MMI_RAP']))['case_count'],22)

if __name__=='__main__':unittest.main()
