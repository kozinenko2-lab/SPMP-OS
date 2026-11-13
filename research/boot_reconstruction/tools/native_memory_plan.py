#!/usr/bin/env python3
"""Bounded, phase-aware host planner for this image's kind-6 BIN allocation.
Input ranges are supplied observations, never discovered physical RAM.
Does not generate a container, linker image, pointer binding or device writes.
"""
import argparse,json
from pathlib import Path
from audit_boot import H
from native_lifecycle_models import POOL_START,POOL_END,BIN_ENTRY,AUDIO_BASE,AUDIO_PCM_BYTES,TONELIB_CAPACITY

def integer(value,name):
    if isinstance(value,str):value=int(value,0)
    if isinstance(value,bool) or not isinstance(value,int) or not 0<=value<=0xffffffff:
        raise ValueError(name+': expected unsigned 32-bit integer')
    return value

def region(name,start,size,phase='run'):
    start=integer(start,name+'.base');size=integer(size,name+'.bytes')
    if size==0 or start+size>0x100000000:raise ValueError(name+': empty or wrapped range')
    return {'name':name,'start':start,'end_exclusive':start+size,'bytes':size,'phase':phase}

def overlap(a,b):return a['start']<b['end_exclusive'] and b['start']<a['end_exclusive']
def contains(a,b):return a['start']<=b['start'] and b['end_exclusive']<=a['end_exclusive']
def align_up(value):return (value+15)&~15

