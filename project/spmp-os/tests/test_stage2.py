import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
BIN = ROOT / 'build/spmp-os-host'

class Stage2Tests(unittest.TestCase):
    def run_bin(self, *args, ok=True):
        p = subprocess.run([str(BIN), *map(str, args)], capture_output=True, text=True, input="q\n")
        if ok:
            self.assertEqual(p.returncode, 0, p.stderr)
        return p

    def test_home_and_theme_palette(self):
        with tempfile.TemporaryDirectory() as td:
            a = pathlib.Path(td) / 'a.ppm'
            b = pathlib.Path(td) / 'b.ppm'
            self.run_bin('--home', '--theme', ROOT/'themes/midnight.ini', '--screenshot', a)
            self.run_bin('--home', '--theme', ROOT/'themes/amber.ini', '--screenshot', b)
            self.assertEqual(a.stat().st_size, 230415)
            self.assertNotEqual(a.read_bytes(), b.read_bytes())

    def test_button_config(self):
        p = self.run_bin('--home', '--buttons', ROOT/'config/buttons.ini')
        self.assertIn('confirm=A', p.stdout)
        self.assertIn('music', p.stdout)
        self.assertIn('planned', p.stdout)

    def test_reject_invalid_config_no_partial_write(self):
        with tempfile.TemporaryDirectory() as td:
            theme=pathlib.Path(td)/'evil.ini'
            theme.write_text('accent=#ff01zz\n')
            self.assertNotEqual(self.run_bin('--home', '--theme',theme,ok=False).returncode,0)
            buttons=pathlib.Path(td)/'evilbuttons.ini'
            buttons.write_text('confirm=A;dangerous\n')
            self.assertNotEqual(self.run_bin('--home', '--buttons',buttons,ok=False).returncode,0)

    def test_detect_by_magic(self):
        cases={'music.mp3':(b'ID3'+b'\0'*16,'MP3'),
               'png.bin':(b'\x89PNG\r\n\x1a\n'+b'\0'*10,'PNG'),
               'video.mp4':(b'\0\0\0\x18ftypisom','MP4-container'),
               'video.avi':(b'RIFF'+b'\0'*4+b'AVI '+b'\0'*8,'AVI-container'),
               'game.bin':(b'NGame1.0'+b'\0'*10,'NGame1.0'),
               'photo.gif':(b'GIF89a'+b'\0'*10,'GIF')}
        with tempfile.TemporaryDirectory() as td:
            for name,(content,expected) in cases.items():
                p=pathlib.Path(td)/name;p.write_bytes(content)
                self.assertIn(expected,self.run_bin('--inspect',p).stdout)

    def test_new_file_classification(self):
        with tempfile.TemporaryDirectory() as td:
            for name in ['a.MP3','b.MP4','c.GIF','d.WAD','e.GG','f.A26']:
                (pathlib.Path(td)/name).write_bytes(b'xx')
            p=self.run_bin('--root',td)
            for marker in ['AUDIO','VIDEO','IMAGE','DATA','SMS/GG','2600']:
                self.assertIn(marker,p.stdout)

if __name__ == '__main__':unittest.main()
