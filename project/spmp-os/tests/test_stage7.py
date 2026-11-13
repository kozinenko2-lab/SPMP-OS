"""Stage7 NES NROM/UxROM/CNROM limited baseline regressions."""
from pathlib import Path
import subprocess
import tempfile
import unittest

BASE = Path(__file__).resolve().parents[1]
EXE = BASE / 'build/spmp-os-host'
SAMPLES = BASE / 'samples'


def run(*args):
    return subprocess.run([str(EXE), *map(str, args)], cwd=BASE,
                          text=True, capture_output=True, timeout=15)


class NESIntegration(unittest.TestCase):
    def test_registry_separates_nes_and_gb_from_unported_platforms(self):
        out=run('--cores')
        self.assertEqual(out.returncode,0,out.stderr)
        self.assertRegex(out.stdout,r'nes\s+HOST_TESTED')
        self.assertRegex(out.stdout,r'gb\s+HOST_TESTED')
        self.assertRegex(out.stdout,r'snes\s+HOST_TESTED')
        for core in ('gba',):
            self.assertRegex(out.stdout,rf'{core}\s+RESEARCH')

    def test_three_mapper_smoke_roms(self):
        for sample in ('nes_cpu_ppu_smoke.nes','nes_uxrom_smoke.nes','nes_cnrom_smoke.nes'):
            with self.subTest(sample=sample), tempfile.TemporaryDirectory() as td:
                target=Path(td)/'frame.ppm'
                p=run('--run-core','nes-baseline','--content',SAMPLES/sample,
                      '--frames','3','--screenshot',target)
                self.assertEqual(p.returncode,0,p.stderr)
                self.assertIn('Frame callbacks: 3',p.stdout)
                data=target.read_bytes()
                magic=b'P6\n256 240\n255\n'
                self.assertEqual(len(data),len(magic)+256*240*3)
                self.assertTrue(data.startswith(magic))
                if 'smoke.nes' in sample and sample!='nes_uxrom_smoke.nes':
                    self.assertNotEqual(data[len(magic):len(magic)+3],
                                        data[len(magic)+3:len(magic)+6])

    def test_bad_rom_rejected_without_a_frame(self):
        with tempfile.TemporaryDirectory() as td:
            path=Path(td)/'bad.nes'
            for data in (b'',b'NES\x1a',bytes(512),
                         b'NES\x1a'+bytes([1,0,0x40,0])+bytes(8)+bytes(16384),
                         b'NES\x1a'+bytes([1,0,0x00,0x08])+bytes(8)+bytes(16384)):
                with self.subTest(length=len(data),preview=data[:8]):
                    path.write_bytes(data)
                    p=run('--run-core','nes-baseline','--content',path,'--frames','1')
                    self.assertNotEqual(p.returncode,0)
                    self.assertIn('not compiled/available',p.stderr)

    def test_independent_cpu_ppu_and_save_harness(self):
        exe=BASE/'build/nes_harness'
        self.assertTrue(exe.exists())
        p=subprocess.run([str(exe)],cwd=BASE,capture_output=True,text=True,timeout=15)
        self.assertEqual(p.returncode,0,p.stderr)
        self.assertIn('mapper',p.stdout)

    def test_signature_identification(self):
        path=SAMPLES/'nes_cpu_ppu_smoke.nes'
        p=run('--resolve',path)
        self.assertEqual(p.returncode,0,p.stderr)
        self.assertIn('nes [HOST_TESTED]',p.stdout)

if __name__ == '__main__':unittest.main()
