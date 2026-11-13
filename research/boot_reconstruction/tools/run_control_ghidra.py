#!/usr/bin/env python3
"""Prepare exact control sample for PRIVATE Ghidra analysis outside this checkout.
Never publishes a decoded game, assets, decompilation or Ghidra database.
"""
import argparse,os,subprocess
from pathlib import Path
from control_game_inspect import decode,sha,GAME_SHA256
BASE=Path(__file__).resolve().parents[1]
REPO=BASE.parents[1]
TARGETS=['a00000:control_entry','a21294:control_graphics_initialize','a322e0:control_main',
    'a32fa4:control_heap_set_limit','a32fb4:control_heap_initialize',
    'a330d4:control_heap_layout_initialize','a33374:control_heap_blocks_initialize']

def private_directory(path):
    path=path.resolve()
    if path==REPO or REPO in path.parents:raise ValueError('Private Ghidra workspace must be outside public checkout')
    return path

def run(game,work,ghidra,jdk):
    work=private_directory(work)
    if work.exists() and any(work.iterdir()):raise ValueError('Use a fresh empty private workspace')
    raw=game.read_bytes();payload,_=decode(raw)
    work.mkdir(parents=True,exist_ok=True);os.chmod(work,0o700)
    binary=work/'payload.private';binary.write_bytes(payload)
    project=work/'project';project.mkdir();exports=work/'analysis_private';exports.mkdir()
    env=dict(os.environ,JAVA_HOME=str(jdk),PATH=str(jdk/'bin')+os.pathsep+os.environ['PATH'])
    command=[str(ghidra/'support/analyzeHeadless'),str(project),'control_game','-import',str(binary),
        '-loader','BinaryLoader','-loader-baseAddr','0xa00000','-processor','ARM:LE:32:v5t',
        '-cspec','apcs','-scriptPath',str(BASE/'ghidra_scripts'),'-preScript','SPMP_PrepareControlGame.java',
        '-analysisTimeoutPerFile','180','-postScript','SPMP_ExportBoot.java',str(exports),*TARGETS]
    with (work/'ghidra.log').open('w') as log:subprocess.run(command,env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
    # Headless can log script errors without a nonzero process exit.
    if not (exports/'control_entry.c').exists():raise RuntimeError('Missing private export; inspect ghidra.log')
    if sha(game.read_bytes())!=GAME_SHA256:raise RuntimeError('Original control file changed')
    print('Private analysis finished:',work,'(do not publish contents)')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',required=True,type=Path)
    p.add_argument('--private-work',required=True,type=Path);p.add_argument('--ghidra',required=True,type=Path)
    p.add_argument('--jdk',required=True,type=Path);a=p.parse_args()
    run(a.game.resolve(),a.private_work,a.ghidra.resolve(),a.jdk.resolve())
