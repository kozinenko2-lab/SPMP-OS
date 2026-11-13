#ifndef SPMP_MMI_NATIVE_RESOURCES_ABI_H
#define SPMP_MMI_NATIVE_RESOURCES_ABI_H
#include <stdint.h>
#include <stddef.h>
/* Exact-build declarations for RAP SHA-256 5529b173..., not live bindings.
 * See STAGE5_REPORT_RU.md, especially ownership, parser and exit limitations.
 */
struct mmi_resource_entry32 {
    char name[16];                   /* NUL-terminated; sentinel name "TAEND" */
    uint32_t data_address;           /* borrowed resource blob, +0x10 */
};
struct mmi_resource_buffer32 {
    uint32_t data_address;
    uint32_t byte_count;             /* not used as a bound by every parser */
};
struct mmi_game_pool_info32 {
    uint32_t managed_extent;
    uint32_t unknown_04[6];
    uint32_t used_extent;            /* +0x1c, includes allocation overhead */
    uint32_t free_extent;            /* +0x20 */
    uint32_t unknown_24;
    uint32_t max_request;            /* +0x28, largest free block minus header */
};
/* +0xd4: r0 is unused in this build, r1 supplies the table; no defined status. */
typedef void (*mmi_resource_init_fn)(uint32_t unused_r0,const struct mmi_resource_entry32 *table);
/* +0xd8: kind 0/1/2/4/8; classification alone does not prove a resource was found. */
typedef uint32_t (*mmi_resource_get_fn)(const char *name,struct mmi_resource_buffer32 *output);
/* +0xdc: channels bits 1=WAV, 2=MIDI; repeats is truncated to 8 bits.
 * Repeat zero becomes 0x0fffffff. Returns 0, 1 or -1 on observed paths.
 */
typedef int32_t (*mmi_resource_play_fn)(uint8_t channels,uint8_t repeats,const struct mmi_resource_buffer32 *resource);
typedef void (*mmi_resource_pause_fn)(uint32_t channels);          /* +0xe0 */
typedef void (*mmi_resource_resume_fn)(uint32_t channels);         /* +0xe4 */
typedef void (*mmi_resource_stop_fn)(uint8_t channels);            /* +0xe8 */
/* +0x130: nonnegative mode may suspend/kill the Game thread. Ordinary BIN return
 * instead leads to OEM cleanup(-0x80000000). Not an interchangeable exit().
 */
typedef uint32_t (*mmi_game_exit_control_fn)(int32_t mode);
_Static_assert(sizeof(struct mmi_resource_entry32)==20,"OEM resource entry stride");
_Static_assert(offsetof(struct mmi_resource_entry32,data_address)==16,"OEM resource data offset");
_Static_assert(sizeof(struct mmi_resource_buffer32)==8,"OEM resource buffer descriptor");
_Static_assert(sizeof(struct mmi_game_pool_info32)==44,"OEM game pool info extent");
_Static_assert(offsetof(struct mmi_game_pool_info32,max_request)==40,"OEM maximum allocation field");
#endif
