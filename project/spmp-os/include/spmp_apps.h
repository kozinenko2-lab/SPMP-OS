#ifndef SPMP_APPS_H
#define SPMP_APPS_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_theme.h"

typedef enum {SPMP_PLANNED=0,SPMP_HOST_PROTOTYPE=1,SPMP_DEVICE_TESTED=2} spmp_app_status;
typedef struct {
    const char *id;
    const char *label;
    const char *description;
    spmp_app_status status;
} spmp_app;
const spmp_app *spmp_get_apps(size_t *count);
void spmp_render_home(const spmp_theme *theme,uint16_t *rgb565);
#endif
