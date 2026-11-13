#include "spmp_dualboot.h"

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8) |
           ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}

uint32_t spmp_db_crc32(const uint8_t *b, size_t n) {
    uint32_t crc = 0xffffffffu;
    size_t i; unsigned bit;
    if (!b && n) return 0;
    for (i=0; i<n; i++) {
        crc ^= (uint32_t)b[i];
        for (bit=0; bit<8; bit++) {
            uint32_t mask = 0u-(crc&1u);
            crc = (crc>>1) ^ (0xedb88320u & mask);
        }
    }
    return ~crc;
}

int spmp_db_verify_package(const uint8_t *buf, size_t n,
                           uint32_t board_id,
                           const uint8_t **elf, size_t *elf_size) {
    static const uint8_t magic[8]={'S','P','M','P','D','B','0','1'};
    uint32_t bytes, machine;
    size_t i;
    if (elf) *elf=0;
    if (elf_size) *elf_size=0;
    if (!buf || !elf || !elf_size || !board_id || n<SPMP_DB_HEADER_BYTES)
        return 0;
    for(i=0;i<8;i++) if(buf[i]!=magic[i]) return 0;
    if(rd32(buf+8)!=1 || rd32(buf+12)!=SPMP_DB_HEADER_BYTES) return 0;
    bytes=rd32(buf+16);
    if(bytes<52u || bytes>SPMP_DB_MAX_IMAGE_BYTES ||
       (size_t)bytes != n - SPMP_DB_HEADER_BYTES) return 0;
    if(rd32(buf+24)!=board_id || rd32(buf+28)!=1) return 0;
    for(i=32;i<48;i++) if(buf[i]!=0) return 0;
    const uint8_t *img=buf+SPMP_DB_HEADER_BYTES;
    if(img[0]!=0x7f || img[1]!='E' || img[2]!='L' || img[3]!='F' ||
       img[4]!=1 || img[5]!=1 || img[6]!=1) return 0;
    /* ELF32 little endian, ARM e_machine = 40. The complete ELF validation
     * (headers, segment bounds, relocations) is delegated to Stage34 gate. */
    machine=(uint32_t)img[18]|((uint32_t)img[19]<<8);
    if(machine!=40) return 0;
    if(spmp_db_crc32(img,bytes)!=rd32(buf+20)) return 0;
    *elf=img; *elf_size=bytes;
    return 1;
}

static void event(const spmp_db_ops *ops, enum spmp_db_event e) {
    if(ops->on_event) ops->on_event(ops->context,e);
}

int spmp_db_boot(const spmp_db_ops *ops, uint8_t *staging, size_t capacity) {
    const uint8_t *elf=0;
    size_t actual=0, elf_size=0;
    uint32_t entry=0;
    int sd_attempted=0;
    /* Missing or incomplete platform backend MUST NOT start an SD image. */
    if(!ops || !ops->boot_factory) return -1;
    if(!staging || capacity<SPMP_DB_HEADER_BYTES+52 ||
       capacity>SPMP_DB_MAX_IMAGE_BYTES+SPMP_DB_HEADER_BYTES ||
       !ops->verified_board_id ||
       (ops->force_factory && ops->force_factory(ops->context)!=0)) {
        event(ops,SPMP_DB_EV_FORCE_FACTORY);
        goto factory;
    }
    if(!ops->read_package || !ops->restore_factory_environment ||
       !ops->preflight_elf || !ops->stage_elf || !ops->jump_sd) {
        event(ops,SPMP_DB_EV_NO_CARD_OR_PACKAGE);
        goto factory;
    }
    sd_attempted=1;
    if(ops->read_package(ops->context,SPMP_DB_DEFAULT_PATH,staging,
                         capacity,&actual)!=1 || actual>capacity) {
        event(ops,SPMP_DB_EV_NO_CARD_OR_PACKAGE);
        goto factory;
    }
    if(!spmp_db_verify_package(staging,actual,ops->verified_board_id,
                               &elf,&elf_size)) {
        event(ops,SPMP_DB_EV_INVALID_PACKAGE);
        goto factory;
    }
    if(ops->preflight_elf(ops->context,elf,elf_size,&entry)!=1 || !entry) {
        event(ops,SPMP_DB_EV_RAM_REJECTED);
        goto factory;
    }
    /* Stage verifies layout, proof of ownership, data and instruction caches.
     * Entry returned by Stage34 is the only permitted launch address. */
    entry=0;
    if(ops->stage_elf(ops->context,elf,elf_size,&entry)!=1 || !entry) {
        event(ops,SPMP_DB_EV_STAGE_FAILED);
        goto factory;
    }
    event(ops,SPMP_DB_EV_SD_HANDOFF);
    ops->jump_sd(ops->context,entry);
    /* A returning experimental kernel is treated as a failed handoff. This
     * cannot help with a later kernel hang; hardware watchdog is required. */
    event(ops,SPMP_DB_EV_HANDOFF_RETURNED);
factory:
    if(sd_attempted && ops->restore_factory_environment(ops->context)!=1)
        event(ops,SPMP_DB_EV_FACTORY_RESTORE_FAILED);
    event(ops,SPMP_DB_EV_FACTORY_HANDOFF);
    ops->boot_factory(ops->context);
    return -2; /* also abnormal: factory boot unexpectedly returned */
}
