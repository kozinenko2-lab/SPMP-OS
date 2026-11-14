#!/usr/bin/env python3
"""Run the same compiled copy engine against real temporary files on host/ARM926.

Only this Linux test backend supplies exclusive creation + renameat2 NOREPLACE.
No OEM image, physical SD, MMIO or device binding is used.
"""
import argparse,ctypes,errno,hashlib,json,os,stat,struct,tempfile
from pathlib import Path
ROOT='SPMP-OS/User';SRC=ROOT+'/ROMs/Игра renamed.a26';DST=ROOT+'/Copies/Игра renamed.a26'
NAMES=['stat','fstat','open_read','create_exclusive','read','write','seek_zero','close','publish_noreplace','unlink_stage']
# Corresponds to the public C enum; terminal/result assertions also test its ABI.
CHECK,WRITE,VERIFY,PUBLISH,DONE,FAILED,CANCELLED=1,5,11,16,17,18,19

class Files:
 def __init__(self,root,fault=None,short=False):
  self.root=Path(root);self.rootfd=os.open(root,os.O_DIRECTORY|os.O_RDONLY);self.handles={};self.events=[];self.fault=fault;self.short=short;self.max_io=0;self.writes=0;self.reads=0;self.created=0;self.injected=False
 def parent(self,path):
  assert path.startswith(ROOT+'/') and '..' not in path.split('/')
  parts=path.split('/');fd=os.dup(self.rootfd)
  try:
   for part in parts[:-1]:
    nextfd=os.open(part,os.O_DIRECTORY|os.O_RDONLY|os.O_NOFOLLOW,dir_fd=fd);os.close(fd);fd=nextfd
   return fd,parts[-1]
  except BaseException:os.close(fd);raise
 def meta(self,s):return struct.pack('<III',8 if stat.S_ISREG(s.st_mode) else 1 if stat.S_ISDIR(s.st_mode) else 0,s.st_size,int(s.st_mtime)&0xffffffff)
 def handle(self,fd,path,mode):
  i=0
  while i in self.handles:i+=1
  self.handles[i]=(fd,path,mode);return i
 def close(self):
  leaked=list(self.handles)
  for fd,_,_ in self.handles.values():os.close(fd)
  self.handles.clear();os.close(self.rootfd)
  assert not leaked,('leaked handles',leaked)
 def invoke(self,name,args,runner):
  counts=dict(zip(NAMES,[3,3,2,2,4,4,2,2,3,2]));a=list(args)[1:counts[name]];self.events.append(name)
  try:
   if name=='stat':
    path=runner.string(a[0]);p,base=self.parent(path)
    try:
     if self.fault=='stat_denied' and path==DST:return -errno.EACCES
     data=self.meta(os.stat(base,dir_fd=p,follow_symlinks=False))
    finally:os.close(p)
    runner.output(a[1],data);return 0
   if name=='fstat':
    fd,path,mode=self.handles[a[0]];data=self.meta(os.fstat(fd))
    if self.fault=='changed_open' and path==SRC:data=struct.pack('<III',8,1,0)
    if self.fault=='fstat_error':return -errno.EIO
    runner.output(a[1],data);return 0
   if name in ('open_read','create_exclusive'):
    path=runner.string(a[0]);p,base=self.parent(path)
    try:
     if self.fault=='read_open' and name=='open_read':return -errno.EACCES
     if self.fault=='create_error' and name=='create_exclusive':return -errno.EROFS
     if name=='create_exclusive':
      fd=os.open(base,os.O_RDWR|os.O_CREAT|os.O_EXCL|os.O_NOFOLLOW,0o600,dir_fd=p);self.created+=1;mode='w'
     else:fd=os.open(base,os.O_RDONLY|os.O_NONBLOCK|os.O_NOFOLLOW,dir_fd=p);mode='r'
    finally:os.close(p)
    return self.handle(fd,path,mode)
   if name in ('read','write'):
    fd,path,mode=self.handles[a[0]];buf,n=a[1:];assert 0<n<=2048;self.max_io=max(self.max_io,n)
    if name=='read':
     self.reads+=1
     if self.fault=='read_error' and self.reads==2:return -errno.EIO
     if self.fault=='verify_read' and path!=SRC:return -errno.EIO
     if self.fault=='read_overcount':return n+1
     if self.fault=='premature_eof' and self.reads==2:return 0
     data=os.read(fd,min(n,97) if self.short else n);runner.output(buf,data);return len(data)
    self.writes+=1
    if self.fault=='zero_write':return 0
    if self.fault=='write_overcount':return n+1
    if self.fault=='full' and self.writes==2:return -errno.ENOSPC
    if self.fault=='write_error':return -errno.EIO
    if self.fault=='source_growth' and not self.injected:
     self.injected=True
     with (self.root/SRC).open('ab') as f:f.write(b'new bytes')
    return os.write(fd,runner.read(buf,min(n,31) if self.short else n))
   if name=='seek_zero':
    if self.fault=='seek_error':return -errno.EIO
    os.lseek(self.handles[a[0]][0],0,0);return 0
   if name=='close':
    fd,path,mode=self.handles.pop(a[0]);os.close(fd)
    if mode=='w' and self.fault=='close_error':return -errno.EIO
    if mode=='w' and self.fault=='corrupt':
     with (self.root/path).open('r+b') as f:f.write(b'corrupt')
    return 0
   if name=='unlink_stage':
    assert '.spmp-copy-' in runner.string(a[0]);p,base=self.parent(runner.string(a[0]))
    try:
     if self.fault=='cleanup_error':return -errno.EACCES
     os.unlink(base,dir_fd=p);return 0
    finally:os.close(p)
   if name=='publish_noreplace':
    src,dst=map(runner.string,a);p,base=self.parent(src);q=-1
    try:
     q,target=self.parent(dst)
     if self.fault=='publish_error':return -errno.EIO
     if self.fault=='collision':
      fd=os.open(target,os.O_CREAT|os.O_EXCL|os.O_WRONLY,0o600,dir_fd=q);os.write(fd,b'EXIST');os.close(fd)
     libc=ctypes.CDLL(None,use_errno=True)
     # Explicitly require this operation; no check-then-ordinary-rename fallback.
     fn=libc.renameat2;fn.argtypes=[ctypes.c_int,ctypes.c_char_p,ctypes.c_int,ctypes.c_char_p,ctypes.c_uint]
     if fn(p,os.fsencode(base),q,os.fsencode(target),1):return -ctypes.get_errno()
     return 0
    finally:
     os.close(p)
     if q>=0:os.close(q)
   raise AssertionError(name)
  except OSError as e:return -e.errno

