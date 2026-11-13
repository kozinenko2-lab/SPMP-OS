// SPDX-License-Identifier: MIT
// @category SPMP-OS
// Read-only Ghidra program analysis exporter for the actual user-supplied GZF projects.
// Tested here: static inspection only. Requires Ghidra CodeBrowser or analyzeHeadless to execute.
// Run with optional argument: absolute path to an output directory.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.Symbol;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.LinkedHashSet;
import java.util.Set;

public class SPMP_ExportArchitecture extends GhidraScript {
    private static final String[] MMI = {
       "0x00280040", "0x00298B64", "0x00298B68", "0x002B9A38", "0x002B9B78",
       "0x00283F3C", "0x00283FC4", "0x00284100", "0x00284108",
       "0x0046018C", "0x004611BC", "0x00461268", "0x004618C4", "0x00513CDC"
    };
    private static final String[] RB1 = {"0x00030040", "0x00038F0C", "0x0004A998", "0x0004A9C4", "0x0004AA44", "0x0004AA48"};
    private static final String[] RB3 = {"0x00030040", "0x00036BF4", "0x000467F4", "0x00046818", "0x0004689C", "0x000468A0"};
    private static String csv(String s) {
        if (s == null) return "\"\"";
        return "\"" + s.replace("\r", " ").replace("\n", " ").replace("\"", "\"\"") + "\"";
    }
    private static PrintWriter writer(File f) throws Exception {
        return new PrintWriter(new OutputStreamWriter(new FileOutputStream(f), StandardCharsets.UTF_8));
    }
    private static String safe(String s) { return s.replaceAll("[^A-Za-z0-9_.-]", "_"); }
    private String fname(Function f) { return f == null ? "" : f.getName(); }
    private String functionAt(Address address) {
        if (address == null) return "";
        return fname(currentProgram.getFunctionManager().getFunctionContaining(address));
    }
    @Override
    public void run() throws Exception {
        if (currentProgram == null) throw new IllegalStateException("No current program");
        String[] args = getScriptArgs();
        String chosen = args.length > 0 ? args[0] : System.getProperty("user.home") + File.separator + "SPMP_OS_Ghidra_Export";
        File base = new File(chosen, safe(currentProgram.getName()));
        if (!base.exists() && !base.mkdirs()) throw new IllegalStateException("Cannot create " + base);
        println("SPMP-OS read-only export: " + currentProgram.getName() + " -> " + base);
        try (PrintWriter p = writer(new File(base,"program.txt"))) {
            p.println("Name=" + currentProgram.getName());
            p.println("Language=" + currentProgram.getLanguageID());
            p.println("Compiler=" + currentProgram.getCompilerSpec().getCompilerSpecID());
            p.println("ExecutablePath=" + currentProgram.getExecutablePath());
            p.println("ImageBase=" + currentProgram.getImageBase());
            ghidra.program.model.address.AddressIterator entries = currentProgram.getSymbolTable().getExternalEntryPointIterator();
            while (entries.hasNext()) p.println("ExternalEntry=" + entries.next());
        }
        try (PrintWriter p = writer(new File(base,"memory.csv"))) {
            p.println("name,start,end,bytes,read,write,execute,initialized");
            for (MemoryBlock b : currentProgram.getMemory().getBlocks())
                p.println(csv(b.getName())+","+csv(b.getStart().toString())+","+csv(b.getEnd().toString())+","+b.getSize()+","+b.isRead()+","+b.isWrite()+","+b.isExecute()+","+b.isInitialized());
        }
        try (PrintWriter functions = writer(new File(base,"functions.csv"));
             PrintWriter calls = writer(new File(base,"calls.csv"))) {
            functions.println("entry,name,size,calling_convention,thunk,external,signature");
            calls.println("caller_entry,caller_name,callee_entry,callee_name");
            FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
            while (it.hasNext()) {
                monitor.checkCancelled(); Function f=it.next();
                functions.println(csv(f.getEntryPoint().toString())+","+csv(f.getName())+","+f.getBody().getNumAddresses()+","+csv(f.getCallingConventionName())+","+f.isThunk()+","+f.isExternal()+","+csv(f.getSignature().toString()));
                for (Function callee : f.getCalledFunctions(monitor))
                    calls.println(csv(f.getEntryPoint().toString())+","+csv(f.getName())+","+csv(callee.getEntryPoint().toString())+","+csv(callee.getName()));
            }
        }
        try (PrintWriter p = writer(new File(base,"strings.csv"))) {
            p.println("address,length,value");
            DataIterator di=currentProgram.getListing().getDefinedData(true);
            while(di.hasNext()) {
                monitor.checkCancelled(); Data d=di.next(); Object val=d.getValue();
                if(val instanceof String) p.println(csv(d.getAddress().toString())+","+d.getLength()+","+csv((String) val));
            }
        }
        String lo=currentProgram.getName().toLowerCase();
        String[] targets=lo.contains("v1") ? RB1 : (lo.contains("v3") ? RB3 : MMI);
        ReferenceManager rm=currentProgram.getReferenceManager();
        Set<String> roots = new LinkedHashSet<String>();
        try(PrintWriter p=writer(new File(base,"priority_references.csv"))) {
            p.println("target,kind,source,type,source_function,target_function");
            for(String addr: targets) {
                Address a=toAddr(addr); if(a==null) continue;
                Function containing=currentProgram.getFunctionManager().getFunctionContaining(a);
                if(containing!=null) roots.add(containing.getEntryPoint().toString());
                for(Reference ref:rm.getReferencesTo(a))
                    p.println(csv(a.toString())+",incoming,"+csv(ref.getFromAddress().toString())+","+csv(ref.getReferenceType().toString())+","+csv(functionAt(ref.getFromAddress()))+","+csv(functionAt(a)));
                for(Reference ref:rm.getReferencesFrom(a))
                    p.println(csv(a.toString())+",outgoing,"+csv(ref.getToAddress().toString())+","+csv(ref.getReferenceType().toString())+","+csv(functionAt(a))+","+csv(functionAt(ref.getToAddress())));
            }
        }
        DecompInterface decomp=new DecompInterface();
        if(!decomp.openProgram(currentProgram)) {println("WARN: decompiler unavailable; other exports complete"); return;}
        try {
            int index=0;
            for(String addr: roots) {
                monitor.checkCancelled();Address a=toAddr(addr);
                Function f=currentProgram.getFunctionManager().getFunctionAt(a);
                if(f==null || f.isExternal())continue;
                DecompileResults res=decomp.decompileFunction(f,45,monitor);
                File out=new File(base,"decomp_"+safe(addr)+".c");
                try(PrintWriter p=writer(out)) {
                    p.println("/* READ-ONLY GHIDRA OUTPUT; PSEUDOCODE, NOT COMPILABLE FIRMWARE */");
                    p.println("/* Program: "+currentProgram.getName()+"; Function: "+f.getName()+" */");
                    p.println("/* Entry: "+f.getEntryPoint()+" */");
                    if(res!=null && res.decompileCompleted() && res.getDecompiledFunction()!=null) p.println(res.getDecompiledFunction().getC());
                    else p.println("/* Could not decompile; inspect original ARM disassembly. */");
                }
                index++;
            }
            println("SPMP-OS export complete: "+index+" priority decompilations; full function/callgraph CSV in "+base);
        } finally {decomp.dispose();}
        // This script does not alter Program memory, comments, symbols, function signatures or project files.
    }
}
