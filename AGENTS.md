# SPMP-OS — instructions for Codex

This repository is a **research and development handoff**, not production firmware.

## Goal
Reverse engineer original `8000_MMI.RAP` and RedBoot to develop a modern SPMP-OS interface and services for **Defender MultiMix Magic / Sunplus SPMP8000 (ARMv5TE / ARM926EJ-S)**. Prioritize a *safe MMI/eCos-hosted shell* that runs without replacing RedBoot; independent SD boot is a later, separately gated goal.

## Hard safety rules
- NEVER flash, erase, overwrite or mutate original RedBoot, NAND, ROMFS, or stock MMI images. `firmware/originals/` and `firmware/unpacked_FSP/` are READ-ONLY references. Do not generate a deployable flash image without explicit user approval, recovery validation, and device identification.
- ARM ET_REL files and host binaries are **not** device-bootable packages. Do not mark features as hardware-tested without the user confirming on the console.
- All original firmware, Ghidra databases and third-party game assets are user-provided/private; do **not** publish them to public GitHub repositories.
- Treat reverse-engineered addresses as **image-specific**: validate SHA-256 and actual ARM instructions before using. No undocumented MMIO writes, guessed RAM slot, or unchecked function hook.
- A failure reading SD, validating an image or saving a log must not compromise normal factory boot. Never promise a watchdog recovery without verifying the hardware.
- Keep compiler target ARMv5TE, little endian, EABI5, soft-float. Host x86_64 test results do NOT certify target correctness.

## Current implementation priority
- User cannot currently test the console and explicitly asked to continue offline. Do not make device access a prerequisite for host/ARM development or ask for the same hardware report again while unavailable. Existing FILES hardware status remains VM_TESTED_ONLY.
- Stage16 adds spmp_file_copy + FM02 COPY callback: regular files under SPMP-OS/User, 2-KiB steps, exclusive stage, bytewise read-back, required atomic publish_noreplace, cancellation and separate cleanup/orphan status. 50 host/sanitizer/compiled ARM cases; task5740 bytes/test stack2784. No live FAT mutation binding or operations UI yet; do not claim writes in the existing FILES BIN. See research/boot_reconstruction/STAGE16_REPORT_RU.md.
- Original FAT mutation wrappers/coordinator have 22 ARM cases. OEM return codes 200/302 need explicit normalization; FAT fsync0x284100 is no-op. Rename coordinator0x2ab6e8 dispatches to volume driver+0x6c; atomic no-replace semantics still require separate driver analysis. Never substitute stat-then-overwriting-rename for publish_noreplace.
- User selected file manager first, Atari 2600 as the first device emulator. Other bundled cores are considered fresh enough: adapt/build for ARMv5TE; do not bulk replace/downgrade versions.
- Stage15 SPMP_OS_FILES.bin integrates FM01 as a read-only two-panel RAM app with measured buttons, explicit MMI->FM translation and System/User SD folders. It does not yet execute selected ROMs or enable FM02 mutations; do not advertise a finished file manager/Atari player. See research/boot_reconstruction/STAGE15_REPORT_RU.md.
- User supplied Roboto-Regular.ttf and prefers direct TTF use. New shared streaming integer TrueType renderer reads System/Fonts with a 2-KiB IO cache and cached glyphs; no full-file RAM copy or required PC raster conversion. Original Roboto is Apache-2.0 with metadata/license in project/spmp-os/assets/fonts. Built-in glyphs are fallback only. New UI design is still pending.
- Adventure ROM is private; publish only metadata/evidence, not its bytes or game screenshots. Atari adapter accepts external renamed/nested ROM paths and is ARM_OBJECT_ONLY until a runnable Atari package is integrated and user-tested.
- Directory bindings: opendir fd>=1, readdir_r consumes 260-byte names without d_type, stat supplies type/size, closedir closes fd. FS packets are SD0 only; no NAND fallback. Selected Ghidra exports now total 295.

## Ground truth and how to begin
0. Current product scope: `docs/PRODUCT_REQUIREMENTS_RU.md`, `docs/PRODUCT_AUDIT_2026-11-14_RU.md` and `docs/PRODUCT_ROADMAP_RU.md`. The user's seven-part specification supersedes historical feature plans. Existing themes are explicitly rejected as the final design; redesign details will come later. Reuse useful algorithms/tests, do not undertake mass deletion or claim integrated device support from independent host demos. Include the user's ULI/homebrew references and NGP/MSX/Doom/Quake candidates; see `docs/evidence/uli_spmp_sources_2026-11-14.json`.
1. `README_RU.md` and `docs/STATUS_ROADMAP_RU.md` for architecture + limitations.
2. `firmware/originals/8000_MMI.RAP`: source of truth, SHA-256 `5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e`.
3. `ghidra/user_GZF/`: original user's three Ghidra snapshots. `ghidra/Stage52_ELF_Imports/`: prepared ARM ELF input images. `research/Stage53_to_55/`: function/callgraph work.
4. `project/spmp-os/`: merged current Stage49 source + original chat branch multimedia/theme/FM01–FM05 patches.
5. `integration/chat_merge_report.json`: merge provenance; no content conflicts at assembly.

