"""Gambatte integration and RAM-residency regression checks."""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]
EXE=ROOT/'build/spmp-os-host'

class Stage9GB(unittest.TestCase):
    def test_gambatte_lifecycle_harness(self):
        r=subprocess.run([str(ROOT/'build/gambatte_harness')], cwd=ROOT,
                         capture_output=True,text=True,timeout=20)
        self.assertEqual(r.returncode,0,r.stderr)
        self.assertIn('opens=2 closes=2',r.stdout)

    def test_gameboy_original_rom_video(self):
        with tempfile.TemporaryDirectory() as tmp:
            shot=Path(tmp)/'frame.ppm'
            r=subprocess.run([str(EXE),'--run-core','gb','--content',
                str(ROOT/'samples/spmp_battery.gb'),'--frames','8',
                '--screenshot',str(shot)],cwd=ROOT,capture_output=True,text=True,timeout=20)
            self.assertEqual(r.returncode,0,r.stderr)
            self.assertIn('Frame callbacks: 8',r.stdout)
            data=shot.read_bytes()
            hdr=b'P6\n160 144\n255\n'
            self.assertTrue(data.startswith(hdr))
            self.assertEqual(len(data),len(hdr)+160*144*3)
            self.assertGreater(len(set(data[len(hdr):])),2)  # Tile checker is rendered

    def test_gbc_original_rom_video(self):
        with tempfile.TemporaryDirectory() as tmp:
            shot=Path(tmp)/'frame.ppm'
            r=subprocess.run([str(EXE),'--run-core','gb','--content',
                str(ROOT/'samples/spmp_color.gbc'),'--frames','4',
                '--screenshot',str(shot)],cwd=ROOT,capture_output=True,text=True,timeout=20)
            self.assertEqual(r.returncode,0,r.stderr)
            self.assertIn('Frame callbacks: 4',r.stdout)
            self.assertTrue(shot.read_bytes().startswith(b'P6\n160 144\n255\n'))

    def test_corrupt_gameboy_refused(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'bad.gb';p.write_bytes(bytes(0x20))
            r=subprocess.run([str(EXE),'--run-core','gb','--content',str(p),
                              '--frames','1'],cwd=ROOT,capture_output=True,text=True,timeout=20)
            self.assertNotEqual(r.returncode,0)

if __name__=='__main__':unittest.main()
