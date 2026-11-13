"""Compatibility references and saved ARM observations; no private bytes required."""
import json,os,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from audit_boot import H
from make_binding_profile import extract,header,OFFSETS

class NativePackageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.profile=json.loads((BASE/'evidence/native_binding_profile.json').read_text())
        cls.vm=json.loads((BASE/'evidence/native_binding_vm.json').read_text())
        cls.rap=json.loads((BASE/'evidence/native_package_rap_vm.json').read_text())
        cls.package=json.loads((BASE/'evidence/native_package.json').read_text())

    def test_generated_headers_match_published_profile(self):
        self.assertEqual(self.profile['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(len(self.profile['slots']),85)
        self.assertEqual(header(self.profile),(BASE/'examples/native_startup/binding_profile.h').read_text())
        self.assertEqual(header(self.profile,True),(BASE/'tests/binding_profile_synthetic.h').read_text())
        original=json.loads((BASE/'evidence/stage3_verified.json').read_text())['native_api_slots']
        self.assertEqual(self.profile['slots'],[int(s['target'],16) for s in original])

    def test_all_nine_bound_slots_have_existing_typed_contracts(self):
        contracts=json.loads((BASE/'evidence/stage6_verified.json').read_text())['contracts']
        typed={int(c['offset'],16) for c in contracts}
        self.assertEqual([c['slot']*4 for c in self.profile['prefixes']],OFFSETS)
        self.assertTrue(set(OFFSETS)<=typed)
        for p in self.profile['prefixes']:
            self.assertEqual(p['target'],self.profile['slots'][p['slot']])
            self.assertEqual(p['bytes'],32)

    def test_every_slot_and_prefix_rejection_is_before_bss_and_api(self):
        cases=self.vm['cases'];slots=[c for c in cases if c['case'].startswith('slot_')]
        prefixes=[c for c in cases if c['case'].startswith('prefix_')]
        self.assertEqual({int(c['case'][5:]) for c in slots},set(range(85)))
        self.assertEqual({int(c['case'][7:]) for c in prefixes},set(range(9)))
        self.assertEqual(self.vm['case_count'],116)
        for c in slots+prefixes:
            self.assertFalse(c['main_called']);self.assertEqual(c['api_calls'],0)
            self.assertTrue(c['bss_untouched_on_rejection']);self.assertTrue(c['bss_guard_preserved'])

    def test_original_loader_and_normal_return_evidence(self):
        self.assertEqual(self.rap['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(self.rap['package'],self.package)
        self.assertEqual(self.rap['case_count'],6)
        for c in self.rap['cases']:
            if c['case'] in ('bad_magic','bad_footer'):
                self.assertFalse(c['entry_reached']);self.assertEqual(c['mmi_wrapper_return'],0xfffffffe)
            else:
                self.assertTrue(c['entry_reached']);self.assertEqual(c['mmi_wrapper_return'],0)
                self.assertEqual(c['commands'],['0xd06','0xd08','0xd07','0xd07'])
            self.assertTrue(c['sp_and_r4_r11_restored'])

    def test_package_metadata_describes_ram_application_only(self):
        p=self.package
        self.assertEqual(p['status'],'EXPERIMENTAL_RAM_NGAME_VM_ONLY')
        self.assertEqual(p['container_bytes'],128+p['payload_bytes'])
        self.assertEqual(p['tail_bytes'],0)
        self.assertEqual(p['payload_bytes']%8,0)
        self.assertEqual(p['structural_check']['status'],'structural_match')

    def test_unrecognized_rap_cannot_generate_profile(self):
        with self.assertRaises(ValueError):extract(b'not original RAP')

    @unittest.skipUnless(os.environ.get('SPMP_MMI_RAP'),'exact private RAP not provided')
    def test_optional_exact_rap_profile(self):
        self.assertEqual(extract(Path(os.environ['SPMP_MMI_RAP']).read_bytes()),self.profile)

    @unittest.skipUnless(os.environ.get('SPMP_MMI_RAP') and os.environ.get('SPMP_NATIVE_ELF'),
                         'exact RAP and source-built native ELF not provided')
    def test_optional_actual_package_chain(self):
        from emulate_native_package import private_rap
        result=private_rap(Path(os.environ['SPMP_NATIVE_ELF']),Path(os.environ['SPMP_MMI_RAP']))
        self.assertEqual(result['case_count'],6)
        def outcomes(report):
            return {c['case']:(c['mmi_wrapper_return'],c['entry_reached'],
                None if c['application'] is None else c['application']['status']) for c in report['cases']}
        self.assertEqual(outcomes(result),outcomes(self.rap))

if __name__=='__main__':unittest.main()
