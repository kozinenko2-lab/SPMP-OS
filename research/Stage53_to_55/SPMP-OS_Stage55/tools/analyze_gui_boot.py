#!/usr/bin/env python3
"""SPMP-OS Stage 55: reproducible, read-only MMI GUI/RTOS boot-chain audit.

Requires the ORIGINAL 8000_MMI.RAP only. No hardware access, no patches.
End-of-function boundaries are heuristics (next characteristic ARM prologue);
call edges/branch literals reported only if matching exact original ARM words.
"""
import argparse
import csv
import hashlib
import json
import sys
from pathlib import Path

# Reuse checked original RAP mapping and ARM BL/PC literal decoders from Stage54.
_here=Path(__file__).resolve().parent
_stage54=_here.parent.parent / 'SPMP-OS_Stage54_MMI_Architecture' / 'tools'
# For copying as a portable patch, include a bundled copy of the Stage54 decoder.
try:
    from spmp_oem_arm import Firmware, SEGMENT_BASE, SEGMENT_LEN, KNOWN_SHA256
except ImportError:
    sys.path.insert(0,str(_stage54))
    from trace_mmi_threads import Firmware, SEGMENT_BASE, SEGMENT_LEN, KNOWN_SHA256

# Explicitly scoped functions: no speculative names for discovered helpers.
ROOTS = {
    0x00280040: 'MMI_reset',
    0x00297CE8: 'MMI_startup_dispatch',
    0x002807C8: 'create_Demo_GUI',
    0x003BE5B0: 'Demo_GUI_thread',
    0x00447640: 'GUI_system_coordinator',
    0x004475C0: 'create_system_initial_thread',
    0x00447540: 'system_initial_thread',
    0x004474D8: 'system_init_retry_helper',
    0x00447458: 'create_background_thread',
    0x004473BC: 'background_thread',
    0x00339AAC: 'thread_creation_wrapper',
    0x0033A2DC: 'thread_creation_internal',
}
NODES_WITH_EVIDENCE = {
    0x003BE5B0: ('Demo GUI', 0x00280810, 25, 0x4000),
    0x00447540: ('Systen initial Thread', 0x00447610, 12, 10624),
    0x004473BC: ('Systen BackGround Thread', 0x0044749C, 30, 10624),
}
HARD_CALLS = {
    0x00297CFC: 0x002807C8,
    0x00280810: 0x00339AAC,
    0x003BE5CC: 0x00447640,
    0x004476B0: 0x004475C0,
    0x00447610: 0x00339AAC,
    0x004475A4: 0x00447458,
    0x0044749C: 0x00339AAC,
    0x004473F0: 0x002CDACC,
    0x0044744C: 0x00339BE8,
    0x00339AF0: 0x0033A2DC,
}
GUI_MMIO_ADDR = 0x90005040
XREF_TARGETS = (0x003BE5B0,0x00447640,0x004475C0,0x00447540,0x00447458,0x004473BC,0x00339AAC,0x00339BE8,0x00339B04)


def hex8(n):return '0x%08X'%n

def sign24(word):
    v=word&0xffffff
    return v - (1<<24) if v & (1<<23) else v

def branch_target(addr, word):
    if ((word>>25)&7)!=5 or ((word>>28)&15)==15: return None
    return (addr+8+sign24(word)*4)&0xffffffff

def arm_imm(word):
    n=word&0xff
    rotate=((word>>8)&15)*2
    return (((n>>rotate)|(n<<(32-rotate)))&0xffffffff) if rotate else n


def function_boundary(fw, start):
    # Last word of the discovered function, exclusive. Data-literal pools may
    # appear before the next prologue; do not interpret them as executable facts.
    for pos in range(start+4, min(start+0x4000,SEGMENT_BASE+SEGMENT_LEN-8),4):
        nxt=fw.u32(pos+4)
        if fw.u32(pos)==0xe1a0c00d and nxt is not None and (nxt&0xffff0000)==0xe92d0000:
            return pos
    raise RuntimeError('next ARM prologue not found: '+hex8(start))


