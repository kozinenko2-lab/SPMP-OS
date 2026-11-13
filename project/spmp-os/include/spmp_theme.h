#ifndef SPMP_THEME_H
#define SPMP_THEME_H
#include <stdint.h>

typedef struct {
    uint16_t background, panel, accent, text, muted, selection, border;
    char title[32];
} spmp_theme;

void spmp_theme_default(spmp_theme *theme);
/* INI subset, file-backed host import; returns -1 for malformed/unsafe themes */
int spmp_theme_load(spmp_theme *theme, const char *path);
#endif
