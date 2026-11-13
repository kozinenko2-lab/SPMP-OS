#ifndef SPMP_UI_STAGE10_BRIDGE_H
#define SPMP_UI_STAGE10_BRIDGE_H
#include "spmp_ui_theme_v7.h"
#include "defos_hal.h"
/* Safe add-on; existing Stage10 HAL is unchanged. Timing and SD paths supplied by firmware. */
int spmp_ui_stage10_present(defos_hal *hal,uint16_t *framebuffer,unsigned pitch_pixels,
  spmp_ui_read_at background_reader,void*background_ctx,
  uint8_t *scratch,size_t scratch_len,
  const spmp_ui_style *style,const uint8_t *font,size_t font_len,
  const uint8_t *icons,size_t icons_len,
  const spmp_ui_item *items,unsigned item_count,
  spmp_ui_status *status,spmp_ui_state *state);
/* Edge-triggered input translation using existing Stage10 HAL buttons.
   Stage10 currently exposes no X/Y keys: SELECT toggles list/grid safely. */
int spmp_ui_stage10_poll_input(defos_hal *hal,spmp_ui_state *ui,
  uint32_t *previous_buttons,spmp_ui_sfx *sound);
/* Only when no music is active: use mixer v6 instead to mix UI with music. */
int spmp_ui_stage10_click(defos_hal*hal,spmp_ui_sfx type,int16_t *stereo_scratch,size_t stereo_frames);
#endif