## Fast reproducible checks
```bash
python3 tools/verify_integrity.py --quick
make -C project/spmp-os host
(cd project/spmp-os && ./build/spmp-os-host --cores)
for n in 01 02 03 04 05; do (cd project/spmp-os/features/filemanager/FM$n && sh tools/build.sh); done
SPMP_MMI_RAP=firmware/originals/8000_MMI.RAP python3 -m unittest discover -s research/Stage53_to_55/SPMP-OS_Stage55/tests -v
```
For a full archive checksum audit use `python3 tools/verify_integrity.py --full`.

## First tasks
- Trace MMI startup => eCos thread creation => Demo GUI => system init => app launch, preserving hardware side effects (notably `0x00447640` touches `0x90005040`).
- Add descriptive Ghidra symbols and export decompilation with evidence. State facts, hypotheses and confidence separately.
- Trace FAT, UART, GPIO, LCD, audio, power and game loader interfaces. Provide a documented HAL ABI and map of writable/overwritten RAM; do not execute guessed bindings.
- Integrate useful FM01–FM05 components through common input/VFS/font services, with host tests before ARM cross build. Theme v9 is reference material only: the user requested a full theme redesign after providing more detail. Native hardware integration is incomplete. Next shared-foundation work includes input bit-layout adapters, forwarding file_size through the UI router, and player-index support; see product audit A01–A05.
- For device diagnostics, prefer a verified MMI-hosted app under the correct OEM container format. The older `MMI_DIAG.bin` failed with «неверный формат»; do not reuse that file as known-good.
- The user confirmed on 2026-11-14 that `SPMP_PROBE_VM.bin` from artifact 14076639604 displayed three colored bars and exited on the console. See `research/boot_reconstruction/evidence/native_probe_hardware_user_report.json`. Hardware confirmation is limited to that one-frame/exit observation; buttons, repeated/long runs, full UI and RTOS cleanup remain unverified.

- Stage10 SD open failed; Stage11 corrected the path. The user then supplied a complete SD0_R2 CSV (4380 bytes, 76 events, END): positive hardware evidence for one journal session, not repeated-run durability/final return after END. See research/boot_reconstruction/STAGE12_REPORT_RU.md. Actual fat_sd_init registers SD id0 (/fat20a0/hda0); MSDC id1 and NAND id2. Do not identify media from misleading stock log text.
- Seven prompt-associated cooked mappings were recorded. The user confirms X/Y/L/R/SELECT/BACK exist and were pressed but produced no samples; BACK was later clarified as short POWER. HOLD is a separate lock key, and volume +/- also exist. The legacy test did not prompt HOLD or volume. Do not call them absent or alias volume to L/R. Kind6 GetKeys filters input; the incomplete device profile must not be applied universally.

- Stage14: user RAW CSV contains two complete sessions (400 events), 14 repeated raw state masks and POWER event1000. User reports stock POWER menu absent in RAW_R1 and keyboard lock after exit. Do not describe RAW_R1 system UI as working. The user explicitly deferred HOLD; do not create more HOLD tests unless requested. See research/boot_reconstruction/STAGE14_REPORT_RU.md.
- New SPMP_OS_MINI.bin links project MMI input/menu modules: state for 14 controls, original collector once per poll for POWER, own Continue/Exit menu, SD0 SPMPOS.CSV. No cooked GetKeys, brightness, emulator or filesystem browser yet. VM_TESTED_ONLY until user confirms this exact version. Long POWER turns off and MUST NOT be a guided test.
- Input gate 0x556aa0 and GUI lock 0x561078 are separate lock flags; do not call the first merely a menu-active flag. Read-only snapshots must not overwrite them. HOLD reader 0x34f6dc configures GPIO/MMIO and is NOT a read-only RAM getter. These flags were not in V2 hardware log, so they cannot prove the cause of RAW_R1 menu/lock behavior.

## Reporting discipline
Produce patches, tests, disassembly evidence and updated `docs/STATUS_ROADMAP_RU.md` for each task. Keep original binaries unchanged. A working host preview is not a working firmware.
