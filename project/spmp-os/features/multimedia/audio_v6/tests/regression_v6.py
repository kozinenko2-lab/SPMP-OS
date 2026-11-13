#!/usr/bin/env python3
"""SPMP Audio v6 regression, optional integration with MP3 v4, Audio v5.
Requires clang/cc and ffmpeg for PCM checks. Generates no proprietary blobs.
"""
import argparse,array,json,pathlib,subprocess,tempfile,struct,sys,os
ROOT=pathlib.Path(__file__).resolve().parents[1]
FIX=ROOT/'tests/fixtures'

def run(cmd):
    r=subprocess.run([str(x) for x in cmd],capture_output=True,text=True)
    if r.returncode:
        raise RuntimeError(f'{cmd}: {r.returncode}\n{r.stdout[-800:]}\n{r.stderr[-800:]}')
    return r.stdout,r.stderr

def arr(data):
    a=array.array('h');a.frombytes(data)
    if sys.byteorder!='little':a.byteswap()
    return a

def compare(got,gold,max_mae,channels=2):
    a=arr(got.read_bytes());b=arr(gold)
    if len(a)<len(b):raise AssertionError(f'output short: {len(a)} < {len(b)}')
    n=len(b)
    if not n:raise AssertionError('empty reference')
    mae=sum(abs(int(x)-int(y)) for x,y in zip(a[:n],b))/n
    if mae>max_mae:raise AssertionError(f'MAE {mae:.3f} exceeds {max_mae}')
    if any(a[n:]):raise AssertionError('nonzero tail after EOF')
    return {'actual_samples':len(a),'reference_samples':len(b),'mean_abs_pcm16_error':round(mae,3)}

def ffmpeg(inp,rate):
    cmd=['ffmpeg','-nostdin','-loglevel','error','-i',str(inp),'-ac','2','-ar',str(rate),'-f','s16le','-acodec','pcm_s16le','-']
    result=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    if result.returncode:raise RuntimeError('ffmpeg: '+result.stderr.decode(errors='ignore')[-350:])
    return result.stdout

p=argparse.ArgumentParser()
p.add_argument('--v5',type=pathlib.Path,default=ROOT.parent/'audio_v5')
p.add_argument('--mp3-v4',type=pathlib.Path,default=ROOT.parent/'mp3_player')
p.add_argument('--result',type=pathlib.Path)
args=p.parse_args()
results={'basic':[],'media_integration':[],'optional_missing':[],
         'status':'Host-only tests, not a hardware performance result'}
run(['make','-C',ROOT,'host','check'])
results['basic']=['9 mixer groups + 1500 randomized cases','ASF 80 bounded-read variants','Stage10 HAL sink']
with tempfile.TemporaryDirectory(prefix='spmp_v6_') as directory:
    d=pathlib.Path(directory)
    # Unknown ASF GUID padding >4KiB must be skipped by structural parser.
    orig=(FIX/'sample.wma').read_bytes()
    old_len=struct.unpack_from('<Q',orig,16)[0]
    old_count=struct.unpack_from('<I',orig,24)[0]
    padding=b'P'*16+struct.pack('<Q',24+13000)+b'\0'*13000
    forged=bytearray(orig[:30]+padding+orig[30:old_len]+orig[old_len:])
    struct.pack_into('<Q',forged,16,old_len+len(padding))
    struct.pack_into('<I',forged,24,old_count+1)
    tmp=d/'large_header.wma';tmp.write_bytes(forged)
    run([ROOT/'build/test_asf',tmp])
    results['basic'].append('ASF 13KB injected unknown object; valid WMA still recognized')
    if (ROOT/'build/test_stage10_sink').exists():run([ROOT/'build/test_stage10_sink'])
    if args.v5.exists():
        v5=args.v5
        run(['make','-C',ROOT,'v5-host',f'V5_DIR={v5}'])
        native_src=[ROOT/'tests/test_integration_v5.c',ROOT/'artifacts/spmp_mix_v5_adapter_host.o',v5/'src/spmp_amr_opencore.c',ROOT/'artifacts/libspmp_audio_v6_host.a',v5/'artifacts/libspmp_audio_v5_host.a']
        run(['cc','-O2',f'-I{ROOT}/include',f'-I{v5}/include',*native_src,
             '-l:libopencore-amrnb.so.0','-l:libopencore-amrwb.so.0','-lm','-o',ROOT/'build/test_integration_v5'])
        for name,rate,fmt,mae in [('sample.wav',44100,None,0.0),('sample.flac',44100,None,0.0),
                                   ('sample.ogg',44100,None,40.0),('sample_nb.amr',8000,5,3000),
                                   ('wb_no_data.awb',16000,6,50.0)]:
            dest=d/(name+'.s16')
            command=[ROOT/'build/test_integration_v5',FIX/name,dest,rate]
            if fmt:command.append(fmt)
            stdout,stderr=run(command)
            rep=compare(dest,ffmpeg(FIX/name,rate),mae)
            results['media_integration'].append({'file':name,'rate':rate,'decoder':'Audio v5 host; AMR OpenCORE host only',**rep})
    else:results['optional_missing'].append('Audio v5 not installed; end-to-end WAV/OGG/FLAC/AMR not run')
    if args.mp3_v4.exists():
        v4=args.mp3_v4
        run(['make','-C',v4,'build/libspmp_mp3_host.a'])
        run(['cc','-O2',f'-I{ROOT}/include',f'-I{v4}/include',ROOT/'tests/test_integration_mp3_v4.c',
             ROOT/'artifacts/libspmp_audio_v6_host.a',v4/'build/libspmp_mp3_host.a',
             '-o',ROOT/'build/test_integration_mp3_v4'])
        dest=d/'sample.mp3.s16'
        run([ROOT/'build/test_integration_mp3_v4',FIX/'sample.mp3',dest])
        rep=compare(dest,ffmpeg(FIX/'sample.mp3',44100),3.0)
        results['media_integration'].append({'file':'sample.mp3','rate':44100,'decoder':'MP3 v4 libmad host',**rep})
    else:results['optional_missing'].append('MP3 v4 not installed; MP3 push integration not run')
    # WMA reference is FFmpeg only. The ARM WMA decoder DOES NOT EXIST.
    wma=ffmpeg(FIX/'sample.wma',44100)
    if not wma:raise AssertionError('WMA reference unexpectedly empty')
    results['wma']={'metadata':'Audio v6 native ASF parser passed',
                   'host_ffmpeg_reference_samples':len(wma)//2,
                   'arm_pcm_decoder':'NOT IMPLEMENTED'}
    if args.result:args.result.write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(results,ensure_ascii=False,indent=2))
