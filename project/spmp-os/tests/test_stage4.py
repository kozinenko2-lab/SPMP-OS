import json
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]
EXE=ROOT/'build/spmp-os-host'
SAMPLE=ROOT/'samples/chip8_font0.ch8'


def run(*args):
    return subprocess.run([str(EXE),*map(str,args)],capture_output=True,text=True)

class Stage4ROMDetection(unittest.TestCase):
    def test_all_stock_core_ids_registered(self):
        r=run('--cores')
        self.assertEqual(r.returncode,0,r.stderr)
        for platform in ['gba']:
            with self.subTest(platform=platform):
                self.assertRegex(r.stdout, rf'\b{platform}\s+RESEARCH\b')

    def test_snes_real_core_host_ready(self):
        r=run('--cores')
        self.assertRegex(r.stdout,r'snes\s+HOST_TESTED')

    def test_ines_signature_beats_extension(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'unknown.bin'
            path.write_bytes(b'NES\x1a'+bytes(12)+bytes(128))
            r=run('--resolve',path)
            self.assertIn('UNIQUE (1 core(s))',r.stdout)
            self.assertIn('nes [HOST_TESTED]',r.stdout)

    def test_malformed_ambiguous_bin_is_not_guessed(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'something.bin'
            path.write_bytes(bytes(128))
            r=run('--resolve',path)
            self.assertIn('AMBIGUOUS',r.stdout)

    def test_nintendo_gb_logo(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'foo.bin'
            buf=bytearray(0x150)
            buf[0x104:0x114]=bytes.fromhex('ce ed 66 66 cc 0d 00 0b 03 73 00 83 00 0c 00 0d')
            path.write_bytes(buf)
            r=run('--resolve',path)
            self.assertIn('gb [HOST_TESTED]',r.stdout)
            self.assertIn('UNIQUE',r.stdout)

    def test_gba_signature(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'foo.bin'
            buf=bytearray(0xc0)
            buf[4:12]=bytes.fromhex('24 ff ae 51 69 9a a2 21')
            buf[0xb2]=0x96
            path.write_bytes(buf)
            r=run('--resolve',path)
            self.assertIn('gba [RESEARCH]',r.stdout)
            self.assertIn('UNIQUE',r.stdout)

    def test_megadrive_and_sms(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'title.bin'
            md=bytearray(0x200);md[0x100:0x104]=b'SEGA';path.write_bytes(md)
            self.assertIn('megadrive [HOST_TESTED]',run('--resolve',path).stdout)
            sms=bytearray(0x8000);sms[0x7ff0:0x7ff8]=b'TMR SEGA';path.write_bytes(sms)
            self.assertIn('sms [HOST_TESTED]',run('--resolve',path).stdout)

class Chip8Integration(unittest.TestCase):
    def test_chip8_runs_nontrivial_opcode_sequence_and_renders(self):
        with tempfile.TemporaryDirectory() as tmp:
            out=Path(tmp)/'shot.ppm'
            r=run('--run-core','chip8','--content',SAMPLE,'--frames','5','--screenshot',out)
            self.assertEqual(r.returncode,0,r.stderr)
            self.assertIn('Frame callbacks: 5',r.stdout)
            data=out.read_bytes()
            head=b'P6\n64 32\n255\n'
            self.assertTrue(data.startswith(head))
            self.assertEqual(len(data),len(head)+64*32*3)
            pixels=data[len(head):]
            self.assertIn(b'\xff\xff\xff',pixels)
            self.assertIn(b'\0\0\0',pixels)
            # CHIP-8 font glyph 0: at row zero x=0..3 are on, x=4 off
            self.assertEqual(pixels[:12],b'\xff'*12)
            self.assertEqual(pixels[12:15],b'\0'*3)

    def test_chip8_bad_content_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'bad.ch8'
            path.write_bytes(b'\xff')
            r=run('--run-core','chip8','--content',path,'--frames','2')
            self.assertNotEqual(r.returncode,0)
            self.assertIn('not compiled/available',r.stderr) # CLI generic error
            path.write_bytes(bytes(3585))
            r=run('--run-core','chip8','--content',path,'--frames','1')
            self.assertNotEqual(r.returncode,0)

    def test_stock_audit_is_read_only(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory=Path(tmp)
            data=b'\x00abcgpSP\x01'
            (directory/'GPSP.BIN').write_bytes(data)
            report=directory/'audit.json'
            cmd=['python3',str(ROOT/'tools/audit_stock_cores.py'),
                 '--game-dir',str(directory),'--out',str(report)]
            p=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertEqual((directory/'GPSP.BIN').read_bytes(),data)
            obj=json.loads(report.read_text())
            gpsp=next(m for m in obj['modules'] if m['name']=='GPSP.BIN')
            self.assertTrue(gpsp['gpSP_string_found'])

if __name__ == '__main__':unittest.main()
