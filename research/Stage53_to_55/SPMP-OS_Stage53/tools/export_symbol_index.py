#!/usr/bin/env python3
"""Extract Ghidra FUN_ symbolic hints from packed DB, for navigation only.
These are auto-analysis symbol strings (NOT independently validated function boundaries).
"""
import csv, pathlib, re, argparse, collections
p=argparse.ArgumentParser();p.add_argument('dbdir',type=pathlib.Path);p.add_argument('outdir',type=pathlib.Path);args=p.parse_args()
args.outdir.mkdir(parents=True,exist_ok=True)
for db in sorted(args.dbdir.glob('*.db')):
 out=args.outdir/(db.stem.replace(' ','_')+'.csv')
 b=db.read_bytes(); names=sorted(set(x.decode() for x in re.findall(rb'FUN_[0-9a-f]{8}',b)))
 with out.open('w',newline='',encoding='utf-8') as o:
  w=csv.writer(o);w.writerow(['function_label_in_GZF','address','evidence_level']);w.writerows([(x,'0x'+x[4:],'Ghidra automatic label only') for x in names])
 print(out.name,'symbols',len(names))
