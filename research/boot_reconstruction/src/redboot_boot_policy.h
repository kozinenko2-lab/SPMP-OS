#ifndef SPMP_REDBOOT_BOOT_POLICY_H
#define SPMP_REDBOOT_BOOT_POLICY_H
#include <stdbool.h>
#include <stdint.h>
/* Host-side reconstruction of variant 3/5 policy, NOT a device ABI. */
struct rb_policy {
    bool ethernet, disable_usb, execute_kernel, load_romfs, linux_boot, fast_boot;
};
enum rb_boot_source {
    RB_NO_AUTOLOAD, RB_EXISTING_SCRIPT, RB_NAND_IMAGE,
    RB_ROMFS_MMI_BUFFERED, RB_ROMFS_LINUX_BUFFERED, RB_ROMFS_MMI_FAST
};
struct rb_policy rb_policy_decode(bool got_config, bool isp_bit,
                                  uint32_t header_word_04, uint32_t boot_word_18);
enum rb_boot_source rb_policy_source(struct rb_policy p, bool existing_script);
bool rb_custom_module_requested(uint32_t boot_word_18);
bool rb_generated_go_uses_cache_option(struct rb_policy p);
bool rb_oem_accepts_nonzero_loader_result(uint32_t result);
#endif
