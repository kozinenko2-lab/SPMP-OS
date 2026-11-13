#!/usr/bin/env python3
"""Synthetic FAT16/FAT32 images. Never touches an actual block device."""
from __future__ import annotations
from pathlib import Path
import tempfile, subprocess, struct, zlib, random, os

P=Path(__file__).resolve().parents[1]
H=Path(os.environ.get('SPMP_FAT_HARNESS', str(P/'build'/'test_fat_disk')))
def put16(buf,at,x):struct.pack_into('<H',buf,at,x)
def put32(buf,at,x):struct.pack_into('<I',buf,at,x)
def sector(disk,at,data):
    with open(disk,'r+b') as f:
        f.seek(at*512);f.write(data)
def package(n=64):
    elf=bytearray(n);elf[:7]=b'\x7fELF\x01\x01\x01';put16(elf,18,40)
    hdr=bytearray(48);hdr[:8]=b'SPMPDB01';put32(hdr,8,1);put32(hdr,12,48)
    put32(hdr,16,n);put32(hdr,20,zlib.crc32(elf));put32(hdr,24,0x00420001);put32(hdr,28,1)
    return bytes(hdr+elf)
def de(name,attr,cluster,size=0,fat32=False):
    x=bytearray(32);x[:11]=name;x[11]=attr;put16(x,26,cluster&65535);put32(x,28,size)
    if fat32:put16(x,20,cluster>>16)
    return x

def make16(path,mbr=False,fragmented=False,missing=False,cycle=False):
    sectors=10000;offset=2048 if mbr else 0;total=sectors+offset
    with open(path,'wb') as f:f.truncate(total*512)
    if mbr:
        b=bytearray(512);b[446+4]=0x0e;put32(b,446+8,offset);put32(b,446+12,sectors);b[510:]=b'\x55\xaa';sector(path,0,b)
    b=bytearray(512);b[:3]=b'\xeb\x3c\x90';put16(b,11,512);b[13]=1;put16(b,14,1);b[16]=1
    put16(b,17,512);put16(b,19,sectors);put16(b,22,64);b[510:]=b'\x55\xaa';sector(path,offset,b)
    fat=bytearray(64*512);put16(fat,0,0xfff8);put16(fat,2,0xffff);put16(fat,4,0xffff)
    data=97;payload=package(1480 if fragmented else 64)
    if fragmented:
        # 3 -> 5 -> 4 (non-consecutive). For cycle test: 3 -> 3.
        put16(fat,6,3 if cycle else 5);put16(fat,10,4);put16(fat,8,0xffff)
        places=[3,5,4]
    else:
        put16(fat,6,0xffff);places=[3]
    with open(path,'r+b') as f:f.seek((offset+1)*512);f.write(fat)
    root=bytearray(512)
    if not missing:root[:32]=de(b'SPMPOS     ',0x10,2)
    sector(path,offset+65,root)
    folder=bytearray(512);folder[:32]=de(b'BOOT    PKG',0x20,3,len(payload));sector(path,offset+data,folder)
    for i,c in enumerate(places):sector(path,offset+data+c-2,payload[i*512:(i+1)*512].ljust(512,b'\x00'))
    return offset

def make32(path,invalid_fat=False):
    sectors=70000
    with open(path,'wb') as f:f.truncate(sectors*512)
    b=bytearray(512);b[:3]=b'\xeb\x58\x90';put16(b,11,512);b[13]=1;put16(b,14,32);b[16]=1
    put32(b,32,sectors);put32(b,36,600);put32(b,44,2);b[510:]=b'\x55\xaa';sector(path,0,b)
    fat=bytearray(512);put32(fat,0,0x0ffffff8);put32(fat,4,0xffffffff)
    for i in (2,3,4):put32(fat,i*4,0x0fffffff)
    sector(path,32,fat)
    data=32+600
    root=bytearray(512);root[:32]=de(b'SPMPOS     ',0x10,3,fat32=True);sector(path,data,root)
    payload=package()
    folder=bytearray(512);folder[:32]=de(b'BOOT    PKG',0x20,4,len(payload),fat32=True);sector(path,data+1,folder)
    sector(path,data+2,payload.ljust(512,b'\0'))
    if invalid_fat:
        b=bytearray(512);b[:]=b'\0'*512;sector(path,32,b)

