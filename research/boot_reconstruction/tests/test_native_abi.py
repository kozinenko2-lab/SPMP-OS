"""Public regressions against stored original-ARM / mocked-backend observations.
Does not require Unicorn, OEM binaries or hardware.
"""
import json,sys,unittest
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
import native_abi_models as model
from audit_boot import H

class NativeABITests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.evidence=json.loads((BASE/'evidence/native_abi_arm_emulation.json').read_text())

    def cases(self,*groups):
        return [c for c in self.evidence['cases'] if c['group'] in groups]

    def test_provenance_and_coverage(self):
        self.assertEqual(self.evidence['source_sha256'],H['8000_MMI.RAP'])
        self.assertEqual(self.evidence['case_count'],len(self.evidence['cases']))
        self.assertEqual(self.evidence['case_count'],99)
        self.assertTrue(all(c['host_model_matches'] for c in self.evidence['cases']))
        self.assertEqual(len(self.cases('file_read','file_write','file_close','file_seek')),26)

    def test_file_boundaries_and_short_transfers(self):
        for c in self.cases('file_read','file_write','file_close','file_seek'):
            i=c['inputs']
            with self.subTest(operation=c['group'],**i):
                self.assertEqual(model.file_result(c['group'][5:],i['fd'],i['count'],i['backend_result']),c['original_arm'])

    def test_open_fd_zero_and_stat_failure(self):
        for c in self.cases('file_open'):
            self.assertEqual(model.open_result(c['inputs']['backend_fd'],c['inputs']['stat_result']),c['original_arm'])

    def test_rgb_quantization(self):
        for c in self.cases('graphics_set_fg'):
            self.assertEqual(model.rgb565(c['inputs']['rgb']),c['original_arm']['rgb565'])
        for c in self.cases('graphics_get_fg'):
            self.assertEqual(model.rgb888_truncated(c['inputs']['rgb565']),c['original_arm']['rgb'])

    def test_query_image_three_argument_contract(self):
        for c in self.cases('graphics_query_image'):
            i=c['inputs'];self.assertEqual(model.image_query(i['image_id'],i['field'],i['active']),c['original_arm'])

    def test_framebuffer_dimensions_and_clear_extent(self):
        for c in self.cases('graphics_set_framebuffer'):
            i=c['inputs'];r=c['original_arm']
            self.assertEqual(model.framebuffer_request(i['requested_width'],i['requested_height']),r['status'])
            # Clear extent uses backend dimensions, even when request says 0x0.
            self.assertEqual(r['bytes_cleared'],0 if r['status'] else i['driver_width']*i['driver_height']*2)
        r=self.cases('graphics_get_framebuffer_dimensions')[0]['original_arm']
        self.assertEqual(r,{'status':0,'output_u16':[320,240]})

    def test_cooked_keys_priority_and_side_effects(self):
        for c in self.cases('keys'):
            i=c['inputs']
            with self.subTest(**i):
                self.assertEqual(model.cooked_keys(i['kind'],i['raw'],i['volume'],i['volume_gate']),c['original_arm'])

    def test_distinct_memory_pools(self):
        for group,fn in [('game_pool_alloc',model.pool_allocate),('general_alloc',model.general_allocate)]:
            for c in self.cases(group):
                self.assertEqual(fn(c['inputs']['bytes'],c['inputs']['backend_result']),c['original_arm'])
        self.assertEqual(model.pool_allocate(1,0x1002000)['result'],0)
        self.assertEqual(model.general_allocate(1,0x1002000)['result'],0x1002000)

    def test_export_plan_and_artifacts(self):
        plan=json.loads((BASE/'ghidra_targets.json').read_text())
        summary=json.loads((BASE/'decompiled/export_summary.json').read_text())
        self.assertEqual(sum(x['successful_targets'] for x in summary['programs']),284)
        counts={p['program']:p['successful_targets'] for p in summary['programs']}
        for program in plan['programs']:
            self.assertEqual(len(program['targets']),counts[program['id']])
            for t in program['targets']:
                stem=BASE/'decompiled'/program['id']/t['label']
                self.assertTrue(stem.with_suffix('.c').is_file(),t['label'])
                self.assertTrue(stem.with_suffix('.asm').is_file(),t['label'])

if __name__=='__main__':unittest.main()
