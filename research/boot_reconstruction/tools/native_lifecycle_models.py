"""Host reconstructions of selected OEM pool, resource and return-path rules.
No live bindings, hardware operations or general-purpose allocator implementation.
"""
from native_abi_models import U32,signed

POOL_START=0x9fff00
POOL_END=0x2000000
BIN_ENTRY=0xa00000
AUDIO_BASE=0xf00000
AUDIO_PCM_BYTES=0x100000
TONELIB_CAPACITY=0x32000

def fresh_pool_layout(prefix_requests=(),framebuffers=True):
    """Fresh, single-free-block pool only; no fragmentation or concurrency."""
    alignment=16  # OEM promotes requested alignment 8 above its 12-byte header.
    bottom=((POOL_START+alignment+11)&-alignment)-12
    top=((POOL_END+12)&-alignment)-12
    free=top-bottom;allocations=[]
    initial={'managed':free,'used':0,'free':free,'max_request':free-12}
    def allocate(request):
        nonlocal free
        charged=(request+alignment+11)&-alignment
        if request<0x8000 or charged>free:raise ValueError('Outside fresh-pool model scope')
        free-=charged
        result={'requested':request,'charged':charged,'address':bottom+free+12}
        allocations.append(result)
        return result
    for n in prefix_requests:allocate(n)
    frames=[allocate(0x25800)['address'] for _ in range(2)] if framebuffers else []
    maximum=free-12;main=allocate(maximum)['address']
    return {'initial':initial,'allocations':allocations,'frames':frames,
            'main':main,'requested':maximum,'capacity':(main+maximum-BIN_ENTRY)&U32}

def reservation(base,maximum,previous_capacity=0):
    """OEM BIN branch: NULL is not rejected here; high base leaves stale capacity."""
    written=(base&U32)<=BIN_ENTRY
    return {'capacity_written':written,
            'capacity':((base+maximum-BIN_ENTRY)&U32) if written else previous_capacity&U32,
            'warning':not written,'loader_reached':True}

def loader_messages(result):
    result=signed(result)
    if result==0:return []
    events=[[0x560,0,0]]
    if result in (-5,-3,-2):events.append([0x555,0x70000002,0])
    return events

def resource_control(operation,mask,features,state):
    result=state.copy()
    if operation=='stop':
        mask &= 255
        result['requests'] &= (~mask)&255
        if mask&features&2:
            if result['midi_state']==3:return result
            result.update(midi_state=3,midi_remaining=0,midi_repeats=0)
        if mask&features&1:
            if result['wave_state']==13:return result
            result.update(wave_state=13,wave_base=0,wave_repeats=0)
    elif operation in ('pause','resume'):
        if mask&features&2:result['midi_state']=2 if operation=='pause' else 1
        if mask&features&1:result['wave_state']=12 if operation=='pause' else 11
    else:raise ValueError(operation)
    return result

def resource_kind(name):
    if name=='vibration':return 4
    if name=='backlight':return 8
    if name.endswith('.mid'):return 2
    if name.endswith('.wav'):return 1
    return 0

def repeat_count(value):
    value &= 255
    return value if value else 0xfffffff

def wave_play_result(rate,bits,repeats,features,descriptor_valid=True):
    scheduled=bool(features&1) and descriptor_valid and rate==8000 and bits==8
    status=0 if not descriptor_valid else U32 if features&1 and not scheduled else 1
    return {'status':status,'scheduled':scheduled,'repeat_count':repeat_count(repeats) if scheduled else None}

def audio_workspace(kind,cached,backend_result):
    allocate=kind==5 and cached==0
    return {'result':backend_result if allocate else AUDIO_BASE,
            'cached':backend_result if allocate else cached,'allocated':allocate}

def mixed_samples(wave=b'',midi=()):
    """One synthetic block; unsigned PCM8 wave + signed PCM16 MIDI, saturating."""
    samples=[]
    for i in range(max(len(wave),len(midi))):
        value=((wave[i]-128)<<8) if i<len(wave) else 0
        if i<len(midi):value+=midi[i]
        samples.append(max(-32768,min(32767,value)))
    return samples

def observed_partial_flush(buffer,valid_bytes):
    """OEM clears the FRONT of the 1024-byte buffer; deliberately not a fix."""
    if not 0<valid_bytes<1024 or len(buffer)!=1024:raise ValueError('Partial block required')
    return bytes(1024-valid_bytes)+buffer[1024-valid_bytes:]

def exit_calls(mode,game_handle,seq_handle,requests):
    """Call order assuming suspend/kill/delete all return, not scheduler behavior."""
    if not game_handle:return []
    calls=[['pause',[requests]],['suspend',[seq_handle]]]
    if signed(mode)>=0:calls.append(['suspend',[game_handle]])
    calls.extend([['sound_stop',[]],['command',[0xd07,0,0]]])
    if signed(mode)>=0:calls.append(['kill',[game_handle]])
    return calls
