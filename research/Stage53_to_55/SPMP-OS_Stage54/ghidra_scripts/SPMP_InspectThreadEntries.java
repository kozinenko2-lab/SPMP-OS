// SPMP-OS Stage 54, read-only by default.
// Import original 8000_MMI.RAP into Ghidra and run this with Script Manager.
// Names/entrypoints are exact-build hints from OEM static BL and PC literals.
// Set APPLY_RENAMES=true ONLY if you want changes to Ghidra analysis database.
// Never edits the firmware file, NAND, RedBoot or underlying SD card.
//@category SPMP-OS
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class SPMP_InspectThreadEntries extends GhidraScript {
    private static final boolean APPLY_RENAMES = false;
    private static final String[][] TARGETS = {
        {"0x003BE5B0", "SPMP_Demo_GUI_3BE5B0"}, // Demo GUI
        {"0x00291300", "SPMP_watchdog_handle_291300"}, // watchdog_handle
        {"0x002914C8", "SPMP_dsr_handle_2914C8"}, // dsr_handle
        {"0x00293600", "SPMP_dsr_rcd_handle_293600"}, // dsr_rcd_handle
        {"0x002C5FBC", "SPMP_mp3_play_2C5FBC"}, // mp3_play
        {"0x002C6064", "SPMP_pcm_play_2C6064"}, // pcm_play
        {"0x00460AB0", "SPMP_Game_thread_460AB0"}, // Game thread
        {"0x002D5110", "SPMP_Detect_Thread_2D5110"}, // Detect Thread
        {"0x002D50C8", "SPMP_mmiEMG_Thread_2D50C8"}, // mmiEMG Thread
        {"0x002D90F4", "SPMP_Middleware_Mangger_Thread_2D90F4"}, // Middleware Mangger Thread
        {"0x002E0858", "SPMP_media_audio_thread_2E0858"}, // media audio thread
        {"0x002E09B4", "SPMP_media_video_record_thread_2E09B4"}, // media video record thread
        {"0x002E0B84", "SPMP_media_audio_thread_2E0B84"}, // media audio thread
        {"0x002E0F98", "SPMP_media_video_thread_2E0F98"}, // media video thread
        {"0x002E2120", "SPMP_Power_management_thread_2E2120"}, // Power management thread
        {"0x002E28CC", "SPMP_HDMI_HW_CTRL_2E28CC"}, // HDMI HW CTRL
        {"0x002EAD40", "SPMP_TV_Thread_2EAD40"}, // TV Thread
        {"0x00315E14", "SPMP_RWThread_UDC_315E14"}, // RWThread_UDC
        {"0x003219AC", "SPMP_audio_encoding_for_video_3219AC"}, // audio encoding for video
        {"0x003217A0", "SPMP_video_encoding_for_video_3217A0"}, // video encoding for video
        {"0x0032248C", "SPMP_read_bits_thread_32248C"}, // read bits thread
        {"0x00327064", "SPMP_Card_Isp_SD_thread_327064"}, // Card_Isp_SD_thread
        {"0x0032DF60", "SPMP_LCM_Thread_32DF60"}, // LCM Thread
        {"0x0033886C", "SPMP_DSC_FRONT_ISR_33886C"}, // DSC FRONT ISR
        {"0x00338A04", "SPMP_DSC_STATE_THREAD_338A04"}, // DSC STATE THREAD
        {"0x0034A2C4", "SPMP_dspCmdDispatcher_34A2C4"}, // dspCmdDispatcher
        {"0x0034CFD0", "SPMP_GPIO_Monitor_34CFD0"}, // GPIO_Monitor
        {"0x0034F2A4", "SPMP_IRDetect_Thread_34F2A4"}, // IRDetect Thread
        {"0x004473BC", "SPMP_Systen_BackGround_Thread_4473BC"}, // Systen BackGround Thread
        {"0x00447540", "SPMP_Systen_initial_Thread_447540"}, // Systen initial Thread
        {"0x00448A7C", "SPMP_stor_thr_448A7C"}, // stor_thr
        {"0x004625B8", "SPMP_Midi_Seq_thread_4625B8"}, // Midi Seq thread
        {"0x004AF738", "SPMP_storRWConsumer_thread_4AF738"}, // storRWConsumer_thread
        {"0x004B0640", "SPMP_read_buffer_thread_4B0640"}, // read buffer thread
    };
    @Override
    public void run() throws Exception {
        println("SPMP-OS OEM thread map: " + currentProgram.getName());
        println("Expected only for original 8000_MMI.RAP SHA256 5529b173...");
        int hit=0;
        for (String[] item : TARGETS) {
            monitor.checkCancelled();
            Address address=toAddr(item[0]);
            Function f=getFunctionAt(address);
            if (f==null) {
                println(item[0] + " unresolved (Ghidra did not identify a function at entry)");
                continue;
            }
            hit++;
            println(item[0]+ " -> " + f.getName() + "; suggest " + item[1]);
            if (APPLY_RENAMES) {
                try { f.setName(item[1], SourceType.USER_DEFINED); }
                catch (Exception e) { println("rename skipped: " + e.getMessage()); }
            }
        }
        println("Recognized " + hit + " of " + TARGETS.length + " statically named entries.");
        println("Project names changed: " + APPLY_RENAMES);
    }
}
