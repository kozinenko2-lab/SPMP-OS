#!/usr/bin/env python3
"""Synthetic media regression for the SPMP-OS Audio v5 host build.
Requires FFmpeg + Python stdlib; OpenCORE required only for optional AMR.
"""
import pathlib, subprocess, tempfile, array, json, shutil, re, sys
ROOT = pathlib.Path(__file__).resolve().parents[1]
FIX = ROOT / 'tests' / 'fixtures'
DEC = ROOT / 'build' / 'audio_decode'
PROBE = ROOT / 'build' / 'audio_probe'
AMR = ROOT / 'build' / 'audio_amr_decode'

def run(*cmd):
    q = subprocess.run([str(x) for x in cmd], capture_output=True, text=True)
    if q.returncode:
        raise RuntimeError(f"{' '.join(map(str, cmd))}: {q.returncode} {q.stderr[-1200:]}")
    return q.stdout, q.stderr

def samples(b):
    a=array.array('h');a.frombytes(pathlib.Path(b).read_bytes())
    if sys.byteorder != 'little':a.byteswap()
    return a

results = []
with tempfile.TemporaryDirectory(prefix='spmp_audio_v5_') as t:
    t=pathlib.Path(t)
    for ext, limit in [('wav', 0.0),('flac',0.0),('ogg',32.0)]:
        output = t/f'{ext}.s16'
        run(DEC,FIX/f'sample.{ext}',output)
        gold = t/f'{ext}.ref.s16'
        run('ffmpeg','-v','error','-y','-i',FIX/f'sample.{ext}', '-f','s16le', '-acodec','pcm_s16le',gold)
        a=samples(output);b=samples(gold)
        assert len(a)==len(b),(ext,len(a),len(b))
        error=sum(abs(int(x)-int(y)) for x,y in zip(a,b))/len(a)
        assert error<=limit,(ext,error)
        results.append({'name':ext,'samples':len(a),'mean_abs_error':round(error,2)})
    output=t/'ima.s16'
    run(DEC,FIX/'ima_stereo.wav',output)
    gold=t/'ima_ref.s16'
    run('ffmpeg','-v','error','-y','-i',FIX/'ima_stereo.wav','-f','s16le', '-acodec','pcm_s16le',gold)
    a=samples(output);b=samples(gold)
    assert len(a)==len(b)
    mae=sum(abs(int(x)-int(y)) for x,y in zip(a,b))/len(a)
    assert mae<=40,mae
    results.append({'name':'wav_ima_stereo','samples':len(a),'mean_abs_error':round(mae,2)})
    for inp,expected in [('sample.wma','fmt=4'),('sample_nb.amr','fmt=5'),('wb_no_data.awb','fmt=6')]:
        a,_=run(PROBE,FIX/inp)
        assert 'rc=0' in a and expected in a,a
        results.append({'name':'probe_'+inp,'result':a.strip()})
    if AMR.exists():
        output=t/'amr_nb.s16'
        run(AMR,FIX/'sample_nb.amr',output,'5')
        b=samples(output)
        assert len(b)==45*160,len(b)
        output=t/'amr_wb.s16'
        run(AMR,FIX/'wb_no_data.awb',output,'6')
        b=samples(output)
        assert len(b)==35*320,len(b)
        results.append({'name':'amr_nb_wb_via_external_opencore','samples':[7200,11200]})
    if (ROOT/'build/audio_backend_test').exists():
        run(ROOT/'build/audio_backend_test')
        results.append({'name':'external_backend_test','ok':True})
print(json.dumps(results,ensure_ascii=False,indent=2))
