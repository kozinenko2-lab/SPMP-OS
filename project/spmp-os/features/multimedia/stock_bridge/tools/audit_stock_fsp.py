#!/usr/bin/env python3
"""Read-only Sunplus SPMP8000 stock-media inventory. Python stdlib only.

Usage: python3 audit_stock_fsp.py --extracted Defender_MultiMix_FSP_All_Extracted.zip
                 [--fsp mmm-firmware-v3_FSP.img] --out audit.json
Never writes to the firmware. Does not execute extracted binary code.
"""
import argparse
import collections
import hashlib
import json
import pathlib
import re
import struct
import sys
import xml.etree.ElementTree as ET
import zipfile

AUDIO = ('LIBMP3.O','LIBAAC.O','LIBWMA.O','LIBFLAC.O','LIBAPE.O',
         'LIBAC3.O','LIBAMRNB.O','LIBAMRWB.O')
VIDEO = ('H264_CODEC.IMG','MPG4ASPDEC.IMG','MPG2DEC_CODEC.IMG',
         'MJPGDEC.IMG','DIVX3DEC.IMG')
IMAGE = ('JPGDEC.IMG','JPGENC_CODEC.IMG')
XML = ('MUSIC/MUSICPLAYMAIN.XML','MUSIC/MUSICPLAYMAINNEW.XML',
       'MUSIC/MUSICLISTMAIN.XML','MUSIC/MUSICALBUMLIST.XML',
       'RADIO/RADIOMAIN.XML','VIDEO/VIDEOPLAY.XML','VIDEO/VIDEOPLAYNEW.XML',
       'PHOTO/PHOTOPLAY.XML','PHOTO/PHOTOLIST.XML','PHOTO/PHOTOMATRIX.XML')
SEARCH_MMI = ('FM_SetFrequency','setFrequency2RadioModule',
              'RadioMainAutoScan','/FMRadio/MMIRadioSaveChannel.bin',
              'mp3_init','codec loading','jpeg size over limit 4096')
MAX_MEMBER = 10 * 1024 * 1024


def sha(b): return hashlib.sha256(b).hexdigest()


def streaming_hash(p):
    h=hashlib.sha256()
    with open(p,'rb') as f:
        for chunk in iter(lambda:f.read(1024*1024), b''):h.update(chunk)
    return h.hexdigest()


def u32(b,o):
    if o<0 or o+4>len(b):raise ValueError('truncated u32')
    return struct.unpack_from('<I',b,o)[0]


def elf(b):
    if len(b)<52 or b[:4]!=b'\x7fELF' or b[4]!=1 or b[5]!=1:
        return {'valid':False,'error':'not little-endian ELF32'}
    etype,emachine=struct.unpack_from('<HH',b,16)
    shoff=u32(b,32); flags=u32(b,36); entsize,shnum=struct.unpack_from('<HH',b,46)
    if shnum>1024 or entsize<40 or shoff+shnum*entsize>len(b):
        return {'valid':False,'error':'bad section table'}
    sections=[]
    for i in range(shnum):
        o=shoff+i*entsize
        name,type_,flg,addr,off,size,link,info,algn,ent=struct.unpack_from('<10I',b,o)
        if type_ != 8 and off+size>len(b):raise ValueError('ELF section out of bounds')
        sections.append((type_,off,size,link,ent))
    exports=[];imports=[];seen=set()
    for section in sections:
        type_,off,size,link,ent=section
        if type_!=2 or ent<16 or link>=len(sections):continue
        _,so,sz,_,_=sections[link];names=b[so:so+sz]
        for pos in range(off,off+size,ent):
            if pos+16>len(b):break
            st_name,st_val,st_size,st_info,st_other,st_shndx=struct.unpack_from('<IIIBBH',b,pos)
            if st_name>=len(names):continue
            end=names.find(b'\0',st_name)
            if end<0:continue
            name=names[st_name:end].decode('ascii','replace')
            bind=st_info>>4;typ=st_info&0x0f
            if not name or bind==0 or name.startswith('BuildAttributes') or name.startswith('Lib$$Request'):continue
            if name in seen:continue
            seen.add(name)
            if st_shndx==0:imports.append(name)
            elif typ==2:exports.append(name)
    api=[n for n in exports if re.search(r'(decoder|encoder|seek|analyz|version|flac_working_ram)',n,re.I)]
    return {'valid':True,'type':etype,'machine':emachine,'e_flags':hex(flags),
            'arm32_rel_softfp_candidate':etype==1 and emachine==40 and bool(flags&0x200),
            'exported_functions':len(exports),'api_symbols':sorted(api),
            'unresolved_symbols':sorted(imports)}