def scan_function(fw, entry, name):
    end=function_boundary(fw,entry)
    calls=[]; backward=[]; pc_literals=[]; indirect=[]
    for addr in range(entry,end,4):
        w=fw.u32(addr)
        if w is None: continue
        # BL/BLX direct conditional; addresses are validated against raw binary.
        if ((w>>24)&15)==0xb and ((w>>28)&15)!=15:
            dst=branch_target(addr,w)
            if dst is not None and SEGMENT_BASE<=dst<SEGMENT_BASE+SEGMENT_LEN:
                calls.append({'from':hex8(addr),'to':hex8(dst),'condition':(w>>28)&15,
                              'role':'direct_bl','observed_oem_word':hex8(w)})
        elif ((w>>25)&7)==5 and ((w>>24)&1)==0 and ((w>>28)&15)!=15:
            dst=branch_target(addr,w)
            if dst is not None and entry<=dst<addr:
                backward.append({'from':hex8(addr),'to':hex8(dst),'word':hex8(w)})
        # Read PC-relative literals with no unrelated variable dereferences.
        lit=fw.pc_literal(addr)
        if lit:
            try:
                value=int(lit['value'],16)
                parsed={'instruction':hex8(addr),'register':'r'+str(lit['register']),
                        'literal_at':hex8(int(lit['literal_address'],16)),
                        'value':hex8(value)}
                st=fw.cstring(value,200)
                if st: parsed['string']=st
                pc_literals.append(parsed)
            except (ValueError,TypeError): pass
        # MOV LR,PC + MOV PC,Rn is explicit indirect callsite; do not guess target.
        if w==0xe1a0e00f:
            after=fw.u32(addr+4)
            if after is not None and (after&0xfffffff0)==0xe1a0f000:
                indirect.append({'at':hex8(addr+4),'target_register':'r'+str(after&15)})
    return {'entry':hex8(entry),'label':name,'prologue':hex8(fw.u32(entry)),
            'next_prologue_heuristic':hex8(end),'call_sites':calls,
            'back_edges':backward,'pc_literals':pc_literals,'indirect_calls':indirect}


def validate_specifics(fw):
    results={}
    for a,target in HARD_CALLS.items():
        w=fw.u32(a)
        ok=fw.bl(a)==target
        results[f'bl_{a:08X}_{target:08X}']=ok
    # Strongly anchor exact six instructions; decode MOV/ADD immediate values.
    insns=[(0x00447668,0xe3a02209),(0x0044766c,0xe2822a05),
           (0x00447670,0xe2822040),(0x00447674,0xe5923000),
           (0x00447678,0xe3833007),(0x0044767c,0xe5823000)]
    results['MMIO_six_ARM_words_exact']=all(fw.u32(a)==w for a,w in insns)
    mov,add1,add2=[fw.u32(a) for a in (0x447668,0x44766c,0x447670)]
    target=(arm_imm(mov)+arm_imm(add1)+arm_imm(add2))&0xffffffff
    results['MMIO_address_0x90005040']=target==GUI_MMIO_ADDR
    results['MMIO_sets_low_three_bits']=arm_imm(fw.u32(0x447678))==7
    results['background_unconditional_loop']=fw.u32(0x447450)==0xeaffffe6 and branch_target(0x447450,fw.u32(0x447450))==0x4473f0
    results['background_delay_argument_100']=fw.u32(0x447448)==0xe3a00064
    results['background_delay_r1_zero']=fw.u32(0x447444)==0xe3a01000
    # Thread parameters: r0 = priority; r1 = entrypoint via verified literal.
    for entry,(name,site,priority,stack_size) in NODES_WITH_EVIDENCE.items():
        fcall=fw.bl(site)==0x339aac
        stack_o={0x3be5b0:(0x2807e8,16384),0x447540:(0x4475f0,10624),0x4473bc:(0x447480,10624)}[entry]
        # Stack instruction may be MOV, MOVW or load; for two SYS threads MOV #10624 checked below.
        results['spawn_'+name.replace(' ','_')]=fcall
    results['Demo_GUI_priority25']=fw.u32(0x280800)==0xe3a00019
    results['SysInit_priority12']=fw.u32(0x447600)==0xe3a0000c
    results['BG_priority30']=fw.u32(0x447490)==0xe3a0001e
    results['Demo_GUI_stack_16384']=fw.u32(0x2807e8)==0xe3a03901
    results['SysInit_stack_10624']=fw.u32(0x4475f0)==0xe3a03da6
    results['BG_stack_10624']=fw.u32(0x447480)==0xe3a03da6
    results['SysInit_to_background_builder']=fw.bl(0x4475a4)==0x447458
    results['Demo_GUI_entry_r1']=fw.u32(0x280834)==0x3be5b0
    results['SysInit_entry_r1']=fw.u32(0x447638)==0x447540
    results['BG_entry_r1']=fw.u32(0x4474cc)==0x4473bc
    results['BG_delay_value_is_NOT_milliseconds_without_tick_freq']=True
    if not all(results.values()):
        raise AssertionError('Exact OEM checks failed: '+str([x for x,y in results.items() if not y]))
    return results


