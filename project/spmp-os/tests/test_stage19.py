"""Stage 19 - real PicoDrive core, no proprietary ROMs or real hardware implied."""
from pathlib import Path
import json
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

class Stage19PicoDrive(unittest.TestCase):
    def test_actual_68000_and_save_state(self):
        rom = ROOT / 'samples/spmp_md_68000_test.md'
        exe = ROOT / 'build/stage19_md_harness'
        p = subprocess.run([str(exe), str(rom)], cwd=ROOT, text=True,
                           capture_output=True, timeout=25, errors='replace')
        self.assertEqual(p.returncode, 0, p.stderr)
        self.assertIn('M68K executed', p.stdout)
        self.assertIn('save/restore and reload OK', p.stdout)
        self.assertIn('00 5a', p.stderr)

    def test_smd_copier_file(self):
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / 'frame.ppm'
            p = subprocess.run([str(ROOT / 'build/spmp-os-host'), '--run-core',
                                'megadrive', '--content',
                                'samples/spmp_md_68000_test.smd', '--frames', '3',
                                '--screenshot', str(out)], cwd=ROOT,
                               text=True, capture_output=True, timeout=20,
                               errors='replace')
            self.assertEqual(p.returncode, 0, p.stderr)
            self.assertIn('Frame callbacks: 3', p.stdout)
            frame = out.read_bytes()
            head = b'P6\n320 224\n255\n'
            self.assertTrue(frame.startswith(head))
            self.assertEqual(len(frame), len(head) + 320 * 224 * 3)

    def test_elf32_armv5_partial_core_only(self):
        obj = ROOT / 'build/spmp_picodrive_stage19_armv5_unresolved.o'
        self.assertTrue(obj.exists())
        p = subprocess.run(['readelf', '-h', str(obj)], capture_output=True,
                           text=True, check=True)
        self.assertIn('ELF32', p.stdout)
        self.assertIn('ARM', p.stdout)
        self.assertIn('REL (Relocatable file)', p.stdout)
        p = subprocess.run(['nm', '-u', str(obj)], text=True, capture_output=True,
                           check=True)
        self.assertIn('U ', p.stdout, 'Partial core must NOT be declared linked')

    def test_manifest_not_flashable_and_psx_unresolved(self):
        meta = json.loads((ROOT / 'firmware/module_catalog.json').read_text())
        self.assertFalse(meta['firmware_flashable'])
        md = next(c for c in meta['modules'] if c['id'] == 'megadrive')
        psx = next(c for c in meta['modules'] if c['id'] == 'ps1')
        self.assertTrue(md['core_source_in_package'])
        self.assertFalse(md['native_firmware_available'])
        self.assertFalse(psx['native_firmware_available'])

if __name__ == '__main__':
    unittest.main()
