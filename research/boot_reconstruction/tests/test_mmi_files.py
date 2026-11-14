import hashlib,json,sys,tempfile,unittest,zipfile
from pathlib import Path
BASE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(BASE/'tools'))
from make_files_profile import header,produce
from make_sd_bundle import build,DIRECTORIES
class FileManagerTests(unittest.TestCase):
 def test_generated_directory_profile_and_original_arm_evidence(self):
  p=json.loads((BASE/'evidence/files_profile.json').read_text())
  self.assertEqual(header(p),(BASE/'examples/mmi_files/files_profile.h').read_text())
  self.assertEqual(header(p,True),(BASE/'tests/files_profile_synthetic.h').read_text())
  self.assertEqual(p['dirent_bytes'],260);self.assertEqual(p['stat_size_offset'],16)
  r=json.loads((BASE/'evidence/directory_services_arm.json').read_text());self.assertEqual(r['source_sha256'],p['source_sha256']);self.assertEqual(r['case_count'],14)
  with self.assertRaises(ValueError):produce(b'unknown firmware')
 def test_direct_ttf_is_original_user_font_with_license_metadata(self):
  fonts=BASE.parents[1]/'project/spmp-os/assets/fonts';m=json.loads((fonts/'font_manifest.json').read_text());data=(fonts/'Roboto-Regular.ttf').read_bytes()
  self.assertEqual(hashlib.sha256(data).hexdigest(),m['sha256']);self.assertEqual(len(data),171272)
  self.assertIn('Apache License, Version 2.0',m['name_table']['13'][0]);self.assertEqual(data[:4],b'\x00\x01\x00\x00')
 def test_sd_bundle_separates_data_and_system_and_does_not_distribute_rom(self):
  with tempfile.TemporaryDirectory() as t:
   t=Path(t);binary=t/'app.bin';binary.write_bytes(b'NGame1.0'+b'\0'*128);output=t/'sd.zip';manifest=build(binary,output)
   with zipfile.ZipFile(output) as z:
    names=z.namelist();self.assertIn('SPMP-OS/User/ROMs/Atari2600/',names);self.assertIn('SPMP-OS/System/Lib/',names)
    self.assertIn('SPMP-OS/System/Fonts/Roboto-Regular.ttf',names)
    self.assertFalse(any(n.lower().endswith(('.a26','.rap','.gzf')) for n in names))
    self.assertEqual(z.read('SPMP_OS_FILES.bin'),binary.read_bytes());self.assertEqual(len([n for n in names if n.endswith('/')]),len(DIRECTORIES))
    self.assertEqual(json.loads(z.read('SPMP-OS/System/INSTALL.json')),manifest)
 def test_atari_private_reference_contains_only_metadata(self):
  r=json.loads((BASE/'evidence/atari_user_adventure_reference.json').read_text());self.assertEqual(r['bytes'],4096)
  self.assertEqual(len(r['sha256']),64);self.assertNotIn('payload',r);self.assertNotIn('data',r)
