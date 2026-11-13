"""Stage18: actual Snes9x, bounded allocations, updated ARM overlay ABI."""
from pathlib import Path
import struct
import subprocess
import unittest
ROOT=Path(__file__).resolve().parents[1]

class Stage18(unittest.TestCase):
 def test_snes9x_dynamic_memory_and_frameskip(self):
  run=subprocess.run([str(ROOT/'build/stage18_snes_memory_harness')],cwd=ROOT,
                     capture_output=True,text=True,timeout=25)
  self.assertEqual(run.returncode,0,run.stdout+run.stderr)
  self.assertIn('exact-sized ROM allocation',run.stdout)
  self.assertIn('save/load passed',run.stdout)
 def test_armv5_rejects_old_overlay_abi(self):
  image=(ROOT/'build/probe_overlay_armv5.o').read_bytes()
  self.assertEqual(image[:4],b'\x7fELF')
  self.assertEqual(image[4],1)
  self.assertEqual(struct.unpack_from('<H',image,18)[0],40) # EM_ARM
  self.assertIn(b'SPMP'+struct.pack('<III',1,5,5),image)
 def test_host_snes_smoke_rom(self):
  rom=ROOT/'samples/snes_stage18_smoke.sfc'
  self.assertEqual(rom.stat().st_size,32768)
  run=subprocess.run([str(ROOT/'build/spmp-os-host'),'--run-core','snes',
                      '--content',str(rom),'--frames','4'],cwd=ROOT,
                      capture_output=True,text=True,timeout=25)
  self.assertEqual(run.returncode,0,run.stdout+run.stderr)
  self.assertIn('Frame callbacks: 4',run.stdout)

if __name__=='__main__':unittest.main()
