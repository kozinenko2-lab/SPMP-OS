#!/usr/bin/env python3
"""Tests on real temporary directory trees. Does not modify SD/firmware."""
import os
import re
import subprocess
import tempfile
from pathlib import Path
from datetime import datetime

BINARY=Path(os.environ.get('FM03_BINARY',Path(__file__).resolve().parents[1]/'build/fm03_props'))
count=0
def run(root,parent,name,cancel=None,expected=0):
    global count
    command=[str(BINARY),str(root),parent,name]
    if cancel is not None: command.append(str(cancel))
    r=subprocess.run(command,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=15)
    if r.returncode!=expected:
        raise AssertionError(f'{command} returned {r.returncode}; stderr={r.stderr}; stdout={r.stdout}')
    count+=1
    return dict(line.split('=',1) for line in r.stdout.splitlines() if '=' in line)
with tempfile.TemporaryDirectory(prefix='spmp-fm03-') as td, tempfile.TemporaryDirectory(prefix='spmp-outside-') as outside:
    root=Path(td)
    (root/'MUSIC').mkdir()
    (root/'MUSIC'/'Nested').mkdir()
    (root/'MUSIC'/'Песня.flac').write_bytes(b'a'*113)
    (root/'MUSIC'/'Nested'/'track.ogg').write_bytes(b'c'*260)
    (root/'MUSIC'/'.hidden').write_bytes(b'x'*41)
    d=run(root,'','MUSIC')
    assert d['status']=='3' and d['type']=='1' and d['files']=='3' and d['dirs']=='1' and d['bytes']=='414'
    assert d['visited']=='4' and d['sd_valid']=='1' and int(d['sd_total'])>0
    assert re.fullmatch(r'\d\d\.\d\d\.\d\d \d\d:\d\d',d['modified'])
    f=run(root,'MUSIC','Песня.flac')
    assert f['status']=='1' and f['type']=='2' and f['size']=='113' and f['path']=='MUSIC/Песня.flac'
    z=run(root,'MUSIC','Nested')
    assert z['files']=='1' and z['dirs']=='0' and z['bytes']=='260'
    (root/'empty').mkdir()
    e=run(root,'','empty')
    assert e['files']=='0' and e['bytes']=='0' and e['status']=='3'
    for i in range(275):
        (root/'MUSIC'/f'f{i:03}.txt').write_bytes(bytes(i%32))
    big=run(root,'','MUSIC')
    assert big['files']=='278' and big['dirs']=='1' and big['visited']=='279'
    assert int(big['ticks'])>=30
    early=run(root,'','MUSIC',cancel=2)
    assert early['status']=='4' and 0<int(early['visited'])<279
    symlink=root/'MUSIC'/'external';symlink.symlink_to(Path(outside))
    s=run(root,'','MUSIC')
    assert s['skipped']=='1' and s['files']=='278' and s['dirs']=='1'
    direct=run(root,'MUSIC','external',expected=1)
    assert direct['status']=='5' and direct['error']!='0'
    bad=run(root,'..','MUSIC',expected=1)
    assert bad['status']=='0' and bad['error']=='0'  # invalid args rejected before opening UI
    (root/'huge.bin').write_bytes(b'')
    with open(root/'huge.bin','r+b') as h: h.truncate(5*(1024**3)+123)
    huge=run(root,'','huge.bin')
    assert huge['size']==str(5*(1024**3)+123)
    (root/'bigfolder').mkdir()
    (root/'bigfolder'/'huge.bin').write_bytes(b'')
    with open(root/'bigfolder'/'huge.bin','r+b') as h: h.truncate(5*(1024**3)+123)
    hb=run(root,'','bigfolder')
    assert hb['bytes']==huge['size']
    (root/'deep').mkdir()
    path=root/'deep'
    for j in range(18):
        path=path/f'd{j}'
        path.mkdir()
    deep=run(root,'','deep',expected=1)
    assert deep['status']=='5' and deep['error']!='0'
    (root/'locked.txt').write_text('text')
    os.chmod(root/'locked.txt',0o444)
    locked=run(root,'','locked.txt')
    assert locked['type']=='2'
    # refuse symlink root
    alt=root.parent/(root.name+'-link')
    alt.symlink_to(root)
    try:
        rejected=run(alt,'','MUSIC',expected=3)
        assert rejected=={}
    finally:alt.unlink()
print('FM03 POSIX scenarios:',count,'passed')
