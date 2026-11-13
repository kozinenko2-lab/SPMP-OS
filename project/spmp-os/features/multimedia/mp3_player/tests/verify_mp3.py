#!/usr/bin/env python3
"""Real bitstream regression: gapless, VBR, ID3, random seek, no-seek callbacks."""
import json,subprocess,wave,math,pathlib,struct,statistics,sys,tempfile,os,re
import numpy as np
ROOT=pathlib.Path(__file__).resolve().parents[1]
CLI=ROOT/'build/mp3_to_wav'

def cmd(*a, ok=True):
 r=subprocess.run(list(map(str,a)),stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if ok and r.returncode:
  raise AssertionError(f'command failed {a}\n{r.stderr.decode(errors="replace")[-900:]}')
 return r

def pcm(wav):
 with wave.open(str(wav),'rb') as f:
  assert f.getsampwidth()==2
  n,channels,sr=f.getnframes(),f.getnchannels(),f.getframerate()
  samples=np.frombuffer(f.readframes(n),dtype='<i2').copy()
 return n,channels,sr,samples

results=[]
with tempfile.TemporaryDirectory(prefix='spmp_mp3_test_') as t:
 t=pathlib.Path(t)
 tests=[('mpeg1_cbr',44100,2,'128k',None),('mpeg2_low',22050,1,'64k',None),
        ('mpeg25',11025,1,'32k',None),('mpeg1_48k',48000,2,'192k',None),
        ('mpeg1_32k',32000,2,'96k',None),('vbr_stereo',44100,2,None,2)]
 for name,sr,ch,bitrate,q in tests:
  out=t/(name+'.mp3');wav=t/(name+'.wav');ref=t/(name+'.reference.wav')
  expr='sin(2*PI*440*t)' + ('|sin(2*PI*682*t)' if ch==2 else '')
  gen=['ffmpeg','-v','error','-f','lavfi','-i',f'aevalsrc={expr}:s={sr}:d=1.41',
       '-c:a','libmp3lame','-id3v2_version','4','-metadata','title=Тест MP3','-metadata','artist=SPMP-OS']
  if bitrate:gen+=['-b:a',bitrate]
  if q is not None:gen+=['-q:a',str(q)]
  cmd(*(gen+['-y',out]))
  result=cmd(CLI,out,wav)
  stderr=result.stderr.decode(errors='replace')
  cmd('ffmpeg','-v','error','-i',out,'-acodec','pcm_s16le','-y',ref)
  n,c,r,b=pcm(wav);m,d,rr,a=pcm(ref)
  assert(c,r)==(ch,sr),(name,c,r)
  sample_diff=abs(n-m)
  # With an encoder Xing/LAME delay tag, frame-exact gapless is expected.
  assert sample_diff<=1152,(name,n,m,stderr)
  min_len=min(len(a),len(b))
  mae=float(np.abs(a[:min_len].astype(np.int32)-b[:min_len].astype(np.int32)).mean())
  assert mae<30,(name,mae,n,m,stderr)
  assert 'Тест MP3' in stderr and 'SPMP-OS' in stderr,(name,stderr)
  short=t/(name+'_noseek.wav')
  result2=cmd(CLI,out,short,'noseek','chunk=19')
  a_n=pcm(short)
  assert a_n[0] >= n-1152,(name,a_n[0],n)
  if name in ('mpeg1_cbr','vbr_stereo'):
   seek_out=t/(name+'_seek.wav')
   cmd(CLI,out,seek_out,'seek=700')
   seekn=pcm(seek_out)[0]
   assert 0<seekn<m,(name,seekn,m)
  results.append({'case':name,'sample_rate':sr,'channels':ch,'reference_samples':m,
                  'decoded_samples':n,'samples_diff':sample_diff,'mae_pcm16':round(mae,3),
                  'metadata':'OK','noseek_short_read':'OK'})
 # invalid header, huge tag, garbage/truncated cannot hang or exit successfully
 for label,content in [('garbage',b'not an mp3' * 80),('huge_ID3',b'ID3\x04\0\0\x7f\x7f\x7f\x7f' + b'\0'*128),
                       ('short',b'ID3\x04\0\0\0\0')]:
  f=t/(label+'.mp3');f.write_bytes(content)
  r=cmd(CLI,f,t/(label+'.wav'),ok=False)
  assert r.returncode!=0,(label,r.stderr)
  results.append({'case':label,'rejected':True})
 assert len(results)==9
 print(json.dumps(results,ensure_ascii=False,indent=2))
 (ROOT/'docs'/'MP3_REGRESSION_RESULTS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2))
