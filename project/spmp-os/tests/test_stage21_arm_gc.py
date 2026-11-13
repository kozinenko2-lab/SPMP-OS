import json, pathlib, subprocess, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
REP=ROOT/'build/picodrive_armv5_md_gc/report.json'
class Stage21Tests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  if not REP.exists(): subprocess.run(['python3','tools/build_picodrive_md_gc.py'],cwd=ROOT,check=True)
  cls.r=json.loads(REP.read_text())
 def test_arm_target(self): self.assertIn('ARM926EJ-S',self.r['architecture'])
 def test_31_real_sources(self): self.assertEqual(self.r['selected_objects'],31)
 def test_dead_code_removal(self): self.assertLess(self.r['after_gc_count'],self.r['before_count'])
 def test_real_unresolved_are_reported(self):
  syms=subprocess.run(['nm','-u',str(ROOT/'build/picodrive_armv5_md_gc/picodrive_md_live.o')],text=True,capture_output=True,check=True)
  current=sorted(set(x.split()[-1] for x in syms.stdout.splitlines() if x.strip()))
  self.assertEqual(current,self.r['remaining'])
 def test_not_marked_executable(self): self.assertFalse(self.r['executable'])
 def test_metadata_is_arm(self):
  s=subprocess.run(['readelf','-h',str(ROOT/'build/picodrive_armv5_md_gc/picodrive_md_live.o')],text=True,capture_output=True,check=True).stdout
  self.assertIn('ARM',s); self.assertIn('REL (Relocatable file)',s)
if __name__=='__main__':unittest.main()
