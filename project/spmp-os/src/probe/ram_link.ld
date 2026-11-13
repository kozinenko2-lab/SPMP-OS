/* The address must be validated on the device before this script is used.
 * Pass -Wl,--defsym=DEFOS_RAM_BASE=0x... when linking. */
ENTRY(defos_probe_entry)
SECTIONS {
 . = DEFOS_RAM_BASE;
 .text : { *(.text.probe) *(.text*) }
 . = ALIGN(4);
 .data : { *(.data.probe) *(.data*) }
 . = ALIGN(4);
 .bss : { *(.bss*) *(COMMON) }
 /DISCARD/ : { *(.comment) *(.note*) *(.ARM.exidx*) *(.eh_frame*) }
}
ASSERT(DEFINED(DEFOS_RAM_BASE), "Verified RAM address required!");
