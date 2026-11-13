"""Stage8: FCEUmm, not the prior synthetic baseline, is the default NES engine."""
from pathlib import Path
import subprocess
import tempfile
import unittest
BASE=Path(__file__).resolve().parents[1]
BIN=BASE/'build/spmp-os-host'

def cmd(*args):
    return subprocess.run([str(BIN),*map(str,args)],cwd=BASE,capture_output=True,text=True,timeout=30)

class Stage8FCEUmm(unittest.TestCase):
    def test_default_core_and_preserved_baseline(self):
        p=cmd('--cores')
        self.assertEqual(p.returncode,0,p.stderr)
        self.assertIn('NES / Famicom (FCEUmm)',p.stdout)
        self.assertIn('nes-baseline',p.stdout)
    def test_three_mappers_stereo_audio_and_save_harness(self):
        p=subprocess.run([str(BASE/'build/fceumm_harness')],cwd=BASE,capture_output=True,text=True,timeout=30)
        self.assertEqual(p.returncode,0,p.stderr)
        self.assertIn('stereo PCM',p.stdout)
    def test_full_fceumm_video_from_real_core(self):
        for rom in ('nes_cpu_ppu_smoke.nes','nes_uxrom_smoke.nes','nes_cnrom_smoke.nes'):
            with self.subTest(rom=rom),tempfile.TemporaryDirectory() as td:
                img=Path(td)/'result.ppm'
                p=cmd('--run-core','nes','--content',BASE/'samples'/rom,'--frames','3','--screenshot',img)
                self.assertEqual(p.returncode,0,p.stderr)
                self.assertIn('Frame callbacks: 3',p.stdout)
                data=img.read_bytes()
                self.assertTrue(data.startswith(b'P6\n256 240\n255\n'))
                self.assertEqual(len(data),len(b'P6\n256 240\n255\n')+256*240*3)
    def test_bad_rom_and_follow_up_recovery(self):
        with tempfile.TemporaryDirectory() as td:
            bad=Path(td)/'broken.nes';bad.write_bytes(b'NES\x1a')
            p=cmd('--run-core','nes','--content',bad,'--frames','1')
            self.assertNotEqual(p.returncode,0)
            p=cmd('--run-core','nes','--content',BASE/'samples/nes_cpu_ppu_smoke.nes','--frames','1')
            self.assertEqual(p.returncode,0,p.stderr)
if __name__=='__main__':unittest.main()
