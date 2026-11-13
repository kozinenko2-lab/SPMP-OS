#ifndef SPMP_BUTTONS_H
#define SPMP_BUTTONS_H
#include <stddef.h>
#define SPMP_ACTIONS 12
/* Physical button map is virtual until verified on the target hardware. */
typedef struct {char mapping[SPMP_ACTIONS][32];} spmp_button_map;
void spmp_button_map_default(spmp_button_map *map);
int spmp_button_map_load(spmp_button_map *map,const char *path);
const char *spmp_button_action(size_t index);
#endif
