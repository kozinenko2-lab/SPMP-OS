"""Stage 23 new self-contained smoke tests (build with make stage23-check)."""
from pathlib import Path
import subprocess
import unittest
ROOT=Path(__file__).resolve().parents[1]
class Stage23(unittest.TestCase):
 def test_video_scaler(self):
  r=subprocess.run([str(ROOT/'build/stage23_video_harness')],capture_output=True,text=True,timeout=10)
  self.assertEqual(r.returncode,0,r.stdout+r.stderr)
 def test_gba_preflight(self):
  r=subprocess.run([str(ROOT/'build/stage23_gba_harness')],capture_output=True,text=True,timeout=10)
  self.assertEqual(r.returncode,0,r.stdout+r.stderr)
 def test_armv5_objects(self):
  for name in ('stage23_video_armv5.o','stage23_gba_armv5.o'):
   data=(ROOT/'build'/name).read_bytes()
   self.assertEqual(data[:4],b'\x7fELF')
   self.assertEqual(data[4],1)
   self.assertEqual(data[18:20],b'\x28\x00')
if __name__=='__main__':unittest.main()