def plan(profile):
    errors=[]
    if profile['source_sha256']!=H['8000_MMI.RAP']:raise ValueError('Unsupported source SHA-256')
    if profile.get('native_kind')!=6:raise ValueError('Only kind-6 BIN is modeled')
    allocation=region('supplied_game_allocation',profile['allocation']['base'],profile['allocation']['bytes'])
    if not allocation['start'] or allocation['start']%16:errors.append('allocation is NULL or not 16-byte aligned')
    if allocation['bytes']<0x8000:errors.append('allocation is smaller than OEM game-pool minimum request')
    pool=region('configured_oem_pool',POOL_START,POOL_END-POOL_START)
    if not contains(pool,allocation):errors.append('allocation outside configured OEM game pool')
    if not allocation['start']<=BIN_ENTRY<allocation['end_exclusive']:errors.append('BIN entry not in supplied allocation')
    protected=[region('mmi_code_data',0x280000,0x56ad90-0x280000),
        region('mmi_bss',0x56ad90,0x6c8fe0-0x56ad90),region('mmi_extra_data',0x6c8fe0,0x6ff820-0x6c8fe0),
        region('audio_workspace_and_tonelib',AUDIO_BASE,AUDIO_PCM_BYTES+TONELIB_CAPACITY)]
    frames=[]
    if len(profile['framebuffers'])!=2:errors.append('two framebuffer observations required')
    for i,p in enumerate(profile['framebuffers']):
        f=region('game_virtual_framebuffer_'+str(i),p['base'],p['bytes']);frames.append(f)
        if not f['start'] or f['start']%16 or f['bytes']<320*240*2:errors.append(f['name']+': invalid pointer/alignment/extent')
        if not contains(pool,f):errors.append(f['name']+': outside configured OEM pool')
        if overlap(f,allocation):errors.append(f['name']+': overlaps game allocation')
        if any(overlap(f,other) for other in frames[:-1]):errors.append('framebuffer observations overlap')
    protected.extend(frames)
    lcd=[]
    for i,p in enumerate(profile.get('lcd_buffers',[])):
        f=region('supplied_lcd_buffer_'+str(i),p['base'],p['bytes']);lcd.append(f)
        if not contains(pool,f) or f['start']%16:errors.append(f['name']+': invalid pool extent/alignment')
        if overlap(f,allocation):errors.append(f['name']+': overlaps game allocation')
        if any(overlap(f,other) for other in frames+lcd[:-1]):errors.append('LCD/game buffer observations overlap')
    protected.extend(lcd)
    stack=profile.get('oem_stack')
    if stack:
        s=region('supplied_oem_thread_stack',stack['base'],stack['bytes']);protected.append(s)
        if overlap(s,allocation):errors.append('game allocation overlaps supplied OEM thread stack')
        owner=profile.get('oem_stack_allocation')
        if owner:
            o=region('supplied_oem_stack_allocation',owner['base'],owner['bytes']);protected.append(o)
            if not contains(o,s):errors.append('OEM thread stack exceeds its supplied allocation')
            if overlap(o,allocation):errors.append('game allocation overlaps supplied OEM stack allocation')
    sizes={name:integer(profile['sizes'].get(name,0),name) for name in
        ['payload_bytes','tail_bytes','bss_bytes','heap_bytes','private_stack_reserve_bytes']}
    payload=sizes['payload_bytes'];container_bytes=128+payload+sizes['tail_bytes']
    if payload<16:errors.append('payload smaller than footer span')
    if payload<1024 and payload%8:errors.append('partial OEM cipher block is not supported by host policy')
    if container_bytes>0xffffffff:raise ValueError('container size overflows uint32')
    loaded=region('temporary_container_read',BIN_ENTRY,container_bytes,'load')
    runtime=[]
    if payload:runtime.append(region('decoded_payload_span',BIN_ENTRY,payload))
    cursor=align_up(BIN_ENTRY+payload)
    for key,label in [('bss_bytes','caller_initialized_bss'),('heap_bytes','private_heap_reserve')]:
        if sizes[key]:runtime.append(region(label,cursor,sizes[key]))
        cursor=align_up(cursor+sizes[key])
    lower_end=min(allocation['end_exclusive'],AUDIO_BASE)
    stack_bytes=sizes['private_stack_reserve_bytes'];stack_base=(lower_end-stack_bytes)&~15
    if stack_bytes:
        if stack_base<0:errors.append('private stack reserve underflows lower arena')
        else:runtime.append(region('private_stack_reserve',stack_base,stack_bytes))
    if cursor>(stack_base if stack_bytes else lower_end):errors.append('payload/BSS/heap exceed lower arena or collide with private stack reserve')
    for r in [loaded]+runtime:
        if not contains(allocation,r):errors.append(r['name']+': outside supplied allocation')
        for p in protected:
            if overlap(r,p):errors.append(r['name']+': overlaps '+p['name'])
    for i,r in enumerate(runtime):
        for other in runtime[:i]:
            if overlap(r,other):errors.append(r['name']+': overlaps '+other['name'])
    capacity=allocation['end_exclusive']-BIN_ENTRY
    if container_bytes>=capacity:errors.append('container violates OEM strict file_size < capacity predicate')
    return {'status':'rejected' if errors else 'model_consistent','source_sha256':H['8000_MMI.RAP'],
        'allocation':allocation,'container_bytes':container_bytes,'derived_loader_capacity':capacity,
        'load_phase':[loaded],'run_phase':runtime,'protected':protected,'errors':list(dict.fromkeys(errors)),
        'limits':['Supplied allocation/framebuffer ranges are assumptions until captured on the actual device.',
                  'Physical RAM, package acceptance, IRQ/cache ABI and worker termination are not certified.',
                  'Audio reservation is conservative host policy, not an extra OEM loader check.',
                  'BSS must be initialized by future application startup; original BIN-loader does not provide an ELF BSS loader.',
                  'Private stack reserve does not change SP; the actual application inherits the OEM Game thread stack.',
                  'Temporary load and run regions can overlap across phases; simultaneous run regions may not overlap.'],
        'unresolved':['runtime allocation ownership','board RAM','RTOS/cache context']+([] if stack else ['OEM thread stack extent'])
                     +([] if lcd else ['persistent LCD buffer extents'])}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('profile',type=Path);p.add_argument('--output',type=Path);a=p.parse_args()
    try:result=plan(json.loads(a.profile.read_text()))
    except (KeyError,TypeError,ValueError) as error:p.error(str(error))
    output=json.dumps(result,indent=2)+'\n'
    if a.output:a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(output)
    else:print(output,end='')
    raise SystemExit(0 if result['status']=='model_consistent' else 1)
