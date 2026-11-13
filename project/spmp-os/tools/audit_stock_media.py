#!/usr/bin/env python3
"""Inventories media resources and ELF relocatables from a previously extracted FSP."""
import argparse,json,subprocess
from pathlib import Path
AUDIO=['LIBMP3.O','LIBAAC.O','LIBWMA.O','LIBFLAC.O','LIBAPE.O','LIBAC3.O','LIBAMRNB.O','LIBAMRWB.O']
VIDEO=['H264_CODEC.IMG','MPG4ASPDEC.IMG','MPG2DEC_CODEC.IMG','MJPGDEC.IMG','DIVX3DEC.IMG','JPGDEC.IMG']
XML=['MUSIC/MUSICPLAYMAIN.XML','VIDEO/VIDEOPLAY.XML','RADIO/RADIOMAIN.XML','PHOTO/PHOTOPLAY.XML','EXPLORER/EXPLORERLIST.XML']
def check(base):
    out={'audio_arm_elf':[], 'video_mmpi':[], 'original_xml':[], 'cautions':[
         'Files listed in a multi-model firmware may not all be active on Defender.',
         'Hardware decoding, radio tuner, headphones detection and source licenses need independent verification.']}
    for name in AUDIO:
        p=base/'IMAGE/AUDIO'/name
        if not p.is_file():continue
        b=p.read_bytes();info={'file':name,'bytes':len(b),'elf':b[:4]==b'\x7fELF'}
        if info['elf']:
            result=subprocess.run(['readelf','-h',str(p)],capture_output=True,text=True)
            info['arm']=('Machine:                           ARM' in result.stdout)
            info['relocatable']=('REL (Relocatable file)' in result.stdout)
            if name in ('LIBMP3.O','LIBFLAC.O'):
                symbols=subprocess.run(['readelf','-Ws',str(p)],capture_output=True,text=True).stdout
                info['sample_symbols']=[x for x in ['mp3_decode_header','mp3_get_frame_count','flac_decoder_init','flac_decoder_run'] if x in symbols]
        out['audio_arm_elf'].append(info)
    for name in VIDEO:
        p=base/'IMAGE'/name
        if p.is_file():
            b=p.read_bytes()
            out['video_mmpi'].append({'file':name,'bytes':len(b),'magic':'MMPi' if b[:4]==b'MMPi' else 'unknown'})
    for name in XML:
        p=base/'RESOURCE/XML'/name
        out['original_xml'].append({'file':name,'exists':p.is_file()})
    return out
if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('extracted',type=Path);ap.add_argument('-o','--output',type=Path)
    a=ap.parse_args();s=json.dumps(check(a.extracted),ensure_ascii=False,indent=2)
    if a.output:a.output.write_text(s+'\n',encoding='utf8')
    else:print(s)
