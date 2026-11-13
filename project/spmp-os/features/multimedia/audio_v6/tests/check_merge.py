#!/usr/bin/env python3
"""Reproducible install/rollback test, without changing the supplied Stage10.
Only writes to a temporary copy of the source tree.
"""
import argparse,pathlib,shutil,subprocess,tempfile,hashlib,json,sys

def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def cmd(patch,target,action,should_fail=False):
    p=subprocess.run([sys.executable,str(patch),'--target',str(target),'--'+action],text=True,capture_output=True)
    if (p.returncode==0)==should_fail:
        raise AssertionError(f'Unexpected {action} from {patch}: {p.stdout} {p.stderr}')
    return (p.stdout.strip() or p.stderr.strip())[:250]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--stage10',required=True,type=pathlib.Path)
    p.add_argument('--v3',required=True,type=pathlib.Path,help='CodecLab v3 root')
    p.add_argument('--v4',required=True,type=pathlib.Path,help='MP3 Player v4 root')
    p.add_argument('--v5',required=True,type=pathlib.Path,help='Audio v5 root')
    p.add_argument('--v6',type=pathlib.Path,default=pathlib.Path(__file__).resolve().parents[4],help='Audio v6 package root')
    a=p.parse_args()
    installers=[a.v3/'tools/mm_codec_patch.py',a.v4/'tools/mp3_patch.py',
                a.v5/'tools/mm_audio_patch.py',a.v6/'tools/mm_audio_v6_patch.py']
    for s in installers:
        if not s.is_file():p.error('missing installer '+str(s))
    with tempfile.TemporaryDirectory(prefix='spmp_mm_merge_') as td:
        root=pathlib.Path(td)/'SPMP-OS';shutil.copytree(a.stage10,root)
        core=next((root/'src/cores').rglob('*.c'))
        original={f.relative_to(root).as_posix():sha(f) for f in root.rglob('*') if f.is_file()}
        logs=[]
        for s in installers:
            logs.extend([cmd(s,root,'check'),cmd(s,root,'apply'),cmd(s,root,'verify')])
        assert all(sha(root/f)==h for f,h in original.items())
        core.write_bytes(core.read_bytes()+b'\n/* evolving game branch */\n')
        core_sha=sha(core)
        protected=root/'features/multimedia/audio_v6/include/spmp_mixer_v6.h'
        raw=protected.read_bytes();protected.write_bytes(raw+b'// modified\n')
        cmd(installers[-1],root,'remove',True)
        protected.write_bytes(raw)
        logs.append(cmd(installers[-1],root,'remove'))
        assert sha(core)==core_sha and not (root/'features/multimedia/audio_v6').exists()
        for s in installers[:-1]:cmd(s,root,'verify')
        logs.extend([cmd(installers[-1],root,'apply'),cmd(installers[-1],root,'verify')])
        assert sha(core)==core_sha
        result={'base_files_unchanged':len(original),'coinstall_v3_v4_v5_v6':True,
                'modified_game_source_preserved':True,'v6_tamper_protection':True,
                'independent_v6_removal_and_reinstall':True,'details':logs}
        print(json.dumps(result,ensure_ascii=False,indent=2));return 0
if __name__=='__main__':sys.exit(main())
