#!/usr/bin/env python3
"""Strict ELF reader for the source-built startup experiment, not an OEM loader.
No image generation, dynamic linking or target-memory writes.
"""
import argparse,hashlib,json,struct
from pathlib import Path
ENTRY=0xa00000
BUDGET=128*1024 # Explicit analysis budget, not measured RAM availability.

def bounded(data,offset,size):
    if offset<0 or size<0 or offset+size>len(data):raise ValueError('Truncated ELF range')
    return data[offset:offset+size]

def string(data,offset):
    if not 0<=offset<len(data):raise ValueError('Invalid ELF string offset')
    end=data.find(b'\0',offset)
    if end<0:raise ValueError('Unterminated ELF string')
    return data[offset:end].decode('ascii')

def uleb(data,offset):
    value=0
    for shift in range(0,35,7):
        b=bounded(data,offset,1)[0];offset+=1;value|=(b&127)<<shift
        if b<128:return value,offset
    raise ValueError('Oversized ARM attribute integer')

def arm_attributes(data):
    if not data or data[0]!=ord('A'):raise ValueError('Missing ARM attributes format A')
    result={};offset=1
    while offset<len(data):
        length=struct.unpack('<I',bounded(data,offset,4))[0]
        if length<5:raise ValueError('Short ARM vendor subsection')
        vendor=bounded(data,offset,length);name=string(vendor,4);pos=5+len(name)
        if name!='aeabi':raise ValueError('Unexpected ARM attribute vendor')
        while pos<len(vendor):
            begin=pos;tag,pos=uleb(vendor,pos)
            size=struct.unpack('<I',bounded(vendor,pos,4))[0];pos+=4
            if size<pos-begin:raise ValueError('Invalid ARM attribute subsection size')
            attributes=bounded(vendor,pos,begin+size-pos)
            if tag!=1:raise ValueError('Only file attributes supported')
            at=0
            while at<len(attributes):
                key,at=uleb(attributes,at)
                if key in (4,5) or key>=32 and key%2:
                    value=string(attributes,at);at+=len(value)+1
                elif key==32:raise ValueError('Compatibility attribute not used by this experiment')
                else:value,at=uleb(attributes,at)
                if key in result and result[key]!=value:raise ValueError('Conflicting ARM attributes')
                result[key]=value
            pos=begin+size
        offset+=length
    return result

