//@category SPMP-OS
// Read-only scan for address-alias and CP15/MMU instructions.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

public class SPMP_ScanMmuAlias extends GhidraScript {
    private static String quote(String value) {
        return "\"" + value.replace("\"", "\"\"") + "\"";
    }
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("output.csv");
        File output = new File(args[0]);
        File parent = output.getParentFile();
        if (parent != null && !parent.isDirectory() && !parent.mkdirs()) {
            throw new IllegalStateException("Cannot create " + parent);
        }
        try (PrintWriter out = new PrintWriter(new OutputStreamWriter(
                new FileOutputStream(output), StandardCharsets.UTF_8))) {
            out.println("program,address,kind,instruction,function");
            InstructionIterator iterator = currentProgram.getListing().getInstructions(true);
            while (iterator.hasNext()) {
                monitor.checkCancelled();
                Instruction instruction = iterator.next();
                String text = instruction.toString();
                String mnemonic = instruction.getMnemonicString().toLowerCase();
                String kind = null;
                if (text.contains("0x10000000") || text.contains("0xefffffff")) {
                    kind = "alias";
                } else if ((mnemonic.equals("mcr") || mnemonic.equals("mrc")) &&
                           text.toLowerCase().contains("p15")) {
                    kind = "cp15";
                }
                if (kind == null) continue;
                Function function = currentProgram.getFunctionManager().getFunctionContaining(instruction.getAddress());
                out.println(quote(currentProgram.getName()) + "," + instruction.getAddress() + "," +
                    kind + "," + quote(text) + "," + quote(function == null ? "" : function.getName()));
            }
        }
    }
}
