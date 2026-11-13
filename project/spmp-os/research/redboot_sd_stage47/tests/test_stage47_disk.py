#!/usr/bin/env python3
"""Run stage47 through real Stage42 FAT scanner on synthetic IMG files only."""
from pathlib import Path
import subprocess,tempfile,sys
ROOT=Path(__file__).resolve().parents[1]
S42=ROOT.parent/'redboot_sd_stage42'
sys.path.insert(0,str(S42/'tests'))
from test_fat_reader import make16,make32
BIN=ROOT/'build'/'test_stage47_disk'
def run(img,mode,sd,stock):
 p=subprocess.run([str(BIN),str(img),str(mode),str(sd),str(stock)],capture_output=True,text=True,timeout=12)
 if p.returncode:raise AssertionError(f'{img.name},mode={mode}: {p.stdout}\n{p.stderr}')
 return p.stdout.strip()
def main():
 n=0
 with tempfile.TemporaryDirectory(prefix='spmp47_synthetic_fat_') as tmp:
  d=Path(tmp)
  good=d/'good16.img';make16(good)
  for mode in range(8):
   sd=int(mode==0 or mode==5)
   stock=int(mode!=0)
   print(run(good,mode,sd,stock));n+=1
  f=d/'mbr16.img';make16(f,mbr=True);print(run(f,0,1,0));n+=1
  f=d/'fragment16.img';make16(f,fragmented=True);print(run(f,0,1,0));n+=1
  f=d/'missing16.img';make16(f,missing=True);print(run(f,0,0,1));n+=1
  f=d/'cycle16.img';make16(f,fragmented=True,cycle=True);print(run(f,0,0,1));n+=1
  f=d/'good32.img';make32(f);print(run(f,0,1,0));n+=1
  f=d/'badfat32.img';make32(f,invalid_fat=True);print(run(f,0,0,1));n+=1
 print('STAGE47 synthetic FAT/dualboot integration:',n,'scenarios passed')
if __name__=='__main__':main()
