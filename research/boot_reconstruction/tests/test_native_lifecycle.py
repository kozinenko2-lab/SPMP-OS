"""Public regressions from RAM-only original ARM observations, no firmware needed."""
import hashlib,json,struct,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from audit_boot import H
import native_lifecycle_models as model

class NativeLifecycleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.evidence=json.loads((BASE/'evidence/native_lifecycle_arm_emulation.json').read_text())
        cls.static=json.loads((BASE/'evidence/stage5_verified.json').read_text())
    def cases(self,group):return [c for c in self.evidence['cases'] if c['group']==group]

    def test_original_provenance_and_constructor_route(self):
        self.assertEqual(self.evidence['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(self.static['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(self.evidence['case_count'],119)
        self.assertEqual(len(self.evidence['cases']),119)
        r=self.cases('constructor_pool')[0]['original_arm']
        self.assertEqual(r,{'table_entries':21,'pool_initializer':'0x2e3908','pool_is_last':True,'pool_created':True})
        self.assertEqual(self.static['constructors']['invoked_reverse'][-1],'0x2e3908')

    def test_original_allocator_layout_and_restoration(self):
        for c in self.cases('fresh_pool'):
            expected=model.fresh_pool_layout(c['inputs']['prefix_requests'])
            self.assertEqual(c['original_arm'],{**expected,'restored':True})
        fresh=self.cases('fresh_pool')[0]['original_arm']
        self.assertEqual(fresh['main'],0x9fff10)
        self.assertEqual(fresh['frames'],[0x1fda7f0,0x1fb4fe0])
        self.assertEqual(fresh['capacity'],0x15b4fd4)

    def test_failed_reservation_does_not_abort_loader(self):
        for c in self.cases('reservation_fault'):
            i=c['inputs'];self.assertEqual(model.reservation(i['base'],i['maximum'],i['previous_capacity']),c['original_arm'])
        null=next(c for c in self.cases('reservation_fault') if c['inputs']['base']==0)
        self.assertEqual(null['original_arm']['capacity'],0xff700000)
        self.assertTrue(null['original_arm']['loader_reached'])

    def test_loader_result_messages_and_cleanup(self):
        for c in self.cases('game_thread'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(r['messages'],model.loader_messages(i['loader_result']))
            self.assertEqual(r['return'],i['loader_result']&0xffffffff)
            self.assertTrue(r['freed_and_cleared'])
            self.assertEqual(r['capacity_after'],r['at_loader']['capacity'])

    def test_resource_pause_resume_stop_and_early_return(self):
        for c in self.cases('resource_control'):
            i=c['inputs']
            with self.subTest(operation=i['operation'],mask=i['mask'],features=i['features']):
                self.assertEqual(model.resource_control(i['operation'],i['mask'],i['features'],i['state']),c['original_arm'])
        early=next(c for c in self.cases('resource_control') if c['inputs']['operation']=='stop' and c['inputs']['mask']==3 and c['inputs']['state']['midi_state']==3)
        self.assertEqual(early['original_arm']['wave_state'],11)
        self.assertNotEqual(early['original_arm']['wave_base'],0)

    def test_lookup_kind_is_not_found_status(self):
        for c in self.cases('resource_get'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(model.resource_kind(i['name']),r['kind'])
            self.assertEqual(r['output_written'],i['name']=='tone.wav')
            if not r['output_written']:self.assertEqual(r['output'],[0xfeedbeef,0xdeadbeef])
        for c in self.cases('resource_get_invalid'):
            self.assertEqual(c['original_arm'],{'kind':0,'features_after':0,'output_written':False})

    def test_wave_acceptance_and_repeats(self):
        for c in self.cases('resource_play_wave'):
            i=c['inputs']
            self.assertEqual(model.wave_play_result(i['rate'],i['bits'],i['repeats'],i['features'],not i['null_descriptor']),c['original_arm'])
        for c in self.cases('resource_play_midi'):
            r=c['original_arm'];self.assertEqual(r['status'],1)
            self.assertEqual(r['repeat_count'],model.repeat_count(0))
            self.assertEqual(r['decoded_called'],not c['inputs']['cached'])
            self.assertEqual(r['remaining_bytes'],8)

    def test_workspace_cached_path(self):
        for c in self.cases('audio_workspace'):
            i=c['inputs'];self.assertEqual(model.audio_workspace(i['kind'],i['cached'],i['backend_result']),c['original_arm'])
        cached=next(c for c in self.cases('audio_workspace') if c['inputs']['cached'])
        self.assertNotEqual(cached['original_arm']['result'],cached['inputs']['cached'])

    def test_resource_init_workspace_and_ignored_read_error(self):
        first=self.cases('resource_init')[0]['original_arm']
        self.assertEqual(first,{'cleared_bytes':0x100000,'tonelib_destination':0x1000000,'initialized_after':1})
        upper=first['tonelib_destination']+model.TONELIB_CAPACITY
        self.assertEqual(upper,0x1032000)
        fresh=model.fresh_pool_layout()
        self.assertGreater(upper,model.BIN_ENTRY)
        self.assertLess(upper,model.BIN_ENTRY+fresh['capacity'])

    def test_worker_samples_and_observed_partial_flush(self):
        for c in self.cases('audio_worker'):
            case=c['inputs']['case'];r=c['original_arm']
            w=(bytes([0,128,255])*171)[:512] if case in ('wave','mixed') else bytes([0,128,255]) if case=='partial' else b''
            m=([-32768,32767,1,-1]*128) if case in ('midi','mixed') else []
            samples=model.mixed_samples(w,m)
            payload=struct.pack('<'+'h'*len(samples),*samples)
            if case=='partial':payload=model.observed_partial_flush(payload+b'\x6a'*(1024-len(payload)),len(payload))
            with self.subTest(case=case):
                self.assertEqual(hashlib.sha256(payload).hexdigest(),r['sha256'])
                self.assertEqual(len(payload),r['submitted_bytes'])
        partial=next(c['original_arm'] for c in self.cases('audio_worker') if c['inputs']['case']=='partial')
        self.assertEqual(partial['first_samples'],[0]*8)
        self.assertEqual(partial['last_bytes_hex'],'00006a6a6a6a6a6a')

    def test_exit_mode_requires_returning_backends(self):
        for c in self.cases('exit_control'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(model.exit_calls(i['mode'],i['game_handle'],i['seq_handle'],i['requests']),r['calls'])
            self.assertEqual(r['handle_after'],i['game_handle'] if i['mode']&0x80000000 else 0)
        for c in self.cases('sound_stop'):
            r=c['original_arm'];self.assertTrue(r['pointers_cleared'])
            self.assertEqual([call[0] for call in r['calls']],['pool_free','delete','pcm_stop','general_free','delay'])

    def test_ordinary_bin_return_ignores_application_result(self):
        for c in self.cases('loader_return'):
            i=c['inputs'];r=c['original_arm'];self.assertTrue(r['pool_restored'])
            if i['mocked_load_status']:
                self.assertEqual(r['return'],0xfffffffe);self.assertEqual(r['entry'],[])
            else:
                self.assertEqual(r['return'],0);self.assertEqual(r['entry'][0]['r0'],0x5efb74)
                self.assertEqual(r['entry'][0]['callbacks'],[0x46190c,0x46195c,0x4619ac])
                self.assertEqual(r['audio_threads_created'],1)
                self.assertEqual([x['id'] for x in r['commands']],[0xd06,0xd08,0xd07,0xd07])

if __name__=='__main__':unittest.main()