def run(image,expect=1,budget=200,fail=0,verify=True):
    p=subprocess.run([str(H),str(image),str(int(expect)),str(budget),str(fail),str(int(verify))],capture_output=True,text=True)
    if p.returncode:raise AssertionError(f'{image}: {p.stdout} {p.stderr}')
    return p.stdout.strip()

def main():
    # Compile with the same public header and sources used for ARM target.
    cmd=['clang','-std=c11','-Wall','-Wextra','-Werror','-O2','-I'+str(P/'include'),
         str(P/'src/spmp_fat_reader.c'),str(P/'src/spmp_dualboot.c'),str(P/'tests/fat_disk_harness.c'),'-o',str(H)]
    if not os.environ.get('SPMP_FAT_HARNESS'):subprocess.run(cmd,check=True)
    count=0
    with tempfile.TemporaryDirectory(prefix='spmp_fat_test_') as temp:
        def image(name):return Path(temp)/name
        p=image('super16.img');make16(p);run(p);count+=1
        p=image('mbr16.img');make16(p,mbr=True);run(p);count+=1
        p=image('fragment16.img');make16(p,fragmented=True);run(p);count+=1
        p=image('cyclic16.img');make16(p,fragmented=True,cycle=True);run(p,0);count+=1
        p=image('missing16.img');make16(p,missing=True);run(p,0);count+=1
        p=image('super32.img');make32(p);run(p);count+=1
        p=image('badfat32.img');make32(p,invalid_fat=True);run(p,0);count+=1
        p=image('fail_io.img');make16(p)
        for err in range(1,6):run(p,0,fail=err);count+=1
        for budget in range(1,5):run(p,0,budget=budget);count+=1
        run(p,1,budget=5);count+=1
        # No valid file system must ever be interpreted as a valid package.
        p=image('garbage.img')
        with open(p,'wb') as f:f.write(bytes([0x01])*2048)
        run(p,0);count+=1
        # Deterministic single-sector fuzzing, bounded sector budget.
        p=image('fuzz16.img');make16(p)
        with open(p,'rb') as f:initial=f.read(512)
        rng=random.Random(0x420042)
        for i in range(int(os.environ.get('SPMP_FUZZ_COUNT','300'))):
            boot=bytearray(initial)
            at=rng.randrange(512);boot[at]^=1+rng.randrange(255)
            sector(p,0,boot)
            # Mutations may leave an intact and valid BPB; check only for
            # bounded completion, never treat acceptance as guaranteed.
            r=subprocess.run([str(H),str(p),'0','60','0','0'],capture_output=True,text=True)
            if r.returncode:
                r=subprocess.run([str(H),str(p),'1','60','0','0'],capture_output=True,text=True)
            assert r.returncode==0,(i,r.stderr)
            count+=1
    # Exercise Stage41 policy with the real read-only FAT read_package adapter.
    bridge=Path(os.environ.get('SPMP_BRIDGE_HARNESS', str(P/'build'/'test_bridge_disk')))
    if not os.environ.get('SPMP_BRIDGE_HARNESS'):subprocess.run(['clang','-std=c11','-O2','-Wall','-Wextra','-Werror',
       '-I'+str(P/'include'),str(P/'src/spmp_fat_reader.c'),
       str(P/'src/spmp_fat_bridge.c'),str(P/'src/spmp_dualboot.c'),
       str(P/'tests/test_bridge_disk.c'),'-o',str(bridge)],check=True)
    with tempfile.TemporaryDirectory(prefix='spmp_bridge_test_') as temp:
        good=Path(temp)/'ok.img';make16(good)
        bad=Path(temp)/'missing.img';make16(bad,missing=True)
        for path,expected,forced,returns in [
          (good,1,0,0),(good,0,1,0),(bad,0,0,0),(good,2,0,1)]:
            r=subprocess.run([str(bridge),str(path),str(expected),str(forced),str(returns)],
                capture_output=True,text=True)
            assert r.returncode==0,(r.stdout,r.stderr)
            count+=1
    print(f'STAGE42_TESTS: {count} FAT16/FAT32 / MBR / faults / fuzz checks PASS')
if __name__=='__main__':main()