class Host:
 def __init__(self,path,files):
  self.lib=ctypes.CDLL(str(path.resolve()));self.files=files;self.errors=[]
  I,P,U=ctypes.c_int,ctypes.c_void_p,ctypes.c_uint32
  signatures=[[P,P,P],[P,I,P],[P,P],[P,P],[P,I,P,U],[P,I,P,U],[P,I],[P,I],[P,P,P],[P,P]]
  types=[ctypes.CFUNCTYPE(I,*s) for s in signatures]
  class IO(ctypes.Structure):_fields_=[('user',P)]+list(zip(NAMES,types))
  def callback(name):
   def run(*args):
    try:return files.invoke(name,args,self)
    except BaseException as e:self.errors.append(e);return -5
   return run
  self.callbacks=[t(callback(n)) for n,t in zip(NAMES,types)];self.io=IO(None,*self.callbacks)
  for n,types in [('init',[P]),('start',[P,P,U]),('step',[]),('cancel',[]),('cleanup',[]),('value',[U]),('fm02',[P,P,P,U]),('unsupported',[])]:
   f=getattr(self.lib,'copy_test_'+n);f.argtypes=types;f.restype=I
  assert self.lib.copy_test_init(ctypes.byref(self.io))==0
 def read(self,p,n):return ctypes.string_at(p,n)
 def write(self,p,data):ctypes.memmove(p,data,len(data))
 def output(self,p,data):self.write(p,data)
 def string(self,p):return ctypes.string_at(p).decode('utf8')
 def call(self,name,*args):
  args=[s.encode() if isinstance(s,str) else s for s in args];r=getattr(self.lib,'copy_test_'+name)(*args)
  if self.errors:raise self.errors[0]
  return r

