"""RAM-only original ARM wrapper harness. Optional dependency: Unicorn.
Every intercepted backend call is explicit in an individual test. No MMIO mapped.
"""
import hashlib,struct
from audit_boot import Image,H
class ArmVM:
    def __init__(self,blob):
        from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE
        from unicorn.arm_const import UC_ARM_REG_PC,UC_ARM_REG_LR,UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3
        if hashlib.sha256(blob).hexdigest()!=H['8000_MMI.RAP']:raise ValueError('Original RAP required')
        self.im=Image(blob);self.u=Uc(UC_ARCH_ARM,UC_MODE_ARM);self.calls=[];self.stubs={}
        self.regs=[UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3]
        self.pc=UC_ARM_REG_PC;self.lr=UC_ARM_REG_LR
        self.u.mem_map(0x280000,0x480000)
        for s in self.im.s:
            if s['size'] and 0x280000<=s['address']<0x700000:
                self.u.mem_write(s['address'],blob[s['offset']:s['offset']+s['size']])
        self.u.mem_map(0x1000000,0x100000)
        self.u.mem_map(0x1100000,0x20000)
        self.u.mem_map(0x1200000,0x10000)
        def hook(u,address,size,user):
            if address not in self.stubs:return
            name,result,nargs=self.stubs[address];args=[u.reg_read(r) for r in self.regs[:nargs]]
            self.calls.append([name,args])
            if callable(result):result=result(self,args)
            u.reg_write(UC_ARM_REG_R0,result&0xffffffff)
            u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
        self.u.hook_add(UC_HOOK_CODE,hook)
    def stub(self,address,name,result=0,nargs=0):self.stubs[address]=(name,result,nargs)
    def put32(self,address,value):self.u.mem_write(address,struct.pack('<I',value&0xffffffff))
    def get32(self,address):return struct.unpack('<I',self.u.mem_read(address,4))[0]
    def get16(self,address):return struct.unpack('<H',self.u.mem_read(address,2))[0]
    def string(self,address):return bytes(self.u.mem_read(address,512)).split(b'\0')[0].decode('ascii')
    def run(self,entry,args=(),stack=(),*,max_instructions=1000000,timeout_us=10000000):
        if max_instructions<=0 or timeout_us<=0:raise ValueError('Finite positive VM limits required')
        from unicorn.arm_const import (UC_ARM_REG_SP,UC_ARM_REG_R4,UC_ARM_REG_R5,
            UC_ARM_REG_R6,UC_ARM_REG_R7,UC_ARM_REG_R8,UC_ARM_REG_R9,
            UC_ARM_REG_R10,UC_ARM_REG_R11)
        saved=[UC_ARM_REG_R4,UC_ARM_REG_R5,UC_ARM_REG_R6,UC_ARM_REG_R7,
               UC_ARM_REG_R8,UC_ARM_REG_R9,UC_ARM_REG_R10,UC_ARM_REG_R11]
        sentinels=[0x45670000+4*i for i in range(len(saved))]
        for r,value in zip(saved,sentinels):self.u.reg_write(r,value)
        for i,r in enumerate(self.regs):self.u.reg_write(r,(args[i] if i<len(args) else 0x13572468)&0xffffffff)
        self.u.reg_write(UC_ARM_REG_SP,0x1110000);self.u.reg_write(self.lr,0x1200000)
        for i,value in enumerate(stack):self.put32(0x1110000+4*i,value)
        self.u.emu_start(entry,0x1200000,timeout=timeout_us,count=max_instructions)
        if self.u.reg_read(self.pc)!=0x1200000:raise RuntimeError('VM execution limit reached')
        if self.u.reg_read(UC_ARM_REG_SP)!=0x1110000:raise RuntimeError('SP not restored')
        if [self.u.reg_read(r) for r in saved]!=sentinels:
            raise RuntimeError('Callee-saved register mismatch')
        return self.u.reg_read(self.regs[0])
