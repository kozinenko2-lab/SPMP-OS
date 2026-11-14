#ifndef SPMP_MMI_INPUT_H
#define SPMP_MMI_INPUT_H
#include <stdint.h>
/* Logical state bits. These are NOT MMI state/event/cooked masks. */
enum spmp_mmi_button_id {SPMP_MMI_UP,SPMP_MMI_DOWN,SPMP_MMI_LEFT,SPMP_MMI_RIGHT,
 SPMP_MMI_A,SPMP_MMI_B,SPMP_MMI_X,SPMP_MMI_Y,SPMP_MMI_L,SPMP_MMI_R,
 SPMP_MMI_START,SPMP_MMI_SELECT,SPMP_MMI_VOL_PLUS,SPMP_MMI_VOL_MINUS,SPMP_MMI_BUTTON_COUNT};
/* Leave bit8 unused: legacy SPMP_UI_POWER is an event, never an L/R state. */
#define SPMP_MMI_BUTTON(id) (1u<<((id)<8u?(id):(id)+1u))
#define SPMP_MMI_CHORD_PLUS 1u
#define SPMP_MMI_CHORD_MINUS 2u
struct spmp_mmi_profile {uint32_t state_mask[SPMP_MMI_BUTTON_COUNT];uint32_t power_collector_mask;};
struct spmp_mmi_snapshot {uint32_t state,event,input_gate,gui_lock;};
struct spmp_mmi_io {
 void *user;
 void (*read)(void *,struct spmp_mmi_snapshot *);
 uint32_t (*collect)(void *); /* Exactly one original collector call per poll. */
};
struct spmp_mmi_frame {
 uint32_t buttons,pressed,released,power_short,chords,blocked,armed;
 uint32_t state,collector,unmapped_state;
};
struct spmp_mmi_input {
 const struct spmp_mmi_profile *profile;
 struct spmp_mmi_io io;
 uint32_t previous,armed,power_seen;
};
int spmp_mmi_profile_valid(const struct spmp_mmi_profile *);
uint32_t spmp_mmi_translate_state(const struct spmp_mmi_profile *,uint32_t);
int spmp_mmi_input_init(struct spmp_mmi_input *,const struct spmp_mmi_profile *,const struct spmp_mmi_io *);
int spmp_mmi_input_poll(struct spmp_mmi_input *,struct spmp_mmi_frame *);
#endif
