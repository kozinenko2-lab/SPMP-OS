#!/usr/bin/env python3
import sys,unittest,tempfile,hashlib,json,os
from pathlib import Path
TOP=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(TOP/'tools'))
from analyze_gui_boot import Firmware, ROOTS, analyze, outfiles, arm_imm, branch_target, function_boundary, validate_specifics, GUI_MMIO_ADDR
ORIGINAL=Path(os.environ.get('SPMP_MMI_RAP', str(TOP/'reference'/'8000_MMI.RAP')))
if not ORIGINAL.exists(): ORIGINAL=Path('/mnt/data/8000_MMI.RAP')

class Stage55(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.raw=ORIGINAL.read_bytes()
  cls.fw=Firmware(cls.raw)
  cls.analysis=analyze(cls.fw)
 def test_sha(self): self.assertEqual(hashlib.sha256(self.raw).hexdigest(),'5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e')
 def test_exact_arm_checks(self): self.assertTrue(all(self.analysis['checks'].values()))
 def test_12_scoped(self): self.assertEqual(len(self.analysis['functions']),len(ROOTS))
 def test_function_prologue(self):
  for entry in ROOTS:
   if entry==0x280040: self.assertEqual(self.fw.u32(entry),0xee110f10)  # CP15 startup, not a C-function prologue
   else: self.assertEqual(self.fw.u32(entry),0xe1a0c00d)
 def test_ghidra_heuristic_not_empty(self):
  for entry in ROOTS: self.assertGreater(function_boundary(self.fw,entry),entry)
 def test_mmio_address(self):
  a=0x447668
  self.assertEqual(sum(arm_imm(self.fw.u32(a+i)) for i in (0,4,8)),GUI_MMIO_ADDR)
 def test_mmio_read_modify_write(self):
  self.assertEqual([self.fw.u32(a) for a in (0x447674,0x447678,0x44767c)],[0xe5923000,0xe3833007,0xe5823000])
 def test_gui_calls_coordinator(self): self.assertEqual(self.fw.bl(0x3be5cc),0x447640)
 def test_coordinator_calls_init(self):self.assertEqual(self.fw.bl(0x4476b0),0x4475c0)
 def test_init_calls_background(self):self.assertEqual(self.fw.bl(0x4475a4),0x447458)
 def test_background_spawn(self): self.assertEqual(self.fw.bl(0x44749c),0x339aac)
 def test_init_spawn(self): self.assertEqual(self.fw.bl(0x447610),0x339aac)
 def test_gui_spawn(self): self.assertEqual(self.fw.bl(0x280810),0x339aac)
 def test_backedge(self):self.assertEqual(branch_target(0x447450,self.fw.u32(0x447450)),0x4473f0)
 def test_delay_100_ticks(self): self.assertEqual(self.fw.u32(0x447448),0xe3a00064)
 def test_creations(self):self.assertEqual({x['priority_arg_r0'] for x in self.analysis['thread_spawns']},{12,25,30})
 def test_string_sources(self):
  self.assertEqual(self.fw.cstring(0x504214),'Systen BackGround Thread')
  self.assertEqual(self.fw.cstring(0x5042dc),'Systen initial Thread')
 def test_abi_wrapper(self):self.assertEqual(self.fw.bl(0x339af0),0x33a2dc)
 def test_diagnostic_strings(self):
  self.assertEqual(self.fw.cstring(0x4f18a8),'Stack size too small')
 def test_output_reproducible(self):
  with tempfile.TemporaryDirectory() as p1,tempfile.TemporaryDirectory() as p2:
   outfiles(self.analysis,Path(p1));outfiles(analyze(Firmware(self.raw)),Path(p2))
   self.assertEqual(sorted(x.name for x in Path(p1).iterdir()),sorted(x.name for x in Path(p2).iterdir()))
   for x in Path(p1).iterdir(): self.assertEqual(x.read_bytes(),(Path(p2)/x.name).read_bytes())
 def test_wrong_binary_fails_closed(self):
  damaged=bytearray(self.raw)
  damaged[0x140] ^= 1
  with self.assertRaises(ValueError):Firmware(bytes(damaged))
 def test_untrusted_length(self):
  with self.assertRaises(ValueError):Firmware(self.raw[:1000])
 def test_specific_tamper_fails(self):
  # Test must always reject a tampered source at initialization.
  damaged=bytearray(self.raw)
  idx=0x4c+(0x447674-0x280000)
  damaged[idx]^=1
  with self.assertRaises(ValueError):Firmware(damaged)
 def test_thread_entry_distinct(self):
  self.assertEqual(len({x['entry'] for x in self.analysis['thread_spawns']}),3)
 def test_coordinator_one_direct_xref(self):
  self.assertEqual(self.analysis['direct_BL_xrefs']['0x00447640'],['0x003BE5CC'])
 def test_thread_entries_have_no_direct_bl(self):
  self.assertEqual(self.analysis['direct_BL_xrefs']['0x00447540'],[])
  self.assertEqual(self.analysis['direct_BL_xrefs']['0x004473BC'],[])
 def test_no_modified_firmware_outputs(self):
  self.assertNotIn('patched_image',self.analysis)
  self.assertTrue(self.analysis['no_binary_changes'])

if __name__=='__main__': unittest.main()
