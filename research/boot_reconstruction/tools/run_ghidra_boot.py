#!/usr/bin/env python3
"""Import verified user GZF copies, validate bytes, repair scoped entries and export.
No flashing or updates to original GZF. Requires Ghidra 12.2 and JDK 25.
All scratch databases/raw extracts are local; output contains selected analysis text.
"""
import argparse,csv,hashlib,json,os,subprocess,tempfile
from pathlib import Path
from audit_boot import audit,H
BASE=Path(__file__).resolve().parents[1]
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def run(inputs,ghidra,jdk,scratch,output):
    source_audit=audit(inputs)  # Check both OEM hashes and static baseline first.
    config=json.loads((BASE/'ghidra_targets.json').read_text())
    if output.exists():raise ValueError('Use a new output directory to prevent mixing exports')
    output.mkdir(parents=True);scratch.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['JAVA_HOME']=str(jdk);env['PATH']=str(jdk/'bin')+os.pathsep+env['PATH']
    results=[]
    with tempfile.TemporaryDirectory(prefix='ghidra-boot-',dir=scratch) as tmp:
        workspace=Path(tmp);projects=workspace/'projects';projects.mkdir()
        for plan in config['programs']:
            gzf=inputs/'ghidra/user_GZF'/plan['gzf_name']
            if digest(gzf)!=plan['gzf_sha256']:raise ValueError('Unexpected GZF hash: '+gzf.name)
            if plan['variant']:
                v=source_audit['redboot_variants'][plan['variant']-1]
                blob=(inputs/'firmware/originals/Images_RedBoot_201000427.img').read_bytes()
                start=int(v['source_offset'],16);blob=blob[start:start+v['bytes']]
            else:blob=(inputs/'firmware/originals/8000_MMI.RAP').read_bytes()
            raw=workspace/(plan['id']+'.raw');raw.write_bytes(blob)
            if digest(raw)!=plan['raw_sha256']:raise ValueError('Raw slice hash mismatch')
            out=output/plan['id'];out.mkdir()
            pre=[t['address']+':'+t['first_word']+':'+t['label'] for t in plan['targets']]
            post=[t['address']+':'+t['label'] for t in plan['targets']]
            command=[str(ghidra/'support/analyzeHeadless'),str(projects),plan['id'],
                '-import',str(gzf),'-noanalysis','-scriptPath',str(BASE/'ghidra_scripts'),
                '-preScript','SPMP_VerifyAndPrepare.java',str(raw),plan['raw_sha256'],str(out/'verification.tsv'),*pre,
                '-postScript','SPMP_ExportBoot.java',str(out),*post]
            with (out/'headless.log').open('w') as log:
                proc=subprocess.run(command,env=env,stdout=log,stderr=subprocess.STDOUT)
            # Headless may exit 0 despite script errors. Require complete fresh artifacts.
            if proc.returncode or not (out/'status.tsv').is_file() or not (out/'verification.tsv').is_file():
                raise RuntimeError('Ghidra failed; see '+str(out/'headless.log'))
            with (out/'status.tsv').open() as f:rows=list(csv.DictReader(f,delimiter='\t'))
            if len(rows)!=len(post) or any(r['status']!='OK' for r in rows):
                raise RuntimeError('Missing/failed target decompilation: '+plan['id'])
            for t in plan['targets']:
                if not (out/(t['label']+'.c')).is_file():raise RuntimeError('Missing pseudo-C export')
            results.append({'program':plan['id'],'gzf_sha256':plan['gzf_sha256'],
                'raw_sha256':plan['raw_sha256'],'successful_targets':len(rows),
                'program_metadata':(out/'program.txt').read_text()})
            print(plan['id'],len(rows),'verified selected functions',flush=True)
    metadata={'ghidra_application_properties':(ghidra/'Ghidra/application.properties').read_text(),
              'source_hashes':H,'programs':results,'limits':['Static decompiler output; not buildable OEM sources.',
              'Original GZF language is ARM:LE:32:v8; target hardware is ARMv5TE. CP15/status-register semantics need manual ARMv5 review.',
              'No console execution or original GZF changes.']}
    (output/'export_summary.json').write_text(json.dumps(metadata,indent=2)+'\n')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for name in ['inputs','ghidra','jdk','scratch','output']:p.add_argument('--'+name,type=Path,required=True)
    a=p.parse_args();run(*(getattr(a,n).resolve() for n in ['inputs','ghidra','jdk','scratch','output']))
