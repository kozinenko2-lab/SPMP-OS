import ctypes, pathlib, subprocess, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class TestGpSP(unittest.TestCase):
 def test_built_library_symbols(self):
  so=ROOT/'third_party/gpsp/gpsp_libretro.so'
  self.assertTrue(so.exists())
  lib=ctypes.CDLL(str(so))
  for name in ('retro_init','retro_deinit','retro_run','retro_load_game','retro_serialize','retro_unserialize','retro_get_system_info'):
   self.assertTrue(hasattr(lib,name),name)
 def test_registry_and_api(self):
  txt=(ROOT/'src/cores/registry.c').read_text()
  self.assertIn('SPMP_CORE_HOST_TESTED,&spmp_gba_ops',txt)
  txt=(ROOT/'src/cores/libretro_multi_adapter.c').read_text()
  self.assertIn('SPMP_GPSP_SO',txt)
  self.assertIn('spmp_gba_ops=',txt)
 def test_armv5_cart_object(self):
  obj=ROOT/'build/stage24_gba_cart_armv5.o'
  self.assertTrue(obj.exists())
  self.assertEqual(obj.read_bytes()[4],1) # ELF32
  self.assertEqual(int.from_bytes(obj.read_bytes()[18:20], 'little'),40) # ARM
if __name__=='__main__': unittest.main()
