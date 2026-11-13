//@category SPMP-OS
// Only operate on a scratch imported program. Validate raw source before any edits.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;
import java.io.*;
public class SPMP_VerifyAndPrepare extends GhidraScript {
 private long u32(byte[] b,int at) {return Integer.toUnsignedLong((b[at]&255)|((b[at+1]&255)<<8)|((b[at+2]&255)<<16)|((b[at+3]&255)<<24));}
 private String sha(byte[] b) throws Exception {return HexFormat.of().formatHex(MessageDigest.getInstance("SHA-256").digest(b));}
 public void run() throws Exception {
  String[] a=getScriptArgs();if(a.length<3)throw new IllegalArgumentException("raw-file expected-sha verification.tsv [addr:firstword:name ...]");
  byte[] raw=Files.readAllBytes(Paths.get(a[0]));String hash=sha(raw);
  if(!hash.equals(a[1]))throw new IllegalStateException("Wrong raw source SHA-256");
  StringBuilder report=new StringBuilder("source_sha256="+hash+"\nrecord_offset\tdestination\tbytes\tstatus\tpayload_sha256\n");
  int matched=0;
  for(int i=0;i+24<=raw.length;i+=4) {
   if(u32(raw,i)!=0x73504d4dL)continue;
   long len=u32(raw,i+4),v=u32(raw,i+8);
   if(len<24 || len>raw.length-i || v!=u32(raw,i+12) || u32(raw,i+16)!=0 || u32(raw,i+20)>1)continue;
   int n=(int)len-24;String state;
   byte[] expected=Arrays.copyOfRange(raw,i+24,i+(int)len);
   Address start=toAddr(v);
   if(n==0)state="EMPTY";
   else if(!currentProgram.getMemory().contains(start))state="NOT_MAPPED";
   else {
    byte[] actual=new byte[n];currentProgram.getMemory().getBytes(start,actual);
    if(!Arrays.equals(expected,actual))throw new IllegalStateException("GZF bytes differ from original at "+start);
    matched++;state="EXACT_MATCH";
   }
   report.append(String.format("0x%x\t0x%x\t%d\t%s\t%s\n",i,v,n,state,sha(expected)));
  }
  if(matched==0)throw new IllegalStateException("No matching raw segments");
  Path out=Paths.get(a[2]);Files.createDirectories(out.getParent());Files.writeString(out,report.toString());
  for(int i=3;i<a.length;i++) {
   if(a[i].startsWith("noreturn:")) {
    String[] n=a[i].split(":");Address addr=toAddr(n[1]), last=toAddr(n[3]);
    if(Integer.toUnsignedLong(currentProgram.getMemory().getInt(addr))!=Long.parseUnsignedLong(n[2],16) ||
       Integer.toUnsignedLong(currentProgram.getMemory().getInt(last))!=Long.parseUnsignedLong(n[4],16))
       throw new IllegalStateException("No-return function word mismatch");
    Function f=getFunctionAt(addr);
    if(f==null)throw new IllegalStateException("No-return function missing");
    f.setNoReturn(true);println("NORETURN "+addr);continue;
   }
   if(a[i].startsWith("merge:")) {
    String[] m=a[i].split(":");
    Address real=toAddr(m[1]), bad=toAddr(m[2]), end=toAddr(m[3]);
    if(Integer.toUnsignedLong(currentProgram.getMemory().getInt(real))!=Long.parseUnsignedLong(m[4],16) ||
       Integer.toUnsignedLong(currentProgram.getMemory().getInt(bad))!=Long.parseUnsignedLong(m[5],16) ||
       Integer.toUnsignedLong(currentProgram.getMemory().getInt(end.subtract(4)))!=Long.parseUnsignedLong(m[6],16))
       throw new IllegalStateException("Merge boundary word mismatch");
    Function parent=getFunctionAt(real), fragment=getFunctionAt(bad);
    if(parent==null || fragment==null || end.compareTo(bad)<=0 || bad.compareTo(real)<=0)
       throw new IllegalStateException("Unexpected merge functions");
    FunctionIterator all=currentProgram.getFunctionManager().getFunctions(new AddressSet(real,end.subtract(1)),true);
    while(all.hasNext()) {
     Address entry=all.next().getEntryPoint();
     if(!entry.equals(real) && !entry.equals(bad))throw new IllegalStateException("Third function inside merge range");
    }
    currentProgram.getFunctionManager().removeFunction(bad);
    parent.setBody(new AddressSet(real,end.subtract(1)));
    println("MERGED "+bad+" into "+real+" end="+end);
    continue;
   }
   String[] item=a[i].split(":",3);Address addr=toAddr(item[0]);long expected=Long.parseUnsignedLong(item[1],16);
   if(Integer.toUnsignedLong(currentProgram.getMemory().getInt(addr))!=expected)throw new IllegalStateException("Prologue mismatch at "+addr);
   Function f=getFunctionAt(addr);
   if(f==null) {
    if(getFunctionContaining(addr)!=null)throw new IllegalStateException("Conflicting function boundary at "+addr);
    disassemble(addr);f=createFunction(addr,item[2]);
    if(f==null)throw new IllegalStateException("Cannot create function at "+addr);
    println("CREATED "+addr+" "+item[2]);
   }
   f.setName(item[2],SourceType.USER_DEFINED);
  }
 }
}
