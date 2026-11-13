/* Minimal SD logging probe for *unmodified* OEM MMI, native-game ABI.
 * Build against https://github.com/uli/libspmp8k (not bundled).
 * This is an optional separate native application, NOT a patched MMI.
 * Written to the explicit SD mount only; no fallback to /Rom or cwd.
 * On unknown mount, fails closed and exits without storage writes.
 */
#include <libgame.h>
#include <stddef.h>

#ifndef SPMP_PROBE_SD_MOUNT
#define SPMP_PROBE_SD_MOUNT "/fat20a0/hda1/"
#endif
#define SPMP_PROBE_FILE SPMP_PROBE_SD_MOUNT "MMIPROB.LOG"
#define SPMP_PROBE_MARKER SPMP_PROBE_SD_MOUNT "SPMPOS.ID"
static const char marker[] = "SPMP-OS-SD-LOGGING-PROBE-V1\n";
static const char message[] =
    "SPMP-OS MMI filesystem probe v1\n"
    "This runs AFTER OEM MMI boot from a native game.\n"
    "It does NOT record the early boot and does NOT change RedBoot.\n";
int main(void) {
    int fd;
    int n;
    char proof[sizeof(marker)-1u];
    unsigned int i;
    libgame_init();
    if (!_ecos_open || !_ecos_read || !_ecos_write || !_ecos_close) return 0;
    /* Refuse writes unless the user placed an exact marker on the known SD.
     * This does not independently identify hardware; verify SD mount first. */
    fd=_ecos_open(SPMP_PROBE_MARKER, _ECOS_O_RDONLY, 0);
    if(fd<0)return 0;
    n=_ecos_read(fd, proof, (unsigned int)sizeof(proof));
    (void)_ecos_close(fd);
    if(n != (int)sizeof(proof))return 0;
    for(i=0;i<sizeof(proof);i++)if(proof[i]!=marker[i])return 0;
    fd=_ecos_open(SPMP_PROBE_FILE, _ECOS_O_WRONLY | _ECOS_O_CREAT | _ECOS_O_APPEND, 0);
    if(fd<0)return 0;
    n=_ecos_write(fd, message, (unsigned int)(sizeof(message)-1u));
    (void)n;
    (void)_ecos_close(fd);
    return 0;
}
