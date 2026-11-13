"""Stage17 integration smoke, metadata, staged object and regression expectations."""
from pathlib import Path
import subprocess, json, unittest
ROOT=Path(__file__).resolve().parents[1]
class Stage17(unittest.TestCase):
 def test_actual_snes9x_core(self):
  p=subprocess.run([str(ROOT/'build/stage17_snes_harness')],cwd=ROOT,timeout=30,capture_output=True,text=True)
  self.assertEqual(p.returncode,0,p.stdout+p.stderr)
  self.assertIn('save/load passed',p.stdout)
 def test_megadrive_smd_converter(self):
  p=subprocess.run([str(ROOT/'build/stage17_md_harness')],cwd=ROOT,timeout=10,capture_output=True,text=True)
  self.assertEqual(p.returncode,0,p.stdout+p.stderr)
  self.assertIn('2-block SMD',p.stdout)
 def test_psx_object_staged_but_not_runnable(self):
  meta=json.loads((ROOT/'firmware/module_catalog.json').read_text())
  self.assertFalse(meta['firmware_flashable'])
  core=next(c for c in meta['modules'] if c['id']=='ps1')
  self.assertFalse(core['native_firmware_available'])
  self.assertGreater(core['unresolved_symbol_count'],30)
  self.assertEqual(core['core_armv5']['bytes'],(ROOT/core['core_armv5']['path']).stat().st_size)
 def test_picodrive_host_ready_not_device_ready(self):
  p=subprocess.run([str(ROOT/'build/spmp-os-host'),'--cores'],cwd=ROOT,capture_output=True,text=True,timeout=10)
  self.assertIn('snes          HOST_TESTED',p.stdout)
  self.assertIn('megadrive     HOST_TESTED',p.stdout)
  self.assertIn('ps1           RESEARCH',p.stdout)
if __name__=='__main__':unittest.main()
