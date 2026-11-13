import importlib.util,os,unittest,zipfile,tempfile,sys
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('audit',HERE/'tools'/'audit_ghidra_and_boot.py')
audit=importlib.util.module_from_spec(spec);spec.loader.exec_module(audit)
ARCHIVE=Path(os.environ.get('SPMP8K_ARCHIVE','/mnt/data/SPMP8k.zip'))
STAGE52=Path(os.environ.get('SPMP52_ROOT','/mnt/data/spmp_stage53/work'))/'SPMP-OS_Stage52_Ghidra_Imports'/'elf'

class TestAudit(unittest.TestCase):
 def test_arm_branch(self):
  ins=0xeb000000 | (((0x38f0c-0x4a9c4-8)//4)&0xffffff)
  self.assertEqual(audit.branch_target(ins,0x4a9c4),0x38f0c)
  self.assertIsNone(audit.branch_target(0xe12fff1e,0x298b64))
 def test_source_archive(self):
  if not ARCHIVE.exists():self.skipTest('Set SPMP8K_ARCHIVE to original user GZF ZIP')
  with zipfile.ZipFile(ARCHIVE) as z:
   self.assertEqual(len(z.namelist()),3)
   self.assertEqual(sorted(Path(i).suffix for i in z.namelist()),['.gzf']*3)
 def test_real_ghidra_crc(self):
  if not ARCHIVE.exists():self.skipTest('Set SPMP8K_ARCHIVE to original user GZF ZIP')
  with zipfile.ZipFile(ARCHIVE) as z, tempfile.TemporaryDirectory() as td:
   for name,_,_,_ in audit.LABELS.values():
    file=Path(td)/name
    file.write_bytes(z.read(name))
    result=audit.read_gzf(file)
    self.assertTrue(result['deflate_crc_ok'])
    self.assertGreater(result['internal_database_bytes'],1000000)
    self.assertGreater(result['distinct_auto_fun_labels'],100)
 def test_original_bss_word(self):
  f=STAGE52/'MMI'/'segment_00280000_file_00000034.bin'
  if not f.exists():self.skipTest('Set SPMP52_ROOT to extracted Stage52 original-derived ELF bundle')
  b=f.read_bytes()
  self.assertEqual(audit.word(b,0x280000,0x280498),0x56ad90)
  self.assertEqual(audit.word(b,0x280000,0x28049c),0x6c8fdc)
 def test_headers_and_loader(self):
  f=STAGE52/'MMI'/'segment_00280000_file_00000034.bin'
  if not f.exists():self.skipTest('Set SPMP52_ROOT to extracted Stage52 original-derived ELF bundle')
  b=f.read_bytes()
  self.assertEqual(audit.word(b,0x280000,0x298b64),0xe12fff1e)
  self.assertEqual(audit.word(b,0x280000,0x460254),0xe1a0f003)

if __name__ == '__main__':unittest.main()
