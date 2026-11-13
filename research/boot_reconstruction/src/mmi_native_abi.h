#ifndef SPMP_MMI_NATIVE_ABI_H
#define SPMP_MMI_NATIVE_ABI_H
#include <stddef.h>
#include <stdint.h>
/* Reconstructed declarations for 8000_MMI.RAP SHA-256 5529b173... only.
 * Names compared with uli/libspmp8k; signatures checked against this image.
 * See STAGE4_REPORT_RU.md and evidence/native_abi_reference.json.
 * No addresses are bound here. No complete HAL or on-device validation.
 */
struct mmi_rgb888 { uint8_t r, g, b; };
struct mmi_rect16 { uint16_t x, y, width, height; };
struct mmi_key_output32 {
    uint32_t untouched_00;           /* +0 is NOT initialized by getKeyInput */
    uint32_t keys;                   /* +4; meaning depends on native kind */
};
struct mmi_image_descriptor_prefix32 {
    uint32_t pixels_address;         /* +00 borrowed, caller-owned */
    uint16_t width, height;          /* +04/+06 */
    uint32_t pixel_type;             /* +08: 1/2 indexed formats */
    uint8_t shared_palette_image;    /* +0c: 0 means own palette */
    uint8_t unknown_0d[3];
    uint32_t palette_address;        /* +10 borrowed */
    uint16_t palette_entries;        /* +14: OEM may write zero here */
    uint8_t flags_16, flags_17;      /* +16/+17: selected bits copied; full descriptor size unknown */
};
/* These typedefs describe observed register/return widths; no casts from the
 * raw numeric slot array and no automatic binding to firmware addresses.
 * Function pointers and normal C pointers require a verified ARM32 runtime.
 */
typedef uint32_t (*mmi_paint_fn)(void);                            /* +010 */
typedef uint32_t (*mmi_load_image_fn)(struct mmi_image_descriptor_prefix32 *,uint8_t *out_id); /* +014 */
typedef uint32_t (*mmi_free_image_fn)(uint8_t id);                 /* +018 */
typedef uint32_t (*mmi_set_rop_fn)(uint8_t value);                  /* +03c */
typedef uint32_t (*mmi_get_rop_fn)(uint8_t *value);                 /* +040 */
typedef uint32_t (*mmi_set_fg_fn)(const struct mmi_rgb888 *);       /* +044 */
typedef uint32_t (*mmi_get_fg_fn)(struct mmi_rgb888 *);             /* +048 */
typedef uint32_t (*mmi_get_display_rect_fn)(struct mmi_rect16 *);   /* +058 */
typedef uint32_t (*mmi_set_framebuffer_fn)(uint16_t width,uint16_t height); /* +090: gates only; no resize */
typedef uint32_t (*mmi_get_dimensions_fn)(uint16_t *width,uint16_t *height); /* +094 */
typedef uint32_t (*mmi_query_image_fn)(uint8_t id,uint8_t field,uint16_t *value); /* +0a4, THREE arguments */
typedef uint32_t (*mmi_get_keys_fn)(struct mmi_key_output32 *);     /* +100: always returns 1 */
typedef uint32_t (*mmi_fs_open_fn)(const char *name,uint32_t flags,int32_t *fd); /* +13c */
typedef uint32_t (*mmi_fs_read_fn)(int32_t fd,void *buf,uint32_t count,int32_t *done); /* +140 */
typedef uint32_t (*mmi_fs_write_fn)(int32_t fd,const void *buf,uint32_t count,int32_t *done); /* +144 */
typedef uint32_t (*mmi_fs_close_fn)(int32_t fd);                    /* +148 */
typedef uint32_t (*mmi_fs_seek_status32_fn)(int32_t fd,int32_t offset,uint32_t whence); /* +14c */
/* read/write/close/seek: status 0 or 2; read/write done may be -1.
 * Argument pointer validation is not supplied by these OEM wrappers.
 * General heap and game-pool allocators are NOT slots in the 85-entry API.
 */
_Static_assert(sizeof(struct mmi_rgb888)==3,"OEM RGB byte layout");
_Static_assert(sizeof(struct mmi_rect16)==8,"OEM rectangle layout");
_Static_assert(offsetof(struct mmi_key_output32,keys)==4,"OEM key output offset");
_Static_assert(sizeof(struct mmi_image_descriptor_prefix32)==24,"Observed image descriptor prefix extent");
_Static_assert(offsetof(struct mmi_image_descriptor_prefix32,shared_palette_image)==0xc,"OEM shared palette id");
_Static_assert(offsetof(struct mmi_image_descriptor_prefix32,palette_entries)==0x14,"OEM palette count");
#endif
