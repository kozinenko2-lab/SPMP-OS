//@category SPMP-OS
// Private analysis only. Do not export the game's payload, assets or database.
import ghidra.app.script.GhidraScript;
import java.security.MessageDigest;
import java.util.HexFormat;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.SourceType;
public class SPMP_PrepareControlGame extends GhidraScript {
 public void run() throws Exception {
  byte[] bytes=new byte[0x3381d0];
  if(currentProgram.getMemory().getBytes(toAddr(0xa00000),bytes)!=bytes.length ||
     !HexFormat.of().formatHex(MessageDigest.getInstance("SHA-256").digest(bytes)).equals(
      "f312eafba8f7dbf93afb5897ed127d1081e929181b15386bbaf191912ab539da"))
   throw new IllegalStateException("Exact decoded control payload SHA-256 required");
  if(currentProgram.getMemory().getInt(toAddr(0xa00000))!=0xe59f30c0 ||
     currentProgram.getMemory().getInt(toAddr(0xa000c8))!=0xd3e000)
   throw new IllegalStateException("Unexpected control-game startup");
  Memory memory=currentProgram.getMemory();MemoryBlock payload=memory.getBlock(toAddr(0xa00000));
  if(payload==null)throw new IllegalStateException("Payload not mapped at expected base");
  memory.split(payload,toAddr(0xa3b6ac));
  memory.getBlock(toAddr(0xa00000)).setExecute(true);
  memory.getBlock(toAddr(0xa3b6ac)).setExecute(false);
  // This text/data split is an analysis partition, not recovered ELF sections.
  MemoryBlock bss=memory.createUninitializedBlock("control_bss_analysis",toAddr(0xd3e000),0x160c,false);
  bss.setRead(true);bss.setWrite(true);bss.setExecute(false);
  createLabel(toAddr(0xd3e000),"control_api_pointer",true);
  createLabel(toAddr(0xd3ef0c),"control_bss_clear_end",true);
  for(String item:new String[]{"a00000:control_entry","a21294:control_graphics_initialize",
      "a322e0:control_main","a32fa4:control_heap_set_limit","a32fb4:control_heap_initialize",
      "a330d4:control_heap_layout_initialize","a33374:control_heap_blocks_initialize"}) {
   String[] parts=item.split(":");Address at=toAddr(parts[0]);disassemble(at);
   if(getFunctionAt(at)==null)createFunction(at,parts[1]);
   getFunctionAt(at).setName(parts[1],SourceType.USER_DEFINED);
   currentProgram.getSymbolTable().addExternalEntryPoint(at);
  }
 }
}
