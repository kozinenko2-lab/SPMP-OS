//@category SPMP-OS
// READ-ONLY Stage55 exporter for the original Defender/SPMP8000 8000_MMI.RAP
// Changes neither the Ghidra project nor the device's firmware.
// Script Manager: run once on your MMI GZF (not on RedBoot).
// Optional script argument: output directory. Default: ~/SPMP_OS_Stage55_Export
// Never patch flash / RAP / RedBoot with these findings.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

public class SPMP_Stage55_GUI_DeepExport extends GhidraScript {
    private static final String[][] TARGETS = {
        {"0x00280040", "MMI_reset", "0x002804F8"},
        {"0x00297CE8", "MMI_startup", "0x00297D0C"},
        {"0x002807C8", "create_Demo_GUI", "0x0028083C"},
        {"0x003BE5B0", "Demo_GUI", "0x003BE6A0"},
        {"0x00447640", "GUI_system_coordinator", "0x004476CC"},
        {"0x004475C0", "create_System_init", "0x00447640"},
        {"0x00447540", "System_init_thread", "0x004475C0"},
        {"0x004474D8", "System_init_retry", "0x00447540"},
        {"0x00447458", "create_BackGround", "0x004474D8"},
        {"0x004473BC", "BackGround_thread", "0x00447458"},
        {"0x00339AAC", "eCos_thread_wrapper", "0x00339B04"},
        {"0x0033A2DC", "eCos_thread_core", "0x0033A484"}
    };
    private PrintWriter makeWriter(File base, String name) throws Exception {
        return new PrintWriter(new OutputStreamWriter(new FileOutputStream(new File(base, name)), StandardCharsets.UTF_8));
    }
    private static String esc(String s) {
        if (s == null) return "\"\"";
        return "\"" + s.replace("\"","\"\"").replace("\r"," ").replace("\n"," ") + "\"";
    }
    private String safeName(String s) { return s.replaceAll("[^A-Za-z0-9_.-]", "_"); }
    private long parseAddress(String s) { return Long.parseLong(s.replace("0x", ""), 16); }
    @Override
    public void run() throws Exception {
        if (currentProgram == null) throw new IllegalStateException("Open MMI in CodeBrowser first");
        // Check ORIGINAL OEM words; project contains virtual ELF created from supplied RAP.
        String[][] expected = {
            {"0x003BE5B0", "E1A0C00D"},
            {"0x00447668", "E3A02209"},
            {"0x0044767C", "E5823000"},
            {"0x00447540", "E1A0C00D"}
        };
        for (String[] check : expected) {
            Address a = toAddr(check[0]);
            long observed = currentProgram.getMemory().getInt(a) & 0xffffffffL;
            long wanted = Long.parseLong(check[1],16);
            if (observed != wanted) throw new IllegalStateException("Wrong MMI / imported address at " + check[0]);
        }
        String[] args = getScriptArgs();
        File out = new File(args.length > 0 ? args[0] :
            System.getProperty("user.home") + File.separator + "SPMP_OS_Stage55_Export");
        if (!out.isDirectory() && !out.mkdirs()) throw new IllegalStateException("Cannot create export dir: " + out);
        println("SPMP-OS Stage55: READ ONLY: " + currentProgram.getName() + " -> " + out);
        DecompInterface decomp = new DecompInterface();
        boolean decompReady = decomp.openProgram(currentProgram);
        int recognized = 0, decompiled = 0;
        try (PrintWriter table=makeWriter(out,"target_functions.csv");
             PrintWriter refs=makeWriter(out,"references.csv");
             PrintWriter asm=makeWriter(out,"instructions.txt")) {
            table.println("name,address,next_prologue_hint,exact_function,containing_function,decompile_status");
            refs.println("target,from,reference_type,source_function");
            for (String[] t : TARGETS) {
                monitor.checkCancelled();
                Address addr=toAddr(t[0]);
                Function exact = getFunctionAt(addr);
                Function enclosing = currentProgram.getFunctionManager().getFunctionContaining(addr);
                String msg="NOT_DEFINED";
                if (exact != null) {
                    recognized++;
                    if (decompReady && !exact.isExternal()) {
                        DecompileResults dr=decomp.decompileFunction(exact,30,monitor);
                        if (dr!=null && dr.decompileCompleted() && dr.getDecompiledFunction()!=null) {
                            try (PrintWriter p=makeWriter(out,"decomp_"+safeName(t[1])+".c")) {
                                p.println("/* Ghidra pseudo-C ONLY; not compilable firmware sources */");
                                p.println("/* "+t[1]+" at "+t[0]+" */");
                                p.println(dr.getDecompiledFunction().getC());
                            }
                            decompiled++;msg="OK";
                        } else msg="FAILED_DECOMPILATION";
                    } else msg="SKIPPED";
                }
                table.println(esc(t[1])+","+esc(t[0])+","+esc(t[2])+","+
                    esc(exact==null?"":exact.getName())+","+
                    esc(enclosing==null?"":enclosing.getEntryPoint().toString()+"/"+enclosing.getName())+","+esc(msg));
                for (Reference ref : currentProgram.getReferenceManager().getReferencesTo(addr)) {
                    Function source=currentProgram.getFunctionManager().getFunctionContaining(ref.getFromAddress());
                    refs.println(esc(t[0])+","+esc(ref.getFromAddress().toString())+","+
                        esc(ref.getReferenceType().toString())+","+esc(source==null?"":source.getName()));
                }
                asm.println("===== " + t[1] + " " + t[0] + " =====");
                long begin=parseAddress(t[0]);
                long end=parseAddress(t[2]);
                for (long v=begin;v<end;v+=4) {
                    monitor.checkCancelled();
                    Address a=toAddr(String.format("0x%08X",v));
                    long raw=currentProgram.getMemory().getInt(a)&0xffffffffL;
                    Instruction i=currentProgram.getListing().getInstructionAt(a);
                    asm.printf("0x%08X  %08X  %s%n",v,raw,i!=null?i.toString():"(not disassembled / may be literal data)");
                }
                asm.println();
            }
        } finally { decomp.dispose(); }
        println("Exported " + TARGETS.length + " addresses, " + recognized + " exact functions, " + decompiled + " C outputs.");
        println("No addresses, symbols, memory, or function definitions were changed.");
    }
}
