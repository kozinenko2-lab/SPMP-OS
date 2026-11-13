/* Reconstruction from original ARM, corroborated by user GZF pseudocode.
 * RedBoot variants 3/5; raw code addresses are evidence, never host callbacks.
 * Pure functions only: no MMIO, target code execution, memory loading or flashing.
 */
#include "redboot_boot_policy.h"
struct rb_policy rb_policy_decode(bool got_config, bool isp_bit,
                                  uint32_t header_word_04, uint32_t boot_word_18)
{
    struct rb_policy p = {0};
    /* 0x45DF4..0x45E24 clear six globals before checking configuration.
     * 0x45E38, 0x45E50, 0x45E68 gate all following policy changes.
     */
    if (!got_config || isp_bit || !(header_word_04 & 0x10u)) return p;
    p.ethernet       = (boot_word_18 & 0x00000001u) != 0; /* 0x45E7C */
    p.disable_usb    = (boot_word_18 & 0x00000002u) != 0; /* 0x45EB0 */
    p.execute_kernel = (boot_word_18 & 0x00000004u) != 0; /* 0x45ED0 */
    p.load_romfs     = (boot_word_18 & 0x40000000u) != 0; /* 0x45EF0 */
    p.linux_boot     = p.load_romfs && (boot_word_18 & 0x80u); /* 0x45F10 */
    p.fast_boot      = p.execute_kernel && (boot_word_18 & 0x20000000u); /* 0x45F2C */
    return p;
}
enum rb_boot_source rb_policy_source(struct rb_policy p, bool existing_script)
{
    /* Main 0x46AD4..0x46B6C: preexisting script precedes EXEC gate.
     * Fast ROMFS path always uses MMI, even when linux_boot is also set.
     */
    if (existing_script) return RB_EXISTING_SCRIPT;
    if (!p.execute_kernel) return RB_NO_AUTOLOAD;
    if (!p.load_romfs) return RB_NAND_IMAGE;
    if (p.fast_boot) return RB_ROMFS_MMI_FAST;
    return p.linux_boot ? RB_ROMFS_LINUX_BUFFERED : RB_ROMFS_MMI_BUFFERED;
}
bool rb_custom_module_requested(uint32_t boot_word_18)
{
    /* 0x33EF4: helper independently reads bit 3, before apply_bootcfg. */
    return (boot_word_18 & 8u) != 0;
}
bool rb_generated_go_uses_cache_option(struct rb_policy p)
{
    /* 0x46B84..0x46BB4 chooses "go 0x%x\n" vs "go -c 0x%x\n". */
    return !p.linux_boot;
}
bool rb_oem_accepts_nonzero_loader_result(uint32_t result)
{
    /* 0x46B70/0x4681C only compare with zero. This reproduces observed OEM
     * control flow, NOT a safe entry validator: negative errors also pass.
     */
    return result != 0;
}
