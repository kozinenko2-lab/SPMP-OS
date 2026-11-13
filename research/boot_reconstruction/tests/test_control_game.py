"""Control-sample regressions use published metadata, never private game bytes.
Optional exact-file checks: SPMP_CONTROL_GAME, and SPMP_MMI_RAP for ARM execution.
"""
import copy,json,os,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from audit_boot import H
from control_game_inspect import GAME_SHA256,PAYLOAD_SHA256,decode,inspect,memory_comparison
from native_memory_plan import overlap
from run_control_ghidra import private_directory,REPO

class ControlGameTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.evidence=json.loads((BASE/'evidence/control_game_verified.json').read_text())
        cls.arm=json.loads((BASE/'evidence/control_game_arm_emulation.json').read_text())
        cls.profile=json.loads((BASE/'examples/native_memory_profile.json').read_text())

    def test_provenance_container_and_real_loader_decode_agree(self):
        e=self.evidence;a=self.arm
        self.assertEqual(a['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(e['source_sha256'],GAME_SHA256)
        self.assertEqual(a['game_sha256'],GAME_SHA256)
        self.assertEqual(a['loader']['decoded_payload_sha256'],PAYLOAD_SHA256)
        self.assertEqual(e['decoded_payload_sha256'],PAYLOAD_SHA256)
        self.assertEqual(e['container_bytes'],128+e['ngame']['payload_bytes']+e['ngame']['extra_bytes'])
        self.assertEqual(a['loader']['status'],0)
        self.assertTrue(a['loader']['exact_host_match'])
        self.assertTrue(a['loader']['tail_preserved_after_header_shift'])

    def test_startup_bss_heap_and_register_contract(self):
        fixed=self.evidence['private_allocator']['fixed_blocks']
        general=0xd3f800+sum((f['payload_bytes']+16)*f['count'] for f in fixed)
        self.assertEqual(hex(general),self.evidence['private_allocator']['general_heap_metadata_base'])
        for case in self.arm['startup_cases']:
            self.assertEqual(case['bss'],{'bytes':0xf0c,'words':963,'all_zero':True,'boundary_guards_preserved':True})
            self.assertEqual(case['heap'],{'base':0xd3f800,'limit':0xe00000,'general_blocks':general})
            self.assertEqual(case['return'],0)
            self.assertTrue(case['sp_and_callee_saved_registers_restored'])

    def test_tail_is_temporary_and_can_overlap_future_bss(self):
        e=self.evidence;bss=next(r for r in e['run_regions'] if r['name']=='startup_zeroed_bss')
        self.assertTrue(all(overlap(r,bss) for r in e['load_regions']))
        payload=e['run_regions'][0]
        self.assertFalse(overlap(payload,bss))
        r=memory_comparison(e,self.profile)
        self.assertEqual(r,e['memory_comparison'])
        self.assertEqual(r['status'],'model_consistent')
        self.assertTrue(r['load_tail_overlaps_future_bss'])

    def test_real_heap_extent_rejected_with_insufficient_allocation(self):
        p=copy.deepcopy(self.profile);p['allocation']={'base':0xa00000,'bytes':0x380000}
        r=memory_comparison(self.evidence,p)
        self.assertEqual(r['status'],'rejected')
        self.assertIn('private_heap_arena: outside supplied allocation',r['errors'])

    def test_observed_runtime_regions_still_must_avoid_audio(self):
        e=copy.deepcopy(self.evidence);r=e['run_regions'][-1]
        r.update(end_exclusive=0xf01000,bytes=0xf01000-r['start'])
        result=memory_comparison(e,self.profile)
        self.assertEqual(result['status'],'rejected')
        self.assertIn('private_heap_arena: overlaps audio_workspace_and_tonelib',result['errors'])

    def test_graphics_and_first_input_exit_sequence(self):
        calls=self.arm['startup_cases'][1]['calls'];calls=[c for c in calls if c['slot']!='0x4']
        self.assertEqual([c['slot'] for c in calls],['0x38','0x90','0x54','0x8c','0x44','0x3c',
            '0xc4','0x10','0xd4','0x124','0x100','0x108','0xe8','0xe8'])
        self.assertEqual(calls[0]['r0_r1'],[0xd68d20,320*240*2])
        self.assertEqual(calls[1]['r0_r1'],[320,240])
        self.assertEqual(calls[2]['input_u16'],[0,0,320,240])
        self.assertEqual(calls[8]['r0_r1'],[3,0xd33820])
        self.assertEqual(calls[10]['injected_keys'],0x80000)
        self.assertEqual(calls[10]['first_word_after'],calls[10]['first_word_before'])

    def test_working_game_resources_fit_observed_wave_contract(self):
        r=self.evidence['resources']
        self.assertEqual(r['entries'],21);self.assertEqual(r['counts'],{'wave':12,'midi':9,'other':0})
        self.assertEqual(r['wave_formats'],[{'format_tag':1,'channels':1,'rate_hz':8000,'bits':8}])
        self.assertFalse(r['assets_exported'])

    def test_foreign_sample_and_private_exports_inside_checkout_rejected(self):
        with self.assertRaises(ValueError):decode(b'NGame: unknown sample')
        with self.assertRaises(ValueError):private_directory(REPO/'research/private_analysis')
        with self.assertRaises(ValueError):private_directory(REPO)

    @unittest.skipUnless(os.environ.get('SPMP_CONTROL_GAME'),'exact private control game not provided')
    def test_optional_exact_game(self):
        result=inspect(Path(os.environ['SPMP_CONTROL_GAME']).read_bytes())
        expected={k:v for k,v in self.evidence.items() if k!='memory_comparison'}
        self.assertEqual(result,expected)

    @unittest.skipUnless(os.environ.get('SPMP_CONTROL_GAME') and os.environ.get('SPMP_MMI_RAP'),
                         'exact private control game and RAP not provided')
    def test_optional_actual_arm_loader_and_startup(self):
        from emulate_control_game import compare
        self.assertEqual(compare(Path(os.environ['SPMP_MMI_RAP']),Path(os.environ['SPMP_CONTROL_GAME'])),self.arm)

if __name__=='__main__':unittest.main()
