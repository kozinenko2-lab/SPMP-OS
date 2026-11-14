//@category SPMP-OS
// Read-only exact-build exporter. It does not create functions or change labels.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

public class SPMP_ExportPiuDispatcher extends GhidraScript {
    private static final String[][] RANGES = {
        {"0x003498F4", "0x0034996C", "mailbox_register_helpers"},
        {"0x00349974", "0x003499F8", "incoming_c2"},
        {"0x003499F8", "0x00349B7C", "incoming_c0"},
        {"0x00349B7C", "0x00349CC8", "incoming_c1"},
        {"0x00349CC8", "0x00349D34", "incoming_c3"},
        {"0x00349D34", "0x00349DB8", "incoming_c4"},
        {"0x00349DB8", "0x00349E3C", "incoming_c5"},
        {"0x00349E3C", "0x00349EF0", "incoming_c6"},
        {"0x00349EF0", "0x0034A034", "piu_isr_dsr"},
        {"0x0034A124", "0x0034A224", "piu_reply_wait"},
        {"0x0034A224", "0x0034A3F4", "piu_copy_and_dispatcher"},
        {"0x0034A3F4", "0x0034A560", "piu_initialize"}
    };

    private long number(String value) {
        return Long.parseLong(value.substring(2), 16);
    }

    private PrintWriter writer(File directory, String name) throws Exception {
        return new PrintWriter(new OutputStreamWriter(
            new FileOutputStream(new File(directory, name)), StandardCharsets.UTF_8));
    }

    @Override
    public void run() throws Exception {
        if (currentProgram == null) throw new IllegalStateException("Open the exact OEM MMI program");
        long prologue = currentProgram.getMemory().getInt(toAddr("0x0034A124")) & 0xffffffffL;
        long initPrologue = currentProgram.getMemory().getInt(toAddr("0x0034A3F4")) & 0xffffffffL;
        if (prologue != 0xE1A0C00DL || initPrologue != 0xE1A0C00DL) {
            throw new IllegalStateException("Wrong MMI image or address mapping");
        }
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("output-directory");
        File output = new File(args[0]);
        if (!output.isDirectory() && !output.mkdirs()) {
            throw new IllegalStateException("Cannot create " + output);
        }
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try (PrintWriter assembly = writer(output, "piu_dispatcher_instructions.txt");
             PrintWriter status = writer(output, "piu_decompile_status.tsv")) {
            assembly.println("Exact-build Ghidra listing; research evidence, not buildable OEM source.");
            status.println("address\tname\tstatus");
            for (String[] range : RANGES) {
                monitor.checkCancelled();
                long begin = number(range[0]);
                long end = number(range[1]);
                assembly.println();
                assembly.println("===== " + range[2] + " " + range[0] + ".." + range[1] + " =====");
                for (long value = begin; value < end; value += 4) {
                    Address address = toAddr(String.format("0x%08X", value));
                    long word = currentProgram.getMemory().getInt(address) & 0xffffffffL;
                    Instruction instruction = currentProgram.getListing().getInstructionAt(address);
                    assembly.printf("0x%08X  %08X  %s%n", value, word,
                        instruction == null ? "(literal/not disassembled)" : instruction.toString());
                }
                Function function = getFunctionAt(toAddr(range[0]));
                if (function == null) {
                    status.println(range[0] + "\t" + range[2] + "\tNOT_DEFINED");
                    continue;
                }
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                if (result.decompileCompleted() && result.getDecompiledFunction() != null) {
                    try (PrintWriter c = writer(output, "decomp_" + range[2] + ".c")) {
                        c.println("/* Ghidra pseudocode; research evidence, not buildable OEM source. */");
                        c.println(result.getDecompiledFunction().getC());
                    }
                    status.println(range[0] + "\t" + range[2] + "\tOK");
                } else {
                    status.println(range[0] + "\t" + range[2] + "\tFAILED");
                }
            }
        } finally {
            decompiler.dispose();
        }
    }
}
