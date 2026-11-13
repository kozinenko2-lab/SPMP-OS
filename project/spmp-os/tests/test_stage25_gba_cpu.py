import pathlib, subprocess, unittest, struct
ROOT=pathlib.Path(__file__).resolve().parents[1]
class TestGpSPArm7(unittest.TestCase):
 def test_test_rom_arm7_program(self):
  rom=(ROOT/'tests/roms/stage24_gba_test.gba').read_bytes()
  self.assertEqual(struct.unpack_from('<I',rom,0xc8)[0],0xe3811003)
  self.assertEqual(struct.unpack_from('<I',rom,0xcc)[0],0xe5801000)
  self.assertEqual(struct.unpack_from('<I',rom,0xd8)[0],0xe5801000)
 def test_emulator_executes_vram_write(self):
  exe=ROOT/'build/stage24_gba_harness'
  if not exe.exists(): self.skipTest('Build harness first: make stage24_gba_harness')
  res=subprocess.run([str(exe),str(ROOT/'tests/roms/stage24_gba_test.gba')],capture_output=True,text=True,check=True)
  self.assertIn('pixel=0xf800',res.stdout)
if __name__=='__main__': unittest.main()