class ARM:
 def __init__(self,path,files):
  from startup_elf import StartupELF,ENTRY,BUDGET
  from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE,UC_HOOK_MEM_WRITE
  from unicorn import arm_const as a
  self.a=a;self.image=StartupELF(path.read_bytes());self.files=files;self.min_sp=0x1108000
  self.u=Uc(UC_ARCH_ARM,UC_MODE_ARM);self.u.ctl_set_cpu_model(a.UC_CPU_ARM_926)
  self.u.mem_map(ENTRY,BUDGET);self.u.mem_map(0x1000000,0x10000);self.u.mem_map(0x1100000,0x10000);self.u.mem_map(0x1200000,0x2000)
  for s in self.image.segments:self.u.mem_write(s['address'],self.image.data[s['offset']:s['offset']+s['file_bytes']])
  self.regs=[getattr(a,'UC_ARM_REG_R'+str(i)) for i in range(4)];self.targets={0x1201000+4*i:n for i,n in enumerate(NAMES)}
  self.write(0x1000000,struct.pack('<11I',0,*self.targets))
  def hook(u,pc,size,user):
   sp=u.reg_read(a.UC_ARM_REG_SP);self.min_sp=min(self.min_sp,sp)
   if pc not in self.targets:return
   assert sp%8==0
   r=files.invoke(self.targets[pc],[u.reg_read(reg) for reg in self.regs],self)
   u.reg_write(a.UC_ARM_REG_R0,r&0xffffffff);u.reg_write(a.UC_ARM_REG_PC,u.reg_read(a.UC_ARM_REG_LR))
  def writes(u,access,address,size,value,user):
   assert self.image.bss_start<=address and address+size<=self.image.bss_end or 0x1106000<=address and address+size<=0x1108000
  self.u.hook_add(UC_HOOK_CODE,hook);self.u.hook_add(UC_HOOK_MEM_WRITE,writes)
  assert self.call('init',0x1000000)==0
 def read(self,p,n):return bytes(self.u.mem_read(p,n))
 def write(self,p,data):self.u.mem_write(p,data)
 def output(self,p,data):
  assert self.image.bss_start<=p and p+len(data)<=self.image.bss_end or 0x1106000<=p and p+len(data)<=0x1108000
  self.write(p,data)
 def string(self,p):return self.read(p,512).split(b'\0')[0].decode('utf8')
 def call(self,name,*args):
  a=self.a;values=[]
  for i,v in enumerate(args):
   if isinstance(v,str):p=0x1001000+1024*i;self.write(p,v.encode()+b'\0');v=p
   values.append(v)
  for reg,v in zip(self.regs,values):self.u.reg_write(reg,v&0xffffffff)
  saved=[getattr(a,'UC_ARM_REG_R'+str(i)) for i in range(4,12)]
  for i,reg in enumerate(saved):self.u.reg_write(reg,0x12340000+i)
  self.u.reg_write(a.UC_ARM_REG_SP,0x1108000);self.u.reg_write(a.UC_ARM_REG_LR,0x1200000)
  self.u.emu_start(self.image.address('copy_test_'+name),0x1200000,count=10000000,timeout=10000000)
  assert self.u.reg_read(a.UC_ARM_REG_PC)==0x1200000,'finite ARM limit'
  assert self.u.reg_read(a.UC_ARM_REG_SP)==0x1108000
  assert [self.u.reg_read(reg) for reg in saved]==[0x12340000+i for i in range(8)]
  assert self.min_sp>=0x1106000,'8-KiB inherited stack budget'
  r=self.u.reg_read(a.UC_ARM_REG_R0);return r if r<0x80000000 else r-0x100000000

