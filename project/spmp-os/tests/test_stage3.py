import pathlib
import re
import subprocess
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
EXE = ROOT / 'build' / 'spmp-os-host'


def run(*args):
    return subprocess.run([str(EXE), *args], text=True, capture_output=True)


class BuiltinCoreTests(unittest.TestCase):
    def test_registry_is_explicit_about_unavailable_cores(self):
        p = run('--cores')
        self.assertEqual(p.returncode, 0, p.stderr)
        self.assertRegex(p.stdout, r'diagnostic\s+HOST_TESTED')
        self.assertRegex(p.stdout, r'doom\s+SOURCE_ONLY')
        self.assertRegex(p.stdout, r'ps1\s+RESEARCH')

    def test_resolution(self):
        for file, expected in [('DOOM.WAD', 'doom'), ('FOLDER/TITLE.PCE', 'pcengine'),
                               ('Game.Z80', 'spectrum'), ('TEST.A26', 'atari2600'),
                               ('TEST.D64', 'c64'), ('Q1/PAK0.PAK', 'quake')]:
            with self.subTest(file=file):
                p = run('--resolve', file)
                self.assertEqual(p.returncode, 0)
                self.assertIn('UNIQUE', p.stdout)
                self.assertIn(expected, p.stdout)

    def test_collision_is_not_autolaunched(self):
        for f in ['foo.DSK', 'foo.BIN', 'foo.TAP', 'foo.SNA']:
            with self.subTest(file=f):
                p = run('--resolve', f)
                self.assertIn('AMBIGUOUS', p.stdout)

    def test_unknown(self):
        p = run('--resolve', 'readme.txt')
        self.assertIn('UNKNOWN', p.stdout)

    def test_not_yet_linked_emulator_cannot_launch(self):
        p = run('--run-core', 'doom', '--frames', '2')
        self.assertNotEqual(p.returncode, 0)
        self.assertIn('not compiled', p.stderr)

    def test_builtin_core_runs_through_frame_callbacks(self):
        out = ROOT/'build'/'stage3_unittest.ppm'
        try:
            p=run('--run-core','diagnostic','--frames','4','--screenshot',str(out))
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertIn('Frame callbacks: 4',p.stdout)
            data=out.read_bytes()
            self.assertTrue(data.startswith(b'P6\n320 240\n255\n'))
            self.assertEqual(len(data), len(b'P6\n320 240\n255\n')+320*240*3)
        finally:
            out.unlink(missing_ok=True)

    def test_bad_frames(self):
        p=run('--run-core','diagnostic','--frames','0')
        self.assertNotEqual(p.returncode,0)

if __name__ == '__main__': unittest.main()

class BrowserRoutingTests(unittest.TestCase):
    def test_file_browser_informs_user_before_launch(self):
        import tempfile
        with tempfile.TemporaryDirectory() as temp:
            folder=pathlib.Path(temp)
            (folder/'doom.wad').write_bytes(b'IWAD')
            p=subprocess.run([str(EXE),'--root',temp],input='o\nq\n',text=True,capture_output=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertIn('doom: SOURCE ONLY',p.stdout)

    def test_ambiguous_disk_must_ask_for_core(self):
        import tempfile
        with tempfile.TemporaryDirectory() as temp:
            folder=pathlib.Path(temp)
            (folder/'demo.DSK').write_bytes(b'XX')
            p=subprocess.run([str(EXE),'--root',temp],input='o\nq\n',text=True,capture_output=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertIn('SELECT CORE: 2 OPTIONS',p.stdout)
