#ifndef SPMP_OEM_LAYOUTS_H
#define SPMP_OEM_LAYOUTS_H
#include <stdint.h>
#include <stddef.h>
/* Image-specific recovered 32-bit layouts, NOT callable host/native bindings.
 * All *_address fields store target numeric addresses, not C function pointers.
 * MMI SHA 5529b173... and RedBoot variant 3 SHA 1723e3ce... only.
 */
struct rb_module_descriptor32 {
    uint32_t source;                 /* file descriptor or memory base */
    uint32_t backend;                /* 0=file, 3=memory */
    uint32_t cursor;
    uint32_t memory_length;
    uint32_t read_address;
    uint32_t seek_address;
    uint32_t close_address;
    uint32_t elf_header_address;
    uint32_t section_headers_address;
    uint32_t symbol_section_index;
    uint32_t string_section_index;
    uint32_t section_cache_address;
};
struct mmi_native_api32 {
    uint32_t slot_address[85];       /* initialized by 0x002D32B0 */
};
struct mmi_gui_message_prefix32 {
    uint32_t unknown_00;
    uint32_t unknown_04;
    uint32_t window;
    uint32_t type;
    uint32_t argument_1;
    uint32_t argument_2;
    /* Full message object size is not established. */
};
_Static_assert(sizeof(struct rb_module_descriptor32)==0x30,"OEM module descriptor size");
_Static_assert(offsetof(struct rb_module_descriptor32,read_address)==0x10,"OEM read slot");
_Static_assert(offsetof(struct rb_module_descriptor32,section_cache_address)==0x2c,"OEM section cache slot");
_Static_assert(sizeof(struct mmi_native_api32)==0x154,"OEM native slot array size");
_Static_assert(offsetof(struct mmi_gui_message_prefix32,type)==0xc,"OEM message type offset");
#endif
