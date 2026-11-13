#!/usr/bin/env python3
"""Read-only analysis of the user's working BattleGround control sample.
Emits addresses, hashes and ABI observations, never decoded game bytes/assets.
The device-running provenance is the user's statement, not our hardware test.
"""
import argparse, copy, hashlib, json, struct
from pathlib import Path
from ngame_inspect import inspect as inspect_ngame, transform
from native_memory_plan import plan, region, contains, overlap

GAME_SHA256='ffb5dd9fc4f26f299010a78e9be96ab3c7d05c6c64630f68caa21f315ff2e017'
PAYLOAD_SHA256='f312eafba8f7dbf93afb5897ed127d1081e929181b15386bbaf191912ab539da'
ENTRY=0xa00000
# Verified call instructions in the private Ghidra analysis. Values are API offsets.
API_CALLS={0xa00044:4,0xa0005c:4,0xa00070:4,0xa00084:4,0xa000a8:4,0xa000b8:4,
    0xa212c4:0x38,0xa212e8:0x90,0xa21308:0x54,0xa21318:0x8c,
    0xa21334:0x44,0xa21344:0x3c,0xa21364:0xc4,0xa21374:0x10,
    0xa32358:0xd4,0xa32374:0x124,0xa323c8:0x100,0xa323e0:0x108}

def sha(data):return hashlib.sha256(data).hexdigest()

def decode(data):
    if sha(data)!=GAME_SHA256:raise ValueError('Exact user-provided BattleGround control sample required')
    structural=inspect_ngame(data)
    if structural['status']!='structural_match':raise ValueError('Control NGame structural mismatch')
    payload=bytearray(data[128:128+structural['payload_bytes']])
    payload[:1024]=transform(payload[:1024])
    if sha(payload)!=PAYLOAD_SHA256:raise ValueError('Decoded control payload SHA-256 mismatch')
    return bytes(payload),structural

class ControlImage:
    def __init__(self,payload):self.payload=payload
    def word(self,address):
        offset=address-ENTRY
        if offset<0 or offset+4>len(self.payload):raise ValueError('Word outside control payload')
        return struct.unpack_from('<I',self.payload,offset)[0]
    def literal(self,address):
        w=self.word(address)
        if w&0x0f7f0000!=0x051f0000:raise ValueError('Expected PC-relative ARM LDR')
        return self.word(address+8+(w&4095)*(1 if w&0x800000 else -1))
    def branch(self,address):
        w=self.word(address)
        if w&0x0f000000!=0x0b000000:raise ValueError('Expected ARM BL')
        offset=w&0xffffff
        if offset&0x800000:offset-=0x1000000
        return (address+8+offset*4)&0xffffffff

def resource_summary(im):
    counts={'wave':0,'midi':0,'other':0};wave_formats=set();at=0xd33820
    for count in range(256):
        offset=at-ENTRY;name=im.payload[offset:offset+16].split(b'\0')[0]
        if name==b'TAEND':break
        address=im.word(at+16);start=address-ENTRY
        if start<0 or start+44>len(im.payload):raise ValueError('Resource outside payload')
        prefix=im.payload[start:start+44]
        if prefix[:4]==b'RIFF' and prefix[8:12]==b'WAVE':
            if prefix[12:16]!=b'fmt ':raise ValueError('Unexpected control WAV layout')
            tag,channels,rate,byte_rate,block,bits=struct.unpack_from('<HHIIHH',prefix,20)
            wave_formats.add((tag,channels,rate,bits));counts['wave']+=1
        elif prefix[:4]==b'MThd':counts['midi']+=1
        else:counts['other']+=1
        at+=20
    else:raise ValueError('Missing resource terminator')
    return {'table':'0xd33820','entry_bytes':20,'entries':count,'terminator':hex(at),
        'counts':counts,'wave_formats':[{'format_tag':f[0],'channels':f[1],'rate_hz':f[2],'bits':f[3]}
            for f in sorted(wave_formats)],'assets_exported':False}

