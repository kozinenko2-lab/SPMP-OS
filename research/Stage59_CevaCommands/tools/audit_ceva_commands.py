#!/usr/bin/env python3
"""Read-only exact-build audit of the OEM MJPEG encoder command ABI."""
import argparse
import importlib.util
import json
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SPEC=importlib.util.spec_from_file_location("stage56_audit",ROOT/"Stage56_MediaLoader"/"tools"/"audit_media_loader.py")
S56=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(S56)

CALLS={
 "c0_mailbox":(0x00349510,0x0034A034),"c1_mailbox":(0x003495E0,0x0034A034),
 "c2_mailbox":(0x00349674,0x0034A034),"c0_copy_60":(0x00349544,0x00298C6C),
 "c1_copy_60":(0x00349610,0x00298C6C),"c2_copy_60":(0x003496A8,0x00298C6C),
 "encoder_init_c1":(0x00321578,0x0034955C),"init_retry_cleanup_c2":(0x0032159C,0x00349638),
 "init_retry_c1":(0x003215B0,0x0034955C),"frame_c0":(0x003218CC,0x003494D4),
 "frame_report":(0x003218D0,0x00321298),"normal_close_c2":(0x00321670,0x00349638),
 "worker_init":(0x00321810,0x00321304),"controller_close":(0x00321E00,0x00321644),
 "legacy_c0_mailbox":(0x00348A48,0x0034A034),"legacy_c1_mailbox":(0x00348B30,0x0034A034),
 "legacy_c2_mailbox":(0x00348C04,0x0034A034),
}

WORDS={
 "c0_opcode":(0x003494F8,0xE3E0303F),"c0_payload_length":(0x00349504,0xE2833044),
 "c0_control_pointer":(0x0034950C,0xE5805004),"c0_alias":(0x0034953C,0xE3851201),
 "c0_copy_bytes":(0x00349540,0xE3A02060),"c1_opcode":(0x003495C8,0xE3E0303E),
 "c1_payload_length":(0x003495D4,0xE2833043),"c1_control_pointer":(0x003495DC,0xE5804004),
 "c1_alias":(0x00349608,0xE3841201),"c1_copy_bytes":(0x0034960C,0xE3A02060),
 "c2_opcode":(0x0034965C,0xE3E0303D),"c2_payload_length":(0x00349668,0xE2833042),
 "c2_control_pointer":(0x00349670,0xE5805004),"c2_alias":(0x003496A0,0xE3851201),
 "c2_copy_bytes":(0x003496A4,0xE3A02060),"control_global_init":(0x00321618,0x0055866C),
 "control_global_worker":(0x00321978,0x0055866C),"init_zero_60":(0x0032132C,0xEBFDDE80),
 "field_width_18":(0x0032133C,0xE5841018),"field_height_1c":(0x00321344,0xE584201C),
 "field_fps_24":(0x0032134C,0xE5843024),"field_allocation_10":(0x00321388,0xE5840010),
 "field_result_58":(0x00321580,0xE5933058),"frame_output_4c":(0x003218BC,0xE586304C),
 "frame_input_04":(0x003218C4,0xE5863004),"frame_size_50":(0x003218D8,0xE5961050),
 "legacy_c0_copy_44":(0x00348A78,0xE3A02044),"legacy_c1_copy_44":(0x00348B5C,0xE3A02044),
 "legacy_c2_copy_44":(0x00348C30,0xE3A02044),
}

STRINGS={
 "encoder_os_ext":(0x004EB538,"/Rom/image/mjpgenc_os_ext.img"),
 "encoder_os_int":(0x004EB5B0,"/Rom/image/mjpgenc_os_int.img"),
 "encoder_codec":(0x004EB628,"/Rom/image/mjpgenc_codec.img"),
 "frame_error":(0x004EB780,"cx_test: Encode1Frame() err=%d\n"),
 "frame_failed":(0x004EB7A0,"cx_test: Encode1Frame() failed. err=%d\n"),
 "init_dimensions":(0x004EB7F0,"wid: %d, hei: %d, fps: %d, buff: 0x%x \n"),
 "init_rate":(0x004EB818,"rcMode: %d, gopLen: %d, bitRate: %d, flags: 0x%x \n"),
 "null_handle":(0x004EB84C,"[video_rec_codec_init] g_st_vid_enc.pEncHandle = NULL\n"),
 "encode_init":(0x004EB884,"cx_test: EncodeInit() err=%d\n"),
 "encode_init_return":(0x004EB8A4,"cx_test: EncodeInit() returns %d\n"),
 "encode_init_failed":(0x004EB8C8,"cx_test: EncodeInit() failed. err=%d\n"),
 "vlc_warning":(0x004EB8F0,"Warning !! vlcSize = %d > maxVlcSize\n"),
 "legacy_jpeg_os_ext":(0x004EC028,"/jpgdec_os_ext.img"),
 "legacy_jpeg_os_int":(0x004EC03C,"/jpgdec_os_int.img"),
 "legacy_jpeg_codec":(0x004EC050,"/jpgdec.img"),
}


