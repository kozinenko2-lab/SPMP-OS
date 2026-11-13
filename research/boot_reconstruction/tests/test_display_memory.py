"""Regressions against original ARM observations and bounded host layout policy."""
import copy,json,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
from audit_boot import H
import display_pcm_models as model
from native_lifecycle_models import fresh_pool_layout
from native_memory_plan import plan

class DisplayMemoryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.evidence=json.loads((BASE/'evidence/display_pcm_arm_emulation.json').read_text())
        cls.profile=json.loads((BASE/'examples/native_memory_profile.json').read_text())
    def cases(self,group):return [c for c in self.evidence['cases'] if c['group']==group]
    def profile_copy(self):return copy.deepcopy(self.profile)

    def test_original_provenance_and_all_typed_slots(self):
        self.assertEqual(self.evidence['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(self.evidence['case_count'],len(self.evidence['cases']))
        self.assertEqual(self.evidence['case_count'],53)
        static=json.loads((BASE/'evidence/stage6_verified.json').read_text())
        self.assertEqual(len({c['slot'] for c in static['contracts']}),28)

    def test_persistent_lcd_allocations_reduce_game_capacity(self):
        actual=self.cases('lcd_pool_before_game')[0]['original_arm']
        expected=fresh_pool_layout([0xb7840,0xb7840,0x25800,0x25800,0x100010])
        self.assertEqual(actual['allocation'],{'base':expected['main'],'bytes':expected['requested']})
        self.assertEqual([f['base'] for f in actual['framebuffers']],expected['frames'])
        self.assertEqual([f['base'] for f in actual['lcd_buffers']],[a['address'] for a in expected['allocations'][:4]])
        self.assertEqual(actual['oem_stack_allocation'],{'base':expected['allocations'][4]['address'],'bytes':0x100010})
        self.assertTrue(actual['remaining_pool_restored'])
        self.assertLess(expected['capacity'],fresh_pool_layout()['capacity'])

    def test_lcd_swap_copy_and_wait(self):
        for c in self.cases('lcd_paint'):
            i=c['inputs'];r=c['original_arm']
            expected=model.lcd_swap(0x1004000,0x1004000 if i['same'] else 0x1044000,i['update'])
            with self.subTest(**i):
                self.assertEqual({k:r[k] for k in expected},expected)
                self.assertEqual(r['copied_bytes'],0 if i['same'] else 320*240*2)
                self.assertEqual(r['waited'],not i['same'] and bool(i['sync']))

    def test_graphics_callback_selection_and_configure(self):
        direct,virtual=self.cases('lcd_init_graph')
        self.assertFalse(direct['original_arm']['virtual']);self.assertTrue(virtual['original_arm']['virtual'])
        for c in [direct,virtual]:self.assertEqual(c['original_arm']['configured_context'],[320,240])
        self.assertEqual(direct['original_arm']['render_buffer'],0x1004000)
        self.assertEqual(virtual['original_arm']['render_buffer'],0x1044000)

    def test_graphics_wait_and_fill_timeout_hidden_by_wrapper(self):
        for c in self.cases('graphics_wait'):
            ready=(c['inputs']['status_word']&0x3b)==0x10;r=c['original_arm']
            self.assertEqual(r,{'return':0 if ready else 1,'polls':1 if ready else 300,'delays':0 if ready else 300})
        for c in self.cases('fill_rect'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(r['status'],model.fill_rect_status(i['x'],i['y'],i['width'],i['height']))
            if r['blitter_called']:self.assertEqual(r['coordinates_forwarded'],[i['x']&0xffffffff,i['y']&0xffffffff,i['width'],i['height']])
        busy=next(c['original_arm'] for c in self.cases('fill_rect') if c['inputs']['gpu_busy'])
        self.assertEqual(busy['status'],0);self.assertEqual(busy['delay_calls'],300)

    def test_dma_address_arithmetic_and_channel_scope(self):
        for c in self.cases('pcm_dma'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(model.dma_program(i['channel'],i['first'],i['first_bytes'],i['second'],i['second_bytes'],i['control']),r['writes'])
            self.assertEqual(r['allocator_supported_channel'],i['channel']<2)

    def test_pcm_queue_strict_space_predicate(self):
        for c in self.cases('pcm_reserve'):
            i=c['inputs'];self.assertEqual(model.reserve_try(i['request'],i['read'],i['write'],i['capacity'],i['threshold'],i['base']),c['original_arm']['return'])

    def test_pcm_zero_odd_and_failed_start(self):
        for c in self.cases('pcm_submit'):
            i=c['inputs'];r=c['original_arm']
            if not i['available']:
                self.assertEqual(r['copied_bytes'],0)
                self.assertEqual(r['commits'],[0] if i['started']==0 else [])
                self.assertEqual(r['started_after'],1)
            elif i['channels']==1:
                self.assertEqual(r['copied_bytes'],max(1,(i['count']+1)//2)*4)
                self.assertEqual(r['commits'],[i['count']*2])
        zero=next(c for c in self.cases('pcm_submit') if c['inputs']['count']==0)
        self.assertEqual(zero['original_arm']['prefix_hex'],'00010001')

    def test_thread_delete_requires_exited_state(self):
        for c in self.cases('thread_delete'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(r['return'],int(i['state']==16 or i['kill_exits']))
            self.assertEqual(r['destructed'],bool(r['return']))

    def test_memory_example_is_only_a_consistent_model(self):
        r=plan(self.profile_copy())
        self.assertEqual(r['status'],'model_consistent');self.assertEqual(r['errors'],[])
        self.assertIn('runtime allocation ownership',r['unresolved'])
        self.assertNotIn('OEM thread stack extent',r['unresolved'])
        stored=json.loads((BASE/'evidence/native_memory_example_plan.json').read_text())
        self.assertEqual(stored,r)

    def test_load_phase_header_can_cross_audio_before_runtime_payload(self):
        p=self.profile_copy();p['sizes'].update(payload_bytes=0x500000-64,bss_bytes=0,heap_bytes=0,private_stack_reserve_bytes=0)
        r=plan(p);self.assertEqual(r['status'],'rejected')
        self.assertIn('temporary_container_read: overlaps audio_workspace_and_tonelib',r['errors'])
        self.assertNotIn('decoded_payload_span: overlaps audio_workspace_and_tonelib',r['errors'])

    def test_heap_stack_collision_and_strict_loader_bound(self):
        p=self.profile_copy();p['sizes']['heap_bytes']=0x500000
        self.assertEqual(plan(p)['status'],'rejected')
        p=self.profile_copy();p['sizes'].update(payload_bytes=0x10000,bss_bytes=0,heap_bytes=0,private_stack_reserve_bytes=0)
        p['allocation']={'base':0xa00000,'bytes':0x10080}
        self.assertIn('container violates OEM strict file_size < capacity predicate',plan(p)['errors'])

    def test_failed_allocation_or_lcd_overlap_rejected(self):
        p=self.profile_copy();p['allocation']['base']=0
        self.assertEqual(plan(p)['status'],'rejected')
        p=self.profile_copy();p['lcd_buffers'][0]['base']=p['allocation']['base']
        self.assertIn('supplied_lcd_buffer_0: overlaps game allocation',plan(p)['errors'])
        p=self.profile_copy();p['framebuffers'][1]=p['framebuffers'][0].copy()
        self.assertIn('framebuffer observations overlap',plan(p)['errors'])
        p=self.profile_copy();p['oem_stack']={'base':0xa10000,'bytes':0x4000}
        self.assertIn('game allocation overlaps supplied OEM thread stack',plan(p)['errors'])

    def test_foreign_image_and_unsigned_arithmetic(self):
        p=self.profile_copy();p['source_sha256']='0'*64
        with self.assertRaises(ValueError):plan(p)
        p=self.profile_copy();p['allocation']={'base':0xfffffff0,'bytes':0x1000}
        with self.assertRaises(ValueError):plan(p)
        p=self.profile_copy();p['sizes']['heap_bytes']=-1
        with self.assertRaises(ValueError):plan(p)
        p=self.profile_copy();p['native_kind']=8
        with self.assertRaises(ValueError):plan(p)

if __name__=='__main__':unittest.main()