def inspect(data):
    payload,structural=decode(data);im=ControlImage(payload)
    expected_literals={0xa00000:0xd3e000,0xa00004:0xd3ef0c,0xa00030:0xd3e000,
        0xa00048:0xd3fa0b,0xa00060:0xd3f60c}
    for address,value in expected_literals.items():
        if im.literal(address)!=value:raise ValueError('Unexpected startup literal '+hex(address))
    for address,target in {0xa00090:0xa32fa4,0xa00098:0xa32fb4,0xa000bc:0xa322e0,
                           0xa212a8:0xa32f0c,0xa32324:0xa21294}.items():
        if im.branch(address)!=target:raise ValueError('Unexpected startup call '+hex(address))
    calls=[]
    for address,slot in API_CALLS.items():
        word=im.word(address)
        if word&0x0ff0f000!=0x0590f000 or word&0xfff!=slot:
            raise ValueError('Unexpected API call instruction '+hex(address))
        calls.append({'address':hex(address),'slot_offset':hex(slot),'base_register':(word>>16)&15})
    extra=data[128+len(payload):]
    if extra[:3]!=b'\xff\xd8\xff':raise ValueError('Unexpected control tail format')
    return {'source_sha256':GAME_SHA256,'decoded_payload_sha256':sha(payload),
        'provenance':'User reports this exact uploaded game runs on the console; no new hardware test by us.',
        'container_bytes':len(data),'ngame':structural,'tail_format':'JPEG; contents and EXIF not exported',
        'load_regions':[
            region('raw_container_read',ENTRY,len(data),'load'),
            region('after_header_shift_including_tail',ENTRY,len(data)-128,'load')],
        'run_regions':[
            region('decoded_payload',ENTRY,len(payload)),
            region('payload_to_bss_gap',ENTRY+len(payload),0xd3e000-ENTRY-len(payload)),
            region('startup_zeroed_bss',0xd3e000,0xf0c),
            region('bss_end_to_linker_end_unresolved',0xd3ef0c,0x700),
            region('heap_alignment_gap',0xd3f60c,0x1f4),
            region('private_heap_arena',0xd3f800,0xe00000-0xd3f800)],
        'startup':{'entry':hex(ENTRY),'api_argument':'r0','api_saved_at':'0xd3e000',
            'bss_clear_start':'0xd3e000','bss_clear_end_exclusive':'0xd3ef0c','bss_zero_words':0xf0c//4,
            'linker_end_literal':'0xd3f60c','heap_base':'0xd3f800','heap_limit_exclusive':'0xe00000',
            'main_entry':'0xa322e0','returns_zero_after_main':True,'stack':'inherits caller SP; no startup stack switch'},
        'verified_api_calls':calls,'resources':resource_summary(im),
        'private_allocator':{'configuration_table':'0xd379f4',
            'fixed_blocks':[{'payload_bytes':size,'count':count} for size,count in
                struct.iter_unpack('<II',payload[0x3379f4:0x337a1c])],
            'general_heap_metadata_base':hex(0xd3f800+sum((size+16)*count for size,count in
                struct.iter_unpack('<II',payload[0x3379f4:0x337a1c])))},
        'limits':['Private Ghidra text/data split at 0xa3b6ac is an analysis partition, not recovered ELF sections.',
                  'The 0x700 bytes after the BSS-clear end are reserved conservatively; their purpose remains unresolved.',
                  'This inspection neither runs gameplay nor certifies another NGame file or SPMP-OS on hardware.']}

def memory_comparison(observation,profile):
    """Compare observed fixed game ranges with the existing *supplied* allocation model."""
    p=copy.deepcopy(profile)
    p['sizes']={'payload_bytes':observation['ngame']['payload_bytes'],
        'tail_bytes':observation['ngame']['extra_bytes'],'bss_bytes':0,
        'heap_bytes':0,'private_stack_reserve_bytes':0}
    result=plan(p);errors=list(result['errors']);runtime=observation['run_regions']
    for r in runtime:
        if not contains(result['allocation'],r):errors.append(r['name']+': outside supplied allocation')
        for protected in result['protected']:
            if overlap(r,protected):errors.append(r['name']+': overlaps '+protected['name'])
    for i,r in enumerate(runtime):
        if any(overlap(r,other) for other in runtime[:i]):errors.append(r['name']+': overlaps another run region')
    result.update(status='rejected' if errors else 'model_consistent',errors=errors,run_phase=runtime,
        game_source_sha256=observation['source_sha256'],
        origin='Observed control-game ranges compared with supplied allocation assumptions; not a device RAM capture.',
        load_tail_overlaps_future_bss=any(overlap(r,runtime[2]) for r in observation['load_regions']))
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('game',type=Path)
    p.add_argument('--output',type=Path);p.add_argument('--allocation-profile',type=Path)
    a=p.parse_args();result=inspect(a.game.read_bytes())
    if a.allocation_profile:result['memory_comparison']=memory_comparison(result,json.loads(a.allocation_profile.read_text()))
    output=json.dumps(result,indent=2)+'\n'
    if a.output:a.output.write_text(output)
    else:print(output,end='')
