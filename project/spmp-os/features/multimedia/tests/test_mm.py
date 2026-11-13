import io
import math
import pathlib
import struct
import subprocess
import tempfile
import unittest
import wave
from PIL import Image

ROOT = pathlib.Path(__file__).resolve().parents[1]
EXE = ROOT / 'build/mm-host'


def run(*args, ok=True):
    p = subprocess.run([str(EXE), *map(str, args)], capture_output=True, text=True, timeout=10)
    if ok and p.returncode:
        raise AssertionError(f'{args}: {p.returncode}\n{p.stdout}\n{p.stderr}')
    return p


def box(name, data):
    return struct.pack('>I', len(data)+8) + name.encode() + data


class MultimediaTests(unittest.TestCase):
    def test_theme_previews(self):
        with tempfile.TemporaryDirectory() as td:
            hashes = []
            for theme in (ROOT / 'themes').glob('*.ini'):
                out = pathlib.Path(td) / 'home.ppm'
                p = run('--home', '--theme', theme, '--output', out)
                self.assertIn('Theme:', p.stdout)
                blob = out.read_bytes()
                self.assertEqual(len(blob), len(b'P6\n320 240\n255\n')+320*240*3)
                hashes.append(hash(blob))
            self.assertEqual(len(set(hashes)), 4)

    def test_skin_security(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td)/'bad.ini'
            for bad in ('accent=#GGGGGG', 'name='+'A'*100,
                        'wallpaper=../bad', '[evil]\naccent=#123456',
                        'format=2', 'a' * 6000):
                path.write_text(bad)
                p = run('--home','--theme',path,'--output',pathlib.Path(td)/'x.ppm',ok=False)
                self.assertNotEqual(p.returncode, 0, bad)

    def test_wav_stream(self):
        with tempfile.TemporaryDirectory() as td:
            p=pathlib.Path(td)/'mono.wav';raw=pathlib.Path(td)/'pcm.raw'
            with wave.open(str(p),'wb') as w:
                w.setnchannels(1);w.setsampwidth(1);w.setframerate(11025)
                w.writeframes(bytes([0,127,255,128])*512)
            inspect=run('--inspect',p)
            self.assertIn('format=WAV',inspect.stdout)
            self.assertIn('sample_rate=11025',inspect.stdout)
            run('--wav',p,'--pcm',raw)
            frames=raw.read_bytes()
            self.assertEqual(len(frames),2048*4)
            self.assertEqual(struct.unpack_from('<hh',frames,0),(-32768,-32768))
            self.assertEqual(struct.unpack_from('<hh',frames,4),(-256,-256))

    def test_photo_bmp_png_jpg_gif(self):
        with tempfile.TemporaryDirectory() as td:
            image=Image.new('RGB',(48,32),(225,40,75))
            for ext in ('.bmp','.png','.jpg','.gif'):
                infile=pathlib.Path(td)/('sample'+ext)
                image.save(infile)
                out=pathlib.Path(td)/('photo'+ext+'.ppm')
                p=run('--inspect',infile)
                self.assertIn('width=48',p.stdout)
                self.assertIn('height=32',p.stdout)
                q=run('--photo',infile,'--output',out)
                self.assertIn('Photo:',q.stdout)
                result=Image.open(out)
                # center of fitted image should retain red-dominant color
                color=result.getpixel((160,120))
                self.assertGreater(color[0],color[1]+80)
                self.assertEqual(result.size,(320,240))

    def test_mp3_headers(self):
        with tempfile.TemporaryDirectory() as td:
            path=pathlib.Path(td)/'frame.mp3'
            path.write_bytes(b'ID3\x04\x00\x00\x00\x00\x00\x06' + b'123456' +
                             bytes.fromhex('fffb9064')+b'\0'*100)
            p=run('--inspect',path)
            self.assertIn('format=MP3',p.stdout)
            self.assertIn('sample_rate=44100',p.stdout)
            self.assertIn('audio_codec=MP3',p.stdout)

    def test_mp4_box_parser(self):
        with tempfile.TemporaryDirectory() as td:
            path=pathlib.Path(td)/'s.mp4'
            sample=struct.pack('>I',36)+b'avc1'+b'\0'*24+struct.pack('>HH',1280,720)
            stsd=box('stsd',b'\0'*4+struct.pack('>I',1)+sample)
            path.write_bytes(box('ftyp',b'isom\0\0\0\0') +
                box('moov',box('trak',box('mdia',box('minf',box('stbl',stsd))))))
            p=run('--inspect',path)
            self.assertIn('format=MP4',p.stdout)
            self.assertIn('video_codec=avc1',p.stdout)
            self.assertIn('width=1280',p.stdout)
            self.assertIn('height=720',p.stdout)

    def test_invalid_media_fail_closed(self):
        with tempfile.TemporaryDirectory() as td:
            p=pathlib.Path(td)/'bad.bin'
            for data in (b'', b'RIFF1234WAVEbad', b'\x89PNG\r\n\x1a\n'+b'0'*40,
                         b'BM'+b'\xff'*60, b'\x00\x00\x00\x01ftyp'+b'\0'*40,
                         b'GIF89a'+b'\xff'*20):
                p.write_bytes(data)
                result=run('--inspect',p,ok=False)
                self.assertNotEqual(result.returncode,0,repr(data))

if __name__ == '__main__':
    unittest.main()
