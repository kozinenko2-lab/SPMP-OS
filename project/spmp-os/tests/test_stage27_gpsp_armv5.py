import json, pathlib, subprocess, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'build/stage27/gpsp_armv5'
class GPspARMv5(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        assert (OUT/'audit.json').exists(), 'run make stage27-arm first'
        cls.a=json.loads((OUT/'audit.json').read_text())
    def test_selected_interpreter_sources_compiled(self):
        self.assertEqual((self.a['passed'],self.a['total']),(15,15))
        self.assertTrue(self.a['linked'])
        self.assertTrue(self.a['runtime_ready'])
        self.assertIn('cpu.cc',self.a['files'])
        self.assertNotIn('cpu_threaded.c',self.a['files'])
    def test_elf_armv5_eabi(self):
        elf=OUT/'gpsp_interpreter_core.o'
        p=subprocess.run(['readelf','-h',str(elf)],capture_output=True,text=True,check=True)
        self.assertIn('Machine:                           ARM',p.stdout)
        self.assertIn('REL (Relocatable file)',p.stdout)
        attrs=subprocess.run(['readelf','-A',str(elf)],capture_output=True,text=True,check=True)
        self.assertIn('v5TE',attrs.stdout)
    def test_heap_symbols_resolved_locally(self):
        missing=self.a['unresolved_symbols']
        for name in ('malloc','calloc','realloc','free','open_gba_bios_rom','gpsp_heap_malloc','gpsp_heap_free','strlcpy_retro__'):
            self.assertNotIn(name,missing)
        self.assertNotIn('filestream_open',missing)  # Stage 28 HAL now resolves streaming
        self.assertNotIn('__aeabi_dmul',missing)  # Stage33 softfloat EABI
    def test_clean_bios_matches_distribution(self):
        bios=ROOT/'third_party/gpsp/bios/open_gba_bios.bin'
        self.assertEqual(bios.stat().st_size,16384)
if __name__=='__main__':unittest.main()
