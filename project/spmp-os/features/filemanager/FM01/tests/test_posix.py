#!/usr/bin/env python3
"""Real read-only directory, paging and containment checks for host adapter."""
import ctypes as C
import datetime as dt
import os
from pathlib import Path
import tempfile

BASE=Path(__file__).resolve().parents[1]
lib=C.CDLL(str(BASE/'build/libfm01_host.so'))
class Entry(C.Structure):
    _fields_=[('name',C.c_char*256),('size_bytes',C.c_uint64),('year',C.c_uint16),('month',C.c_uint8),('day',C.c_uint8),('type',C.c_uint8),('read_only',C.c_uint8)]
class FS(C.Structure):
    _fields_=[('rootfd',C.c_int)]
lib.fm01_posix_init.argtypes=[C.POINTER(FS),C.c_char_p];lib.fm01_posix_init.restype=C.c_int
lib.fm01_posix_list.argtypes=[C.c_void_p,C.c_char_p,C.c_uint32,C.c_uint32,C.POINTER(Entry),C.POINTER(C.c_uint32),C.POINTER(C.c_uint32),C.POINTER(C.c_uint32),C.POINTER(C.c_uint32)];lib.fm01_posix_list.restype=C.c_int
lib.fm01_posix_close.argtypes=[C.POINTER(FS)]

def query(fs,rel,offset=0,capacity=9):
    entries=(Entry*max(1,capacity))()
    returned,total,files,dirs=(C.c_uint32() for _ in range(4))
    rc=lib.fm01_posix_list(C.byref(fs),os.fsencode(rel),offset,capacity,entries,C.byref(returned),C.byref(total),C.byref(files),C.byref(dirs))
    return rc,returned.value,total.value,files.value,dirs.value,[entries[i] for i in range(returned.value)]

with tempfile.TemporaryDirectory() as tmp:
    root=Path(tmp);(root/'music').mkdir();(root/'photos').mkdir();
    for i in range(275):(root/'music'/f'f{i:03d}.mp3').write_bytes(b'*'*(i%7+1))
    (root/'music'/'Вечерний дождь.flac').write_bytes(b'X'*777)
    stamp=dt.datetime(2026,11,11,11,42,tzinfo=dt.timezone.utc).timestamp()
    os.utime(root/'music'/'Вечерний дождь.flac',(stamp,stamp))
    (root/'music'/'folder').mkdir()
    (root/'music'/'outside').symlink_to('/tmp')
    (root/'photos'/'outside_file').symlink_to('/etc/passwd')
    fs=FS(-1)
    assert lib.fm01_posix_init(C.byref(fs),os.fsencode(root))==0
    try:
        rc,n,total,files,dirs,items=query(fs,'music',0)
        assert (rc,n,total,files,dirs)==(0,9,277,276,1),(rc,n,total,files,dirs)
        assert items[0].name==b'folder' and items[0].type==1
        names=[]
        for start in range(0,277,9):
            rc,n,t,f,d,page=query(fs,'music',start)
            assert (rc,t,f,d)==(0,277,276,1)
            names.extend(e.name.decode('utf8') for e in page)
        assert len(names)==277 and len(set(names))==277
        assert 'Вечерний дождь.flac' in names
        assert 'outside' not in names
        rc,n,t,f,d,page=query(fs,'music',276,9)
        assert (rc,n)==(0,1)
        assert page[0].year==2026 and page[0].month==11 and page[0].day==11
        assert query(fs,'../')[0]!=0
        assert query(fs,'music/../../')[0]!=0
        assert query(fs,'music/outside')[0]!=0
        assert query(fs,'photos')[3:5]==(0,0)
    finally:lib.fm01_posix_close(C.byref(fs))
    (root/'rootlink').symlink_to(root,target_is_directory=True)
    assert lib.fm01_posix_init(C.byref(fs),os.fsencode(root/'rootlink'))!=0
print('FM01 POSIX: 277 entries, full paging, dates, UTF-8, symlink containment: PASS')