def command_packet(opcode:int,control_pointer:int)->bytes:
 if opcode not in (0xC0,0xC1,0xC2):raise ValueError("unsupported opcode")
 if control_pointer<0 or control_pointer>0xffffffff:raise ValueError("pointer outside ARM32")
 return struct.pack("<BBHI",opcode,0,4,control_pointer)


def checked_uncached_alias(pointer:int,size:int,pool_begin:int,pool_end:int)->int:
 if pointer<0 or size<=0 or pool_begin<0 or pool_end<=pool_begin:return 0
 if pointer&3 or pool_begin&3 or pool_end&3:return 0
 if pool_end>0x10000000 or pointer<pool_begin or pointer>=pool_end:return 0
 if size>pool_end-pointer or size>0x10000000-pointer:return 0
 return pointer|0x10000000


def raw_cstring(fw,address,limit=256):
 offset=address-S56.SEGMENT_BASE
 raw=fw.segment[offset:offset+limit];end=raw.find(b"\0")
 if offset<0 or end<0:raise ValueError(f"bad string at 0x{address:08x}")
 return raw[:end].decode("ascii")


def audit(rap:Path)->dict:
 fw=S56.Firmware(rap.read_bytes());calls={};words={};strings={}
 for name,(site,target) in CALLS.items():
  actual=fw.bl_target(site)
  if actual!=target:raise ValueError(f"BL mismatch at 0x{site:08x}: {actual!r}")
  calls[name]={"callsite":f"0x{site:08x}","target":f"0x{target:08x}"}
 for name,(address,expected) in WORDS.items():
  actual=fw.u32(address)
  if actual!=expected:raise ValueError(f"word mismatch at 0x{address:08x}: 0x{actual:08x}")
  words[name]={"address":f"0x{address:08x}","value":f"0x{actual:08x}"}
 for name,(address,expected) in STRINGS.items():
  actual=raw_cstring(fw,address)
  if actual!=expected:raise ValueError(f"string mismatch at 0x{address:08x}")
  strings[name]={"address":f"0x{address:08x}","text":actual}
 return {
  "research_only":True,"source_rap_sha256":fw.sha256,"verified_calls":calls,
  "verified_words":words,"verified_strings":strings,
  "mjpeg_encoder_abi":{
   "commands":{"0xc1":"EncodeInit","0xc0":"Encode1Frame","0xc2":"close/free"},
   "wire":"8 bytes: opcode u8, zero u8, payload length u16 LE = 4, cached ARM pointer u32 LE",
   "reply":"copy 0x60 bytes from pointer | 0x10000000 to the cached control structure",
   "control_fields":{"0x04":"input frame pointer","0x10":"allocated encoder workspace/handle",
    "0x14":"workspace size","0x18":"width","0x1c":"height","0x20":"bit rate/config value",
    "0x24":"fps","0x28":"GOP length","0x44":"rate-control mode","0x4c":"VLC output buffer",
    "0x50":"produced VLC bytes","0x54":"flags","0x58":"init result/handle-ready flag"},
   "control_global":"0x0055866c","init_retry":"unbounded in OEM code while +0x58 is zero",
  },
  "separate_legacy_abi":{"control_bytes":0x44,"commands":["0xc0","0xc1","0xc2"],
   "identity":"JPEG decoder paths and callers; same opcodes do not imply the same structure"},
  "provider_requirements":["configured physical shared pool must remain below 0x10000000",
   "validate alignment, overflow and complete [pointer,pointer+size) containment before adding the alias bit",
   "finite mailbox and init timeouts; never copy the OEM unbounded retry loop",
   "keep the 0x44 JPEG decoder and 0x60 MJPEG encoder layouts as distinct typed ABIs"],
  "limits":["No CEVA IMG, mailbox, MMIO or codec was executed.",
   "Dynamic shared-pool bounds were not recovered, so no production pool address is guessed.",
   "The OEM close path frees +0x10 before C2; this ordering is recorded, not recommended for blind reuse."],
 }


def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument("--rap",required=True,type=Path);p.add_argument("--output",required=True,type=Path);a=p.parse_args()
 result=audit(a.rap);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,indent=2,ensure_ascii=False)+"\n")
 print(f"PASS: {len(result['verified_calls'])} calls, {len(result['verified_words'])} words, {len(result['verified_strings'])} strings")
if __name__=="__main__":main()
