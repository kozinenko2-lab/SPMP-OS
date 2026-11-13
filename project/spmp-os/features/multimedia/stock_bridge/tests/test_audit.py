#!/usr/bin/env python3
import importlib.util
import io
import pathlib
import struct
import sys
import unittest
import zipfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('audit',ROOT/'tools/audit_stock_fsp.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
class StockAuditTest(unittest.TestCase):
 def test_mmpi(self):
  buf=bytearray(56);buf[:4]=b'MMPi';struct.pack_into('<I',buf,4,56);buf[16:20]=b'MMPd';struct.pack_into('<I',buf,20,36)
  self.assertTrue(module.mmpi(buf)['matches_file_size'])
  buf[3]=0;self.assertFalse(module.mmpi(buf)['valid'])
 def test_xml(self):
  x=b'<MMI ClassName="PhotoPlayWindow" w="320" h="240"><Image id="bk"><Img_Src>/RESOURCE/PIC.BMP</Img_Src></Image></MMI>'
  r=module.xml_info(x,{'RESOURCE/PIC.BMP'})
  self.assertTrue(r['strict_xml_valid']);self.assertEqual(r['canvas'],[320,240]);self.assertEqual(r['missing_resource_references'],[])
 def test_xml_broken(self):
  self.assertFalse(module.xml_info(b'<MMI a="1"b="2"/>',set())['strict_xml_valid'])
 def test_elf_invalid(self):
  self.assertFalse(module.elf(b'not elf')['valid'])
 def test_bad_member(self):
  tmp=io.BytesIO()
  with zipfile.ZipFile(tmp,'w') as z:z.writestr('IMAGE/OK','abc')
  tmp.seek(0)
  with zipfile.ZipFile(tmp) as z:
   self.assertEqual(module.read_member(z,'IMAGE/OK'),b'abc')
   self.assertIsNone(module.read_member(z,'IMAGE/NONE'))
if __name__=='__main__':unittest.main()
