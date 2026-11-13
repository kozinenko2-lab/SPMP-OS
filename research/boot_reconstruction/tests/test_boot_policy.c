#include "redboot_boot_policy.h"
#include "oem_layouts.h"
#include <assert.h>
#include <stdio.h>
static void gates(void)
{
    struct rb_policy p=rb_policy_decode(false,false,0x10,0xffffffffu);
    assert(!p.execute_kernel && !p.fast_boot && !p.load_romfs);
    p=rb_policy_decode(true,true,0x10,0xffffffffu);
    assert(!p.execute_kernel && !p.ethernet && !p.disable_usb);
    p=rb_policy_decode(true,false,0,0xffffffffu);
    assert(!p.execute_kernel && !p.load_romfs && !p.linux_boot);
}
int main(void)
{
    gates();
    struct rb_policy p=rb_policy_decode(true,false,0x10,0);
    assert(rb_policy_source(p,false)==RB_NO_AUTOLOAD);
    assert(rb_policy_source(p,true)==RB_EXISTING_SCRIPT);
    p=rb_policy_decode(true,false,0x10,4);
    assert(rb_policy_source(p,false)==RB_NAND_IMAGE);
    p=rb_policy_decode(true,false,0x10,0x40000004u);
    assert(rb_policy_source(p,false)==RB_ROMFS_MMI_BUFFERED);
    assert(rb_generated_go_uses_cache_option(p));
    p=rb_policy_decode(true,false,0x10,0x40000084u);
    assert(rb_policy_source(p,false)==RB_ROMFS_LINUX_BUFFERED);
    assert(!rb_generated_go_uses_cache_option(p));
    p=rb_policy_decode(true,false,0x10,0x60000004u);
    assert(rb_policy_source(p,false)==RB_ROMFS_MMI_FAST);
    /* Odd OEM combination: fast chooses MMI, linux flag still chooses go without -c. */
    p=rb_policy_decode(true,false,0x10,0x60000084u);
    assert(rb_policy_source(p,false)==RB_ROMFS_MMI_FAST);
    assert(!rb_generated_go_uses_cache_option(p));
    p=rb_policy_decode(true,false,0x10,0x20000080u);
    assert(!p.fast_boot && !p.linux_boot); /* Prerequisite bits absent. */
    p=rb_policy_decode(true,false,0x10,3);
    assert(p.ethernet && p.disable_usb);
    assert(rb_custom_module_requested(8));
    assert(!rb_custom_module_requested(4));
    assert(!rb_oem_accepts_nonzero_loader_result(0));
    assert(rb_oem_accepts_nonzero_loader_result(0x280040));
    assert(rb_oem_accepts_nonzero_loader_result(0xffffffffu));
    assert(rb_oem_accepts_nonzero_loader_result(0xfffffffeu));
    assert(rb_oem_accepts_nonzero_loader_result(0xfffffffdu));
    puts("PASS: bootcfg gates, source selection, cache option, customization and OEM error predicate");
    return 0;
}
