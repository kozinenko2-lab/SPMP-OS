#ifndef SPMP_RAW_INPUT_H
#define SPMP_RAW_INPUT_H
#include "input_test.h"
struct raw_snapshot { uint32_t state,event; };
typedef void (*raw_snapshot_fn)(struct raw_snapshot *);
struct raw_state {
 struct input_state session;
 struct raw_snapshot before,after;
 uint32_t samples,armed;
};
struct raw_config { uint32_t duration_units,max_polls,redraw_units; };
_Static_assert(sizeof(struct raw_state)==196,"Raw trace layout");
_Static_assert(offsetof(struct raw_state,before)==172,"Raw snapshot trace offset");
extern const char raw_log_path[];
const char *raw_prompt(uint32_t);
uint32_t raw_input_run(struct raw_state *,const struct input_services *,raw_snapshot_fn,const struct raw_config *);
void spmp_raw_snapshot(struct raw_snapshot *);
#endif
