import sys
import unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_mmi_threads import Firmware, analyze, produce_records, CREATE_ADDR, KNOWN_SHA256

RAP = Path('/mnt/data/8000_MMI.RAP')

class TestStage54(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fw=Firmware(RAP.read_bytes())
        cls.result=analyze(cls.fw)

    def test_source_sha(self):self.assertEqual(self.fw.original_sha256,KNOWN_SHA256)
    def test_calls_count(self):self.assertEqual(len(self.fw.call_sites(CREATE_ADDR)),35)
    def test_resolved_names(self):self.assertEqual(self.result['resolved_names'],34)
    def test_mmi_gui(self):
        r=self.result['threads'][0]
        self.assertEqual((r['callsite'],r['thread_name'],r['entrypoint']),('0x00280810','Demo GUI','0x003BE5B0'))
    def test_sysinit(self):self.assertTrue(any(r['thread_name']=='Systen initial Thread' and r['entrypoint']=='0x00447540' for r in self.result['threads']))
    def test_storage(self):self.assertTrue(any(r['thread_name']=='stor_thr' and r['entrypoint']=='0x00448A7C' for r in self.result['threads']))
    def test_game_thread(self):self.assertTrue(any(r['thread_name']=='Game thread' and r['entrypoint']=='0x00460AB0' for r in self.result['threads']))
    def test_sd(self):self.assertTrue(any(r['thread_name']=='Card_Isp_SD_thread' for r in self.result['threads']))
    def test_dynamic_thread_remains_unresolved(self):
        self.assertEqual(self.result['threads'][-1]['callsite'],'0x004CB3C8')
        self.assertEqual(self.result['threads'][-1]['thread_name'],'')
    def test_priority_stack(self):self.assertTrue(self.result['confirmed_checks']['initial_thread_stack_bytes_0x4000'])
    def test_boot_four_targets(self):
        self.assertEqual(self.result['boot_direct_calls']['0x00280150'],'0x00297CE8')
        self.assertEqual(self.result['boot_direct_calls']['0x00297CFC'],'0x002807C8')
    def test_pc_literal(self):
        self.assertEqual(self.fw.pc_literal(0x28080c,3)['value'],'0x4cfb44')
    def test_bad_format(self):
        self.assertIsNone(self.fw.pc_literal(0x280810,1))
        self.assertIsNone(self.fw.bl(0x280800))
    def test_changed_source_must_fail(self):
        b=bytearray(RAP.read_bytes());b[0x200]^=0x40
        with self.assertRaisesRegex(ValueError,'Unexpected OEM RAP SHA256'):Firmware(bytes(b))
    def test_malformed_source_must_fail(self):
        with self.assertRaises(ValueError):Firmware(b'foo',verify=False)
    def test_exact_thread_entry_unique(self):
        self.assertEqual(len(set(r['callsite'] for r in self.result['threads'])),35)
        self.assertTrue(all(r['entrypoint'] for r in self.result['threads']))
    def test_all_thread_entries_have_arm_prologue(self):
        self.assertTrue(all(self.fw.u32(int(r['entrypoint'],16)) == 0xe1a0c00d for r in self.result['threads']))

    def test_gui_direct_dispatch(self):
        self.assertEqual(self.fw.bl(0x3be5cc),0x447640)
        self.assertEqual(self.fw.bl(0x4476b0),0x4475c0)
        self.assertEqual(self.fw.bl(0x447610),CREATE_ADDR)

if __name__ == '__main__':unittest.main()
