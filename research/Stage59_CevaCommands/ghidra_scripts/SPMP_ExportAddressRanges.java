//@category SPMP-OS
// Read-only instruction/data export for explicitly supplied address ranges.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class SPMP_ExportAddressRanges extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) throw new IllegalArgumentException("output-dir start-end[:label] ...");
        Path output = Paths.get(args[0]);
        Files.createDirectories(output);
        for (int i = 1; i < args.length; i++) {
            String[] named = args[i].split(":", 2);
            String[] bounds = named[0].split("-", 2);
            if (bounds.length != 2) throw new IllegalArgumentException(args[i]);
            Address start = toAddr(bounds[0]);
            Address end = toAddr(bounds[1]);
            String label = named.length == 2 ? named[1] : bounds[0] + "_" + bounds[1];
            label = label.replaceAll("[^a-zA-Z0-9_]", "_");
            try (PrintWriter writer = new PrintWriter(Files.newBufferedWriter(
                    output.resolve(label + ".asm"), StandardCharsets.UTF_8))) {
                InstructionIterator instructions = currentProgram.getListing().getInstructions(
                    new AddressSet(start, end), true);
                while (instructions.hasNext()) {
                    monitor.checkCancelled();
                    Instruction instruction = instructions.next();
                    writer.println(instruction.getAddress() + " " + instruction);
                }
            }
        }
    }
}
