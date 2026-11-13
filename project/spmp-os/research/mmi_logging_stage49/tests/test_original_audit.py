import importlib.util
import pathlib
import unittest
R=pathlib.Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('audit_original_rap',R/'tools/audit_original_rap.py')
audit=importlib.util.module_from_spec(spec)
spec.loader.exec_module(audit)
class AuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data=(pathlib.Path('/mnt/data/8000_MMI.RAP')).read_bytes()
    def test_known_original(self):
        result=audit.analyse(self.data)
        self.assertEqual(result['no_op_diag_stub']['direct_BL_call_sites'],4559)
        self.assertEqual(result['adjacent_formatter']['initial_flag_0x00514244'],0)
    def test_reject_changed_payload(self):
        modified=bytearray(self.data)
        modified[0x18BB0]^=1
        with self.assertRaises(ValueError):audit.analyse(modified)
    def test_reject_truncated(self):
        with self.assertRaises(ValueError):audit.analyse(self.data[:-1])
if __name__=='__main__':unittest.main()
