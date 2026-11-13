//@category SPMP-OS
// Analysis only. Import GZF into a scratch project; never overwrite source GZF.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.io.*;

public class SPMP_ExportBoot extends GhidraScript {
 public void run() throws Exception {
  String[] a=getScriptArgs();
  if(a.length<1) throw new IllegalArgumentException("output-dir [hex:name ...]");
  Path out=Paths.get(a[0]);Files.createDirectories(out);
  try(PrintWriter w=new PrintWriter(Files.newBufferedWriter(out.resolve("program.txt")))) {
   w.println("program="+currentProgram.getName());w.println("language="+currentProgram.getLanguageID());
   w.println("compiler="+currentProgram.getCompilerSpec().getCompilerSpecID());
   w.println("functions="+currentProgram.getFunctionManager().getFunctionCount());
   for(MemoryBlock b:currentProgram.getMemory().getBlocks()) w.println("block="+b.getName()+","+b.getStart()+","+b.getEnd()+",execute="+b.isExecute());
  }
  try(PrintWriter w=new PrintWriter(Files.newBufferedWriter(out.resolve("functions.tsv")))) {
   w.println("address\tname\tbody_bytes\tcallers");
   FunctionIterator fs=currentProgram.getFunctionManager().getFunctions(true);
   while(fs.hasNext()) {Function f=fs.next();w.println(f.getEntryPoint()+"\t"+f.getName()+"\t"+f.getBody().getNumAddresses()+"\t"+f.getCallingFunctions(monitor).size());}
  }
  DecompInterface d=new DecompInterface();d.openProgram(currentProgram);
  try(PrintWriter status=new PrintWriter(Files.newBufferedWriter(out.resolve("status.tsv")))) {
   status.println("address\tlabel\tstatus");
   for(int i=1;i<a.length;i++) {
    monitor.checkCancelled();String[] item=a[i].split(":",2);Address addr=toAddr(item[0]);
    String label=item.length>1?item[1]:item[0];Function f=getFunctionAt(addr);
    if(f==null) {status.println(addr+"\t"+label+"\tNOT_DEFINED");continue;}
    String filename=label.replaceAll("[^a-zA-Z0-9_]","_");
    DecompileResults dr=d.decompileFunction(f,60,monitor);
    if(dr.decompileCompleted()&&dr.getDecompiledFunction()!=null) {
     Files.writeString(out.resolve(filename+".c"),"/* Ghidra pseudocode; not recovered buildable OEM source. */\n/* entry="+addr+"; original_symbol="+f.getName()+" */\n"+dr.getDecompiledFunction().getC(),StandardCharsets.UTF_8);
     status.println(addr+"\t"+label+"\tOK");
    } else status.println(addr+"\t"+label+"\tFAILED "+dr.getErrorMessage());
    try(PrintWriter w=new PrintWriter(Files.newBufferedWriter(out.resolve(filename+".asm")))) {
     InstructionIterator ins=currentProgram.getListing().getInstructions(f.getBody(),true);
     while(ins.hasNext()){Instruction x=ins.next();w.println(x.getAddress()+" "+x.toString());}
     w.println("REFERENCES TO ENTRY:");
     for(Reference ref:getReferencesTo(addr)) w.println(ref.getFromAddress()+" "+ref.getReferenceType());
    }
   }
  } finally {d.dispose();}
 }
}
