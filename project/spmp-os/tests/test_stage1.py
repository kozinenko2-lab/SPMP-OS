import pathlib
import subprocess
import tempfile
import unittest

ROOT=pathlib.Path(__file__).resolve().parents[1]
BIN=ROOT/'build/spmp-os-host'

class Stage1Tests(unittest.TestCase):
    def test_file_manager_navigation_and_no_execution(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=pathlib.Path(tmp)
            (root/'games').mkdir()
            (root/'games'/'demo.GBA').write_bytes(b'game test')
            (root/'other.NES').write_bytes(b'test')
            (root/'escape').symlink_to('/etc')
            p=subprocess.run([str(BIN),'--root',tmp],input='o\no\nb\nq\n',
                capture_output=True,text=True,check=True)
            self.assertIn('gba: RESEARCH ONLY',p.stdout)
            self.assertIn('other.NES',p.stdout)
            self.assertNotIn('escape',p.stdout)
            self.assertIn('| /games |',p.stdout)
    def test_render_ppm(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=pathlib.Path(tmp)
            (root/'SEGA').mkdir()
            (root/'demo.nes').write_bytes(b'X')
            pp=root/'shot.ppm'
            subprocess.run([str(BIN),'--root',tmp,'--screenshot',str(pp)],
                check=True,capture_output=True)
            b=pp.read_bytes()
            self.assertTrue(b.startswith(b'P6\n320 240\n255\n'))
            self.assertEqual(len(b),len(b'P6\n320 240\n255\n')+320*240*3)

if __name__=='__main__':unittest.main()
