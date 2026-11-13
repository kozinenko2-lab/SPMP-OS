#!/usr/bin/env python3
"""Read-only ARM ELF diagnostic for the user's ORIGINAL, separately supplied
Sunplus libraries. Does not reverse proprietary algorithms into source code.
Optional -r performs ONLY ld.lld -r diagnostic rewriting, not usable decoding.
No .O vendor content is included in the SPMP-OS Audio v6 patch.
"""
import argparse, hashlib, json, pathlib, re, subprocess, sys
CODECS=('LIBWMA.O','LIBAMRNB.O','LIBAMRWB.O')

def proc(cmd):
    r=subprocess.run(cmd,text=True,capture_output=True,check=False)
    if r.returncode: raise RuntimeError(f'{cmd[0]} error {r.returncode}: {r.stderr.strip()[:300]}')
    return r.stdout

def audit(path):
    data=path.read_bytes()
    if len(data)<52 or data[:4]!=b'\x7fELF' or data[4]!=1 or data[5]!=1 or data[18:20]!=b'\x28\0':
        raise ValueError(f'{path}: not 32-bit ARM little-endian ELF')
    hdr=proc(['readelf','-h',str(path)])
    syms=proc(['readelf','-Ws',str(path)])
    em=re.search(r'Flags:\s*(.*)',hdr)
    result={'name':path.name,'sha256':hashlib.sha256(data).hexdigest(),
            'bytes':len(data),'elf_flags':em.group(1) if em else None,
            'exports':[],'undefined':[]}
    for line in syms.splitlines():
        fields=line.split()
        if len(fields)<8 or not fields[0].rstrip(':').isdigit(): continue
        _,value,size,typ,binding,visibility,section,name=fields[:8]
        if binding not in ('GLOBAL','WEAK'):continue
        if section=='UND':result['undefined'].append(name)
        elif typ=='FUNC' and not name.startswith('$') and (
             'decoder_' in name.lower() or name.lower().startswith('wmfile') or
             name.lower().startswith('wma_audec') or name.lower().startswith('amrnb_dec') or
             name.lower().startswith('amrwb_dec')):
            result['exports'].append({'name':name,'size':int(size) if size.isdigit() else 0})
    result['undefined']=sorted(set(result['undefined']))
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input',required=True,type=pathlib.Path,help='Original IMAGE/AUDIO or extracted audio directory')
    p.add_argument('--output',type=pathlib.Path,help='JSON report destination, not a firmware path')
    p.add_argument('--relink-diagnostic',type=pathlib.Path,help='OUTSIDE SPMP-OS source tree; unredistributable ELF tests only')
    args=p.parse_args()
    if not args.input.is_dir():p.error('input must be a directory with user-owned original .O files')
    report=[]
    for n in CODECS:
        path=args.input/n
        if not path.is_file():p.error(f'missing {path}')
        data=audit(path)
        if args.relink_diagnostic:
            out=args.relink_diagnostic.resolve()
            src=args.input.resolve()
            if out==src or out in src.parents or src in out.parents:p.error('relink output must be outside original firmware input')
            out.mkdir(parents=True,exist_ok=True)
            obj=out/(n+'.relinked.o')
            try:
                proc(['ld.lld','-m','armelf','-r',str(path),'-o',str(obj)])
                data['relink']={'status':'relocatable ELF created; no code reconstructed or executed',
                               'size_bytes':obj.stat().st_size}
            except RuntimeError as exc:data['relink']={'error':str(exc)}
        report.append(data)
    result={'codecs':report,'comment':'No evidence of runnable decoder ABI; do not bundle or flash relink outputs.'}
    if args.output:args.output.write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(json.dumps(result,indent=2,ensure_ascii=False))
    return 0
if __name__=='__main__':sys.exit(main())