class StartupELF:
    def __init__(self,data):
        self.data=data
        h=struct.unpack('<16sHHIIIIIHHHHHH',bounded(data,0,52))
        ident,kind,machine,version,self.entry,phoff,shoff,flags,ehsize,phsize,phnum,shsize,shnum,shstr=h
        if ident[:9]!=b'\x7fELF\x01\x01\x01\x00\x00' or (kind,machine,version)!=(2,40,1):
            raise ValueError('Expected ELF32 little-endian ARM ET_EXEC System V')
        if flags>>24!=5 or flags&0x400:raise ValueError('Expected EABI5 without hard-float flag')
        if (ehsize,phsize,shsize)!=(52,32,40) or not 0<phnum<=8 or not 0<shnum<=256 or shstr>=shnum:
            raise ValueError('Unsupported ELF table sizes/counts')
        if self.entry!=ENTRY:raise ValueError('Unexpected entry point')
        self.flags=flags;self.segments=[]
        for i in range(phnum):
            typ,off,address,physical,file_size,size,permissions,alignment=struct.unpack('<8I',bounded(data,phoff+i*32,32))
            if typ!=1 or address!=physical or permissions not in (5,6):raise ValueError('Expected separate RX/RW PT_LOAD only')
            if not ENTRY<=address or size==0 or address+size>ENTRY+BUDGET or file_size>size:
                raise ValueError('PT_LOAD outside analysis budget')
            if alignment==0 or alignment&(alignment-1) or (off-address)%alignment:
                raise ValueError('Invalid PT_LOAD alignment')
            bounded(data,off,file_size)
            segment={'address':address,'offset':off,'file_bytes':file_size,'memory_bytes':size,'permissions':permissions}
            if any(address<s['address']+s['memory_bytes'] and s['address']<address+size for s in self.segments):
                raise ValueError('Overlapping PT_LOAD segments')
            self.segments.append(segment)
        if len(self.segments)!=2 or sorted(s['permissions'] for s in self.segments)!=[5,6]:
            raise ValueError('Expected exactly one code and one data segment')
        records=[struct.unpack('<10I',bounded(data,shoff+i*40,40)) for i in range(shnum)]
        names=bounded(data,records[shstr][4],records[shstr][5]);self.sections={};self.symbols={}
        for record in records[1:]:
            name,typ,section_flags,address,off,size,link,info,alignment,entsize=record
            name=string(names,name)
            if name in self.sections:raise ValueError('Duplicate section name')
            if typ!=8:bounded(data,off,size)
            if typ in (4,9) and size:raise ValueError('Unresolved relocation section')
            if section_flags&2:
                expected_flags={'.text':6,'.rodata':2,'.data':3,'.bss':3}
                if name not in expected_flags or section_flags!=expected_flags[name]:
                    raise ValueError('Unsupported allocated section/flags: '+name)
                if not any(s['address']<=address and address+size<=s['address']+s['memory_bytes'] for s in self.segments):
                    raise ValueError('Allocated section not in a segment')
            self.sections[name]={'type':typ,'flags':section_flags,'address':address,'offset':off,'bytes':size}
            if typ==2:
                if entsize!=16 or size%16 or link>=shnum:raise ValueError('Invalid symbol table')
                strings=bounded(data,records[link][4],records[link][5])
                for at in range(off,off+size,16):
                    n,value,symbol_size,symbol_info,other,index=struct.unpack('<IIIBBH',bounded(data,at,16))
                    symbol=string(strings,n)
                    if symbol and index==0:raise ValueError('Undefined symbol: '+symbol)
                    if symbol:self.symbols[symbol]={'address':value,'bytes':symbol_size}
        for name in ('_spmp_start','spmp_app_main','__bss_start__','__bss_end__','__file_image_end__','__runtime_end__','__reserved_footer_start__','__reserved_footer_end__'):
            if name not in self.symbols:raise ValueError('Missing startup symbol '+name)
        if self.address('_spmp_start')!=ENTRY:raise ValueError('Entry symbol mismatch')
        self.bss_start=self.address('__bss_start__');self.bss_end=self.address('__bss_end__')
        bss=self.sections.get('.bss');text=self.sections.get('.text')
        if not bss or bss['type']!=8 or bss['address']!=self.bss_start or bss['bytes']!=self.bss_end-self.bss_start:
            raise ValueError('BSS section/symbol mismatch')
        if self.bss_start%4 or self.bss_end%4 or self.bss_start>self.bss_end:
            raise ValueError('BSS word alignment/order mismatch')
        if not ENTRY<=self.address('__file_image_end__')<=self.bss_start<=self.bss_end==self.address('__runtime_end__')<=ENTRY+BUDGET:
            raise ValueError('Startup layout outside analysis budget')
        footer_start=self.address('__reserved_footer_start__');footer_end=self.address('__reserved_footer_end__')
        if footer_start%8 or footer_end-footer_start!=16 or not self.address('__file_image_end__')<=footer_start<footer_end<=self.bss_start:
            raise ValueError('Future footer reserve overlaps content/BSS')
        if not text or text['address']!=ENTRY or text['flags']&7!=6:raise ValueError('Expected AX startup text')
        attr=self.sections.get('.ARM.attributes')
        if not attr:raise ValueError('ARM attributes missing')
        self.attributes=arm_attributes(bounded(data,attr['offset'],attr['bytes']))
        if self.attributes.get(6)!=4 or self.attributes.get(8)!=1 or self.attributes.get(10,0)!=0 or self.attributes.get(28,0)!=0:
            raise ValueError('Expected ARMv5TE, ARM instructions, software float calls')
    def address(self,name):return self.symbols[name]['address']
    def report(self):
        return {'status':'ANALYSIS_ELF_ONLY','sha256':hashlib.sha256(self.data).hexdigest(),
            'entry':hex(self.entry),'eabi_flags':hex(self.flags),'cpu_arch':'ARMv5TE',
            'research_budget_bytes':BUDGET,'file_image_span_bytes':self.address('__file_image_end__')-ENTRY,
            'runtime_span_bytes':self.address('__runtime_end__')-ENTRY,
            'future_footer_reserve':{'start':hex(self.address('__reserved_footer_start__')),'end_exclusive':hex(self.address('__reserved_footer_end__')),'bytes':16,'marker_emitted':False},
            'bss':{'start':hex(self.bss_start),'end_exclusive':hex(self.bss_end),'bytes':self.bss_end-self.bss_start},
            'segments':self.segments,'limits':'128 KiB is an analysis budget; allocation/stack availability on console is not established.'}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('elf',type=Path);a=p.parse_args()
    print(json.dumps(StartupELF(a.elf.read_bytes()).report(),indent=2))
