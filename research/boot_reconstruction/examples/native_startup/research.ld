/* Analysis-only layout at the confirmed kind-6 BIN entry.
 * 128 KiB is an explicit research size limit, NOT discovered free RAM.
 * No NGame header/footer, image builder, heap or new thread stack.
 */
OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(_spmp_start)
MEMORY { analysis (rwx) : ORIGIN = 0x00a00000, LENGTH = 128K }
PHDRS { code PT_LOAD FLAGS(5); data PT_LOAD FLAGS(6); }
SECTIONS {
    . = ORIGIN(analysis);
    __image_start__ = .;
    .text : { KEEP(*(.text.spmp_start)) *(.text .text.*) } > analysis :code
    .rodata : ALIGN(4) { *(.rodata .rodata.*) } > analysis :code
    .data : ALIGN(16) { __data_start__ = .; *(.data .data.*) __data_end__ = .; } > analysis :data
    __file_image_end__ = .;
    /* Reserve aligned space for a FUTURE container footer. No marker is emitted.
     * Without this gap a short image's footer could collide with its own BSS.
     */
    __reserved_footer_start__ = ALIGN(__file_image_end__, 8);
    __reserved_footer_end__ = __reserved_footer_start__ + 16;
    .bss ALIGN(__reserved_footer_end__, 16) (NOLOAD) : {
        __bss_start__ = .;
        *(.bss .bss.* COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > analysis :data
    __runtime_end__ = .;
    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }
    /DISCARD/ : { *(.ARM.exidx* .ARM.extab* .eh_frame* .comment .note*) }
}
ASSERT(_spmp_start == ORIGIN(analysis), "native entry must be first byte")
ASSERT((__bss_start__ & 3) == 0 && (__bss_end__ & 3) == 0, "BSS must be word aligned")
ASSERT(__file_image_end__ <= __reserved_footer_start__ && __reserved_footer_end__ <= __bss_start__, "content/footer reserve/BSS overlap")
ASSERT(__runtime_end__ <= ORIGIN(analysis) + LENGTH(analysis), "research budget exceeded")
ASSERT(__runtime_end__ <= 0x00f00000, "audio workspace overlap")
