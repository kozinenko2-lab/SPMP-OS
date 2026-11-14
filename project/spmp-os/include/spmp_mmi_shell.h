#ifndef SPMP_MMI_SHELL_H
#define SPMP_MMI_SHELL_H
#include "spmp_mmi_input.h"
enum spmp_mmi_view {SPMP_MMI_HOME,SPMP_MMI_INFO,SPMP_MMI_CONTROLS,SPMP_MMI_POWER_MENU,SPMP_MMI_EXIT};
struct spmp_mmi_shell {uint32_t view,selection,return_view,power_selection,blocked,changes;};
void spmp_mmi_shell_init(struct spmp_mmi_shell *);
/* Returns 1 when the visible model changed. POWER opens our Continue/Exit menu. */
int spmp_mmi_shell_update(struct spmp_mmi_shell *,const struct spmp_mmi_frame *);
#endif
