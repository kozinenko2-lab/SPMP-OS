#!/usr/bin/env python3
"""FM-02 POSIX integration: isolation, atomicity, recursive trees, error handling."""
import subprocess,tempfile, pathlib, os, time
from pathlib import Path
BIN=Path(os.environ.get('FM02_TEST_BINARY',str(Path(__file__).resolve().parents[1]/'build/fm02_ops')))
passed=0
with tempfile.TemporaryDirectory(prefix='spmp-fm02-tests-') as tmp:
    root=Path(tmp);(root/'FROM').mkdir();(root/'TO').mkdir();outside=root.parent/('do-not-touch-'+root.name)
    outside.write_text('PROTECTED',encoding='utf8')
    def call(action,src,name,target=None,cancel=None,ok=True):
        args=[str(BIN),str(root),action,src,name]
        if target is not None: args.append(target)
        if cancel is not None: args.append(str(cancel))
        p=subprocess.run(args,capture_output=True,text=True,timeout=15)
        assert (p.returncode==0)==ok,(args,p.stdout,p.stderr)
        print('PASS' if ok else 'EXPECTED FAIL',action,name,p.stdout.strip())
        return p
    def check(test,label):
        global passed
        assert test,label
        passed+=1
        print('  OK',label)
    try:
        f=root/'FROM'/'Журнал 11.11.26.log';f.write_bytes(b'log'*35000)
        os.utime(f,(946684800,946684800))
        call('copy','FROM',f.name,'TO')
        check((root/'TO'/f.name).read_bytes()==f.read_bytes(),'file exact copy, Cyrillic')
        check(f.read_bytes()==b'log'*35000,'copy leaves source')
        check((root/'TO'/f.name).stat().st_mtime_ns==f.stat().st_mtime_ns,'file date preserved')
        # recursive copy with empty child dir
        tree=root/'FROM'/'Папка';tree.mkdir();(tree/'sub').mkdir();(tree/'sub'/'abc.txt').write_bytes(b'hello'*1234);(tree/'empty').mkdir();os.utime(tree,(978307200,978307200))
        call('copy','FROM',tree.name,'TO')
        check((root/'TO'/'Папка'/'sub'/'abc.txt').read_bytes()==b'hello'*1234,'recursive tree copy')
        check((root/'TO'/'Папка'/'empty').is_dir(),'empty directory retained')
        check((root/'TO'/'Папка').stat().st_mtime_ns==tree.stat().st_mtime_ns,'folder date preserved')
        call('copy','FROM',f.name,'TO',ok=False)
        check((root/'TO'/f.name).read_bytes()==f.read_bytes(),'existing destination untouched')
        check(not list((root/'TO').glob('.spmpfm02-*')),'staging cleaned after conflict')
        call('move','FROM','Папка','TO',ok=False)
        check((root/'FROM'/'Папка').exists(),'conflicting move remains source')
        call('rename','FROM',f.name,'Журнал ✓.log')
        check((root/'FROM'/'Журнал ✓.log').exists(),'rename UTF-8')
        call('rename','FROM','Журнал ✓.log','../escape',ok=False)
        check(outside.read_text()=='PROTECTED','rename traversal rejected')
        # cancelled big file must leave destination absent
        big=root/'FROM'/'large.bin';big.write_bytes(os.urandom(300000))
        call('copy','FROM','large.bin','TO',16384,ok=False)
        check(not (root/'TO'/'large.bin').exists(),'cancel removes temp and target')
        check(not list((root/'TO').glob('.spmpfm02-*')),'cancel cleans temporary files')
        call('copy','FROM','large.bin','TO', 'scan',ok=False)
        check(not (root/'TO'/'large.bin').exists(),'cancel during scan')
        # existing symlink reject on source/destination
        os.symlink(outside,root/'FROM'/'link.txt')
        call('copy','FROM','link.txt','TO',ok=False)
        check(outside.read_text()=='PROTECTED','symlink source not followed')
        call('copy','FROM/../../','large.bin','TO',ok=False)
        check(outside.read_text()=='PROTECTED','parent traversal denied')
        os.symlink(outside,root/'TO'/'targetlink')
        call('copy','FROM','large.bin','TO/targetlink',ok=False)
        check(outside.read_text()=='PROTECTED','symlink intermediate denied')
        (tree/'sub'/'trouble').symlink_to(outside)
        call('copy','FROM','Папка','TO',ok=False)
        check((root/'FROM'/'Папка'/'sub'/'abc.txt').exists(),'recursive symlink denied safely')
        (tree/'sub'/'trouble').unlink()
        (root/'TO'/'Папка'/'sub'/'abc.txt').unlink()
        # path check against own subtree (even with no destination conflict)
        call('copy','FROM','Папка','FROM/Папка/sub',ok=False)
        check(not list((tree/'sub').glob('.spmpfm02-*')),'copy-into-self refused')
        # Delete of nested dir when source content is safe
        call('delete','FROM','Папка')
        check(not tree.exists(),'recursive deletion')
        call('delete','FROM','large.bin','scan',ok=False)
        check(big.exists(),'scan cancellation leaves object')
        call('delete','FROM','large.bin')
        check(not big.exists(),'delete file')
        (root/'FROM'/'a.txt').write_text('a');(root/'FROM'/'b.txt').write_text('b')
        call('rename','FROM','a.txt','b.txt',ok=False)
        check((root/'FROM'/'a.txt').read_text()=='a' and (root/'FROM'/'b.txt').read_text()=='b','rename no-overwrite')
        call('move','FROM','a.txt','TO')
        check(not (root/'FROM'/'a.txt').exists() and (root/'TO'/'a.txt').exists(),'atomic move')
        check(not list(root.rglob('.spmpfm02-*')),'no staging left')
    finally:
        outside.unlink(missing_ok=True)
print('PASSED',passed,'assertion groups')
