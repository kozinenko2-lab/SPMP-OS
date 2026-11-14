#include "spmp_mmi_input.h"
int spmp_mmi_profile_valid(const struct spmp_mmi_profile *p)
{
 if(!p)return 0;
 uint32_t used=0;
 for(unsigned i=0;i<SPMP_MMI_BUTTON_COUNT;i++){
  uint32_t m=p->state_mask[i];if((m&&(m&(m-1u)))||(m&used))return 0;used|=m;
 }
 uint32_t m=p->power_collector_mask;return !m||!(m&(m-1u));
}
uint32_t spmp_mmi_translate_state(const struct spmp_mmi_profile *p,uint32_t state)
{
 if(!p)return 0;
 uint32_t out=0;
 for(unsigned i=0;i<SPMP_MMI_BUTTON_COUNT;i++)if(p->state_mask[i]&&(state&p->state_mask[i]))out|=SPMP_MMI_BUTTON(i);
 return out;
}
int spmp_mmi_input_init(struct spmp_mmi_input *in,const struct spmp_mmi_profile *p,const struct spmp_mmi_io *io)
{
 if(!in||!spmp_mmi_profile_valid(p)||!io||!io->read||!io->collect)return -1;
 in->profile=p;in->io.user=io->user;in->io.read=io->read;in->io.collect=io->collect;
 in->previous=0;in->armed=0;in->power_seen=0;return 0;
}
static uint32_t chords(uint32_t keys)
{
 if(!(keys&SPMP_MMI_BUTTON(SPMP_MMI_SELECT)))return 0;
 return ((keys&SPMP_MMI_BUTTON(SPMP_MMI_VOL_PLUS))?SPMP_MMI_CHORD_PLUS:0u)|
        ((keys&SPMP_MMI_BUTTON(SPMP_MMI_VOL_MINUS))?SPMP_MMI_CHORD_MINUS:0u);
}
int spmp_mmi_input_poll(struct spmp_mmi_input *in,struct spmp_mmi_frame *out)
{
 if(!in||!in->profile||!out||!in->io.read||!in->io.collect)return -1;
 struct spmp_mmi_snapshot before,after;in->io.read(in->io.user,&before);
 uint32_t collected=in->io.collect(in->io.user);in->io.read(in->io.user,&after);
 uint32_t blocked=!!(before.input_gate|after.input_gate|before.gui_lock|after.gui_lock);
 uint32_t physical=spmp_mmi_translate_state(in->profile,after.state);
 uint32_t power=!!(collected&in->profile->power_collector_mask);
 uint32_t pulse=power&&!in->power_seen&&!blocked;in->power_seen=power;
 if(blocked||pulse)in->armed=0;
 else if(!in->armed&&!physical&&!power)in->armed=1;
 uint32_t keys=blocked||!in->armed?0:physical;
 out->buttons=keys;out->pressed=keys&~in->previous;out->released=in->previous&~keys;
 out->power_short=pulse;out->chords=chords(keys)&~chords(in->previous);
 out->blocked=blocked;out->armed=in->armed;out->state=after.state;out->collector=collected;
 uint32_t known=0;for(unsigned i=0;i<SPMP_MMI_BUTTON_COUNT;i++)known|=in->profile->state_mask[i];
 out->unmapped_state=after.state&~known;in->previous=keys;return 0;
}