def mmpi(b):
    if len(b)<24 or b[:4]!=b'MMPi' or b[16:20]!=b'MMPd':
        return {'valid':False,'error':'missing MMPi/MMPd headers'}
    return {'valid':True,'magic':'MMPi','reported_total_bytes':u32(b,4),
            'matches_file_size':u32(b,4)==len(b),'mmpd_record_size':u32(b,20),
            'header_0_to_48_hex':b[:48].hex()}


def read_member(z,name):
    try:info=z.getinfo(name)
    except KeyError:return None
    if info.file_size>MAX_MEMBER:raise ValueError('oversized member '+name)
    data=z.read(info)
    if len(data)!=info.file_size:raise ValueError('size mismatch '+name)
    return data


def xml_info(b,all_paths):
    d={'bytes':len(b),'sha256':sha(b)}
    try:root=ET.fromstring(b)
    except ET.ParseError as exc:
        d['strict_xml_valid']=False
        d['xml_error']=str(exc)
        return d
    d['strict_xml_valid']=True
    d['window_class']=root.attrib.get('ClassName')
    d['window_name']=root.attrib.get('WindowName')
    d['canvas']=[int(root.attrib.get('w','0')),int(root.attrib.get('h','0'))]
    d['tags']=dict(collections.Counter(e.tag for e in root.iter()).most_common())
    d['control_ids']=[e.attrib['id'] for e in root.iter() if 'id' in e.attrib and e.tag!='state']
    d['keys']={e.tag:(e.text or '').strip() for e in root.findall('./KeyDef/state/*')}
    refs=[(e.text or '').strip() for e in root.iter('Img_Src') if (e.text or '').strip()]
    d['image_resource_references']=refs
    missing=[p for p in refs if p.lstrip('/').upper() not in all_paths]
    d['missing_resource_references']=missing
    return d


def run(zip_path,raw_fsp=None):
    result={'schema':2,'source_extracted':pathlib.Path(zip_path).name,
            'WARNING':'File presence != working device driver or vendor ABI compatibility',
            'audio':{},'video':{},'image':{},'xml':{},'stock_mmi_string_evidence':{}}
    with zipfile.ZipFile(zip_path,'r') as z:
        all_paths={p.upper() for p in z.namelist()}
        result['entry_count']=len(z.namelist())
        for n in AUDIO:
            path='IMAGE/AUDIO/'+n;b=read_member(z,path)
            result['audio'][n]=({'found':False} if b is None else
                               {'found':True,'bytes':len(b),'sha256':sha(b),'elf':elf(b)})
        for grp,names in [('video',VIDEO),('image',IMAGE)]:
            for n in names:
                path='IMAGE/'+n;b=read_member(z,path)
                result[grp][n]=({'found':False} if b is None else
                                {'found':True,'bytes':len(b),'sha256':sha(b),'header':mmpi(b)})
        for n in XML:
            b=read_member(z,'RESOURCE/XML/'+n)
            result['xml'][n]=({'found':False} if b is None else
                              {'found':True,**xml_info(b,all_paths)})
        for n in ['H264','MPG2DEC','JPGDEC','DIVX3','MJPG','MPG4']:
            paths=[p for p in z.namelist() if p.startswith('IMAGE/') and p.upper().endswith('.IMG') and n in p.upper()]
            result.setdefault('module_families',{})[n]=sorted(paths)
        b=read_member(z,'IMAGE/8000_MMI.RAP')
        if b is not None:
            result['stock_mmi_sha256']=sha(b)
            for needle in SEARCH_MMI:result['stock_mmi_string_evidence'][needle]=needle.encode('ascii') in b
        result['radio_assets']=[p for p in z.namelist() if p.startswith('RESOURCE/RADIO/')]
    if raw_fsp:
        p=pathlib.Path(raw_fsp)
        with p.open('rb') as f:h=f.read(32)
        result['raw_fsp']={'filename':p.name,'bytes':p.stat().st_size,
                           'header_magic':h[:4].hex(), 'embedded_FSP_string':h[16:20]==b'FSP ',
                           'sha256':streaming_hash(p)}
    return result


def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--extracted',required=True,help='Unmodified extracted firmware ZIP')
    a.add_argument('--fsp',help='Optional original FSP image for SHA256 reference')
    a.add_argument('--out',required=True,help='JSON output (NOT a firmware patch)')
    args=a.parse_args()
    data=run(args.extracted,args.fsp)
    pathlib.Path(args.out).parent.mkdir(parents=True,exist_ok=True)
    pathlib.Path(args.out).write_text(json.dumps(data,indent=2,ensure_ascii=False,sort_keys=True)+'\n',encoding='utf-8')
    print('audited',data['entry_count'],'files; audio',sum(v['found'] for v in data['audio'].values()),
          'video',sum(v['found'] for v in data['video'].values()),
          'xml',sum(v['found'] for v in data['xml'].values()))

if __name__=='__main__':
    try:main()
    except (ValueError,OSError,zipfile.BadZipFile) as exc:
        print('AUDIT FAILED:',exc,file=sys.stderr);sys.exit(1)
