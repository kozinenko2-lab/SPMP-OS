"""Stage6 emulator integration regressions. Synthetic public-domain-free test bytes."""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / 'build/spmp-os-host'

class Stage6Tests(unittest.TestCase):
    def test_registered_native_cores_and_unported_stock(self):
        r = subprocess.run([str(EXE),'--cores'],capture_output=True,text=True)
        self.assertEqual(r.returncode,0,r.stderr)
        self.assertRegex(r.stdout,r'pcengine\s+HOST_TESTED')
        self.assertRegex(r.stdout,r'atari2600\s+HOST_TESTED')
        self.assertRegex(r.stdout,r'gb\s+HOST_TESTED')
        self.assertRegex(r.stdout,r'snes\s+HOST_TESTED')
        for pending in ('gba',):
            self.assertRegex(r.stdout,rf'{pending}\s+RESEARCH')

    def test_atari_synthetic_rom_captures_frame(self):
        with tempfile.TemporaryDirectory() as td:
            shot = Path(td) / 'a.ppm'
            result = subprocess.run([str(EXE),'--run-core','atari2600','--content',
                                    str(ROOT/'samples/atari6507_smoke.a26'), '--frames','2',
                                    '--screenshot',str(shot)],capture_output=True,text=True,timeout=12)
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertIn('Frame callbacks: 2',result.stdout)
            data = shot.read_bytes()
            self.assertTrue(data.startswith(b'P6\n320 240\n255\n'))
            self.assertEqual(len(data),len(b'P6\n320 240\n255\n')+320*240*3)

    def test_too_small_rom_rejected(self):
        with tempfile.TemporaryDirectory() as td:
            f=Path(td)/'bad.a26';f.write_bytes(bytes(100))
            r=subprocess.run([str(EXE),'--run-core','atari2600','--content',str(f),
                              '--frames','1'],capture_output=True,text=True,timeout=6)
            self.assertNotEqual(r.returncode,0)

    def test_independent_harnesses(self):
        for name,msg in [('atari_harness','Atari 2600:'),
                         ('pcengine_harness','PC Engine native core:'),
                         ('pce_save_harness','per-ROM BRAM:')]:
            f=ROOT/'build'/name
            self.assertTrue(f.is_file(),f'{f} not built: use make test-cores')
            p=subprocess.run([str(f)],capture_output=True,text=True,timeout=15)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertIn(msg,p.stdout)

if __name__=='__main__':unittest.main()