def analyze(fw):
    checks=validate_specifics(fw)
    functions={hex8(a):scan_function(fw,a,name) for a,name in ROOTS.items()}
    spawn=[]
    for addr,(name,site,prio,stack) in NODES_WITH_EVIDENCE.items():
        spawn.append({'thread':name,'entry':hex8(addr),'create_callsite':hex8(site),
                      'priority_arg_r0':prio,'stack_bytes_argument':stack,
                      'edge_type':'function_pointer_argument_r1_NOT_direct_call'})
    call_xrefs={hex8(t):[] for t in XREF_TARGETS}
    for i in range(0,len(fw.segment)-3,4):
        src=SEGMENT_BASE+i
        target=fw.bl(src)
        if target in XREF_TARGETS: call_xrefs[hex8(target)].append(hex8(src))
    checks['GUI_coordinator_has_one_direct_BL_caller'] = call_xrefs[hex8(0x447640)]==[hex8(0x3be5cc)]
    checks['system_init_thread_has_no_direct_BL_caller'] = not call_xrefs[hex8(0x447540)]
    checks['background_thread_has_no_direct_BL_caller'] = not call_xrefs[hex8(0x4473bc)]
    if not all(checks.values()):raise AssertionError('Xref checks failed')
    output={'kind':'static_ARM32_original_MMI_analysis','sha256':fw.original_sha256,
            'no_binary_changes':True, 'source_map':{'rap_virtual_base':hex8(SEGMENT_BASE),'rap_file_offset':hex8(0x4c)},
            'checks':checks,'functions':functions,'thread_spawns':spawn,'direct_BL_xrefs':call_xrefs,
            'mmio':{'coordinator':hex8(0x447640),'instruction_range':'0x00447668..0x0044767C',
                    'address':hex8(GUI_MMIO_ADDR),'read_modify_or_0x7_write':True,
                    'register_semantics':'UNKNOWN; do not perform writes from SPMP-OS'},
            'limits':['A static machine-code audit, not a Ghidra decompilation or hardware trace.',
                'No guarantee that conditional code paths execute on every boot.',
                'The next-prologue bound is heuristic; trailing literal pools are not executable.',
                'Exact purpose of 0x90005040 and other helper routines requires further investigation.',
                'Function pointer threads are asynchronous; BL ordering does not establish scheduling order.',
                'DO NOT patch RedBoot, factory RAP or flash from this report.']}
    return output


def outfiles(data, directory):
    directory.mkdir(parents=True,exist_ok=True)
    (directory/'stage55_verified_architecture.json').write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    with (directory/'stage55_direct_calls.csv').open('w',newline='',encoding='utf-8') as fo:
        w=csv.writer(fo);w.writerow(['caller_entry','caller_label','callsite','target','arm_condition','machine_word'])
        for f in data['functions'].values():
            for c in f['call_sites']:
                w.writerow([f['entry'],f['label'],c['from'],c['to'],hex(c['condition']),c['observed_oem_word']])
    with (directory/'stage55_literals.csv').open('w',newline='',encoding='utf-8') as fo:
        w=csv.writer(fo);w.writerow(['function_entry','function_name','load_instruction','dest_reg','literal_address','value','string_if_printable'])
        for f in data['functions'].values():
            for p in f['pc_literals']:
                w.writerow([f['entry'],f['label'],p['instruction'],p['register'],p['literal_at'],p['value'],p.get('string','')])
    with (directory/'stage55_spawns.csv').open('w',newline='',encoding='utf-8') as fo:
        w=csv.DictWriter(fo,fieldnames=list(data['thread_spawns'][0]));w.writeheader();w.writerows(data['thread_spawns'])
    with (directory/'stage55_full_image_xrefs.csv').open('w',newline='',encoding='utf-8') as fo:
        w=csv.writer(fo);w.writerow(['callee','caller_instruction'])
        for target,sites in data['direct_BL_xrefs'].items():
            for site in sites: w.writerow([target,site])
    # DOT groups hardware calls and thread-start edges separately.
    graph=['digraph MMI_Stage55 {',' rankdir=TB;',' node [shape=box, style="rounded", fontname="Liberation Sans"];',
           ' "MMI_reset" -> "startup_dispatch" [label="BL→constructor"];',
           ' "startup_dispatch" -> "create_Demo_GUI" [label="BL"];',
           ' "create_Demo_GUI" -> "Demo GUI" [style=dashed,label="thread entry r1"];',
           ' "Demo GUI" -> "GUI coordinator" [label="BL 0x3BE5CC"];',
           ' "GUI coordinator" -> "0x90005040" [color=red,label="MMIO read|OR 7|write"];',
           ' "GUI coordinator" -> "create_system_initial" [label="BL 0x4476B0"];',
           ' "create_system_initial" -> "System initial" [style=dashed,label="thread entry r1"];',
           ' "System initial" -> "create_background" [label="BL 0x4475A4"];',
           ' "create_background" -> "Background" [style=dashed,label="thread entry r1"];',
           ' "Background" -> "eCos delay(100)" [label="BL 0x44744C"];',
           ' "Background" -> "Background" [color=blue,label="B 0x447450"];',
           ' "0x90005040" [shape=octagon,color=red];','}']
    (directory/'stage55_startup_to_gui.dot').write_text('\n'.join(graph)+'\n',encoding='utf-8')


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--rap',type=Path,required=True)
    ap.add_argument('--outdir',type=Path,required=True)
    args=ap.parse_args()
    fw=Firmware(args.rap.read_bytes())
    result=analyze(fw)
    outfiles(result,args.outdir)
    print('Stage55 OK: ',len(result['checks']),'hard checks,',len(result['functions']),'scoped functions,',
          sum(len(x['call_sites']) for x in result['functions'].values()),'direct BL calls;',
          'MMIO',result['mmio']['address'])
if __name__=='__main__': main()