def run(path,arm=False):
 rows=[]
 cases=[('normal',None,0),('relaunch',None,0),('restart_after_cancel',None,0),('empty',None,0),('boundary',None,0),('short',None,0),('existing',None,-17),('stage_collision',None,0),('stages_full',None,-17),('missing',None,-2),('directory',None,-21),('system',None,-22),('traversal',None,-22),('stage_limit_ok',None,0),('stage_limit_bad',None,-36),('oversize_name',None,-22),('long_path',None,-22),('trailing_space',None,-22),('colon',None,-22),('same',None,-22),('invalid_utf8',None,-22),('symlink',None,-95),('parent_symlink',None,-20),('stat_denied','stat_denied',-13),('read_open','read_open',-13),('changed_open','changed_open',-116),('fstat_error','fstat_error',-5),('create_error','create_error',-30),('read_error','read_error',-5),('premature_eof','premature_eof',-116),('read_overcount','read_overcount',-5),('write_overcount','write_overcount',-5),('zero_write','zero_write',-5),('full','full',-28),('write_error','write_error',-5),('source_growth','source_growth',-116),('close_error','close_error',-5),('seek_error','seek_error',-5),('corrupt','corrupt',-5),('verify_read','verify_read',-5),('collision','collision',-17),('publish_error','publish_error',-5),('cleanup_error','cleanup_error',-125),('cancel_start',None,-125),('cancel_copy',None,-125),('cancel_verify',None,-125),('cancel_publish',None,-125),('fm02',None,0),('fm02_cancel',None,-125),('unsupported',None,-38)]
 for name,fault,expected in cases:
  with tempfile.TemporaryDirectory(prefix='spmp-copy-test-') as tmp:
   root=Path(tmp);(root/ROOT/'ROMs').mkdir(parents=True);(root/ROOT/'Copies').mkdir();payload=bytes((i*17+11)&255 for i in range(6501))
   if name=='empty':payload=b''
   if name=='boundary':payload=payload[:2048]
   source,destination=SRC,DST;(root/source).write_bytes(payload)
   collision_names=[]
   if name=='existing':(root/destination).write_bytes(b'EXIST')
   if name in ('stage_collision','stages_full'):
    for i in range(1 if name=='stage_collision' else 16):
     p=root/ROOT/'Copies'/('.spmp-copy-%08x.tmp'%i);p.write_bytes(b'OLD STAGE');collision_names.append(p)
   if name=='missing':(root/source).unlink()
   if name=='directory':(root/source).unlink();(root/source).mkdir()
   if name=='system':destination='SPMP-OS/System/x.bin'
   if name=='traversal':destination=ROOT+'/Copies/../escape'
   if name=='same':destination=source
   if name in ('stage_limit_ok','stage_limit_bad'):
    parent=ROOT+'/Copies/'+230*'a'+'/'+(236 if name=='stage_limit_ok' else 237)*'b'+'/'
    (root/parent).mkdir(parents=True);destination=parent+'z'
   if name=='oversize_name':destination=ROOT+'/Copies/'+256*'x'
   if name=='long_path':destination=ROOT+'/Copies/'+600*'x'
   if name=='trailing_space':destination=ROOT+'/Copies/x '
   if name=='colon':destination=ROOT+'/Copies/x:y'
   if name=='invalid_utf8':destination=ROOT+'/Copies/\ud800'
   if name=='symlink':(root/source).unlink();(root/source).symlink_to('/etc/passwd')
   if name=='parent_symlink':(root/ROOT/'Copies').rmdir();(root/ROOT/'Copies').symlink_to(root/ROOT/'ROMs',target_is_directory=True)
   files=Files(root,fault,name=='short');r=(ARM if arm else Host)(path,files)
   # Surrogateescape deliberately supplies an invalid byte sequence to C.
   if name=='invalid_utf8':
    destination=ROOT+'/Copies/x'
    if arm:
     r.write(0x1003000,(ROOT+'/Copies/').encode()+b'\xed\xa0\x80\0');result=r.call('start',source,0x1003000,0)
    else:result=r.call('start',source,(ROOT+'/Copies/').encode()+b'\xed\xa0\x80',0)
   elif name.startswith('fm02'):result=r.call('fm02',ROOT+'/ROMs',ROOT+'/Copies','Игра renamed.a26',20 if name=='fm02_cancel' else 0)
   elif name=='unsupported':result=r.call('unsupported')
   else:result=r.call('start',source,destination,0)
   if not result and name=='restart_after_cancel':
    r.call('step');r.call('cancel');assert r.call('step')==1 and r.call('value',1)==-125
    assert r.call('start',source,destination,1)==0
   if not result and not name.startswith('fm02'):
    assert r.call('start',source,destination,1)==-16,'active job replacement'
    assert r.call('cleanup')==-16,'active cleanup must not remove stage'
    stop={'cancel_start':CHECK,'cancel_copy':WRITE,'cancel_verify':VERIFY,'cancel_publish':PUBLISH,'cleanup_error':WRITE}.get(name)
    for steps in range(10000):
     if r.call('value',0)==stop:r.call('cancel')
     if r.call('step'):break
    else:raise AssertionError('bounded job did not terminate')
    result=r.call('value',1)
    before=len(files.events);assert r.call('step')==1 and len(files.events)==before
   assert result==expected,(name,result,expected)
   if name.startswith('fm02'):
    assert r.call('value',10)==(7 if expected else 5)
    assert r.call('value',11)==(2 if expected else 3)
   if expected==0:
    assert (root/destination).read_bytes()==payload and r.call('value',4)==len(payload) and r.call('value',6)==1
   elif name in ('existing','collision'):assert (root/destination).read_bytes()==b'EXIST'
   elif expected!=-22 and name not in ('same','parent_symlink','invalid_utf8'):assert not (root/destination).exists(),name
   if name not in ('directory','missing','symlink','source_growth'):assert (root/SRC).read_bytes()==payload
   stages=[p for p in (root/ROOT/'Copies').rglob('.spmp-copy-*') if p not in collision_names]
   if name=='cleanup_error':
    assert len(stages)==1 and r.call('value',2)==-13 and r.call('value',5)==1
    assert r.call('start',source,destination,2)==-16
   else:assert not stages,name
   assert all(p.read_bytes()==b'OLD STAGE' for p in collision_names)
   if name=='relaunch':
    other=ROOT+'/Copies/Another name.a26';assert r.call('start',SRC,other,1)==0
    for i in range(1000):
     if r.call('step'):break
    else:raise AssertionError('relaunch did not finish')
    assert r.call('value',1)==0 and (root/other).read_bytes()==payload
   if expected in (-22,-36) or name=='unsupported':assert not files.events
   assert files.max_io<=2048
   rows.append({'case':name,'result':result,'callbacks':len(files.events),'max_transfer':files.max_io,'task_bytes':r.call('value',9),'written':r.call('value',3),'verified':r.call('value',4),'published':bool(r.call('value',6)),'cleanup_error':r.call('value',2),'owned_stage':bool(r.call('value',5)),**({'max_stack_bytes':0x1108000-r.min_sp} if arm else {})})
   if name=='cleanup_error':
    assert r.call('cleanup')==-13
    files.fault=None;assert r.call('cleanup')==0 and not r.call('value',5) and not r.call('value',2)
    assert r.call('value',1)==-125 and not stages[0].exists()
    assert r.call('start',source,destination,2)==0;r.call('cancel');assert r.call('step')==1
    rows[-1]['cleanup_retry_pass']=True
   files.close()
 return {'execution':'Compiled ARM926 with real temporary Linux filesystem callbacks' if arm else 'Compiled host library with real temporary Linux files','source_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'case_count':len(rows),'cases':rows,'limits':['Linux exclusive-create and renameat2(RENAME_NOREPLACE) backend; no live FAT bindings.','Read-back comparison is not power-loss durability or a snapshot under concurrent source writers.','Regular file COPY only; recursive directories, MOVE, RENAME, DELETE and device UI are not enabled.']}

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--library',type=Path);p.add_argument('--elf',type=Path);p.add_argument('--output',type=Path);a=p.parse_args()
 if bool(a.library)==bool(a.elf):p.error('Select library or ELF')
 result=run(a.elf or a.library,bool(a.elf))
 if a.output:a.output.write_text(json.dumps(result,indent=2)+'\n')
 print('PASS:',result['case_count'],result['execution'])
