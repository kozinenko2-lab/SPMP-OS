"""Checks that PicoDrive CD and 32X paths remain enabled in the host and ARM
cross-build; these tests do NOT assert successful commercial-game emulation."""
import json
from pathlib import Path
import subprocess
import unittest
ROOT=Path(__file__).resolve().parents[1]
class FullPicoProfile(unittest.TestCase):
    def test_arm_elf_full_core(self):
        r=json.loads((ROOT/'build/picodrive_armv5_full/report.json').read_text())
        self.assertEqual(r['compiled'],51)
        self.assertEqual(r['files'],51)
        self.assertTrue(r['partial_link_ok'])
        self.assertTrue(r['elf_arm32'])
        self.assertTrue(r['enabled_pico_cd'])
        self.assertTrue(r['enabled_pico_32x'])
        self.assertIn('SH2 interpreter x2 (no JIT)',r['cpu_modes'])
        self.assertGreater(r['unresolved_count'],0)
    def test_host_exposes_full_subsystems_and_chd(self):
        so=ROOT/'third_party/picodrive/picodrive_libretro.so'
        self.assertTrue(so.exists())
        x=subprocess.run(['nm','-D',str(so)],capture_output=True,text=True,check=True).stdout
        for key in ('Pico32xStartup','PicoCreateMCD','chd_open','retro_load_game'):
            with self.subTest(symbol=key):self.assertIn(key,x)
    def test_core_formats_and_loader(self):
        s=(ROOT/'src/cores/registry.c').read_text()
        self.assertIn('md;gen;smd;32x;cue;iso;chd;m3u',s)
        p=(ROOT/'src/cores/libretro_multi_adapter.c').read_text()
        self.assertIn('spmp_cd_path_only',p)
        self.assertIn('const int is_disc=spmp_cd_path_only',p)
        self.assertIn('game.size=is_disc?0:bytes',p)
    def test_32x_host(self):
        r=subprocess.run(['./build/stage22_32x_harness','samples/spmp_32x_init_test.32x'],cwd=ROOT,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,timeout=30)
        self.assertEqual(r.returncode,0,r.stderr[-1500:]);self.assertIn('32X memory created',r.stderr)
    def test_cd_host(self):
        r=subprocess.run(['./build/stage22_cd_harness','samples/spmp_test_cd.cue'],cwd=ROOT,
              env={**__import__('os').environ,'SPMP_BIOS_DIR':'build/stage22_test_bios'},
              stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,timeout=30)
        self.assertEqual(r.returncode,0,r.stderr[-1500:]);self.assertIn('CD controller active',r.stderr)
    def test_no_flashable_claim(self):
        r=json.loads((ROOT/'firmware/module_catalog.json').read_text())
        self.assertFalse(r['firmware_flashable'])
        core=next(x for x in r['modules'] if x['id']=='megadrive')
        self.assertFalse(core['native_firmware_available'])
        self.assertIn('Sega CD',core['host_supported_families'])
        self.assertIn('Sega 32X',core['host_supported_families'])
if __name__=='__main__':unittest.main()
