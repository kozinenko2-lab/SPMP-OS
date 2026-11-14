#include <assert.h>
#include <stdio.h>
#include "spmp_mmi_input.h"
#include "spmp_mmi_user01_profile.h"
#include "spmp_mmi_shell.h"
static struct spmp_mmi_snapshot mock;
static uint32_t collected,calls;
static void read_mock(void *u,struct spmp_mmi_snapshot *out){(void)u;*out=mock;}
static uint32_t collect_mock(void *u){(void)u;calls++;return collected;}
static struct spmp_mmi_frame frame(struct spmp_mmi_input *in,uint32_t state,uint32_t event,uint32_t gate,uint32_t gui)
{
 mock=(struct spmp_mmi_snapshot){state,event,gate,gui};collected=gate?0:event;
 struct spmp_mmi_frame f;uint32_t before=calls;assert(!spmp_mmi_input_poll(in,&f));assert(calls==before+1);return f;
}
int main(void)
{
 const struct spmp_mmi_profile *p=&spmp_mmi_user01_profile;assert(spmp_mmi_profile_valid(p));
 for(unsigned i=0;i<SPMP_MMI_BUTTON_COUNT;i++)assert(spmp_mmi_translate_state(p,p->state_mask[i])==SPMP_MMI_BUTTON(i));
 assert(!(spmp_mmi_translate_state(p,0x3fff)&256u)); /* No L/POWER alias. */
 struct spmp_mmi_profile bad=*p;bad.state_mask[1]=bad.state_mask[0];assert(!spmp_mmi_profile_valid(&bad));
 struct spmp_mmi_io io={0,read_mock,collect_mock};struct spmp_mmi_input in;assert(!spmp_mmi_input_init(&in,p,&io));
 struct spmp_mmi_frame f=frame(&in,0x20,0,0,0);assert(!f.buttons&&!f.armed); /* Held launcher ignored. */
 f=frame(&in,0,0,0,0);assert(f.armed);
 f=frame(&in,0x10,0,0,0);assert(f.pressed==SPMP_MMI_BUTTON(SPMP_MMI_X));
 f=frame(&in,0x10,0,0,0);assert(!f.pressed);
 f=frame(&in,0,0,0,0);assert(f.released==SPMP_MMI_BUTTON(SPMP_MMI_X));
 f=frame(&in,0x1000,0x800,0,0);assert(f.buttons==SPMP_MMI_BUTTON(SPMP_MMI_START)&&!f.power_short);
 f=frame(&in,0x1000,0,0,0);assert(f.buttons==SPMP_MMI_BUTTON(SPMP_MMI_START)&&!f.released); /* Persistent START. */
 f=frame(&in,0,0,0,0);
 f=frame(&in,0,0x1000,0,0);assert(f.power_short&&!f.buttons&&!f.armed);
 for(unsigned i=0;i<3;i++){f=frame(&in,0,0x1000,0,0);assert(!f.power_short);}
 f=frame(&in,0,0,0,0);assert(f.armed);
 f=frame(&in,0x20,0,1,0);assert(f.blocked&&!f.buttons&&!f.armed);
 f=frame(&in,0x20,0,0,0);assert(!f.buttons&&!f.armed); /* Wait for release after unlock. */
 f=frame(&in,0,0,0,0);assert(f.armed);
 f=frame(&in,0x20,0,0,1);assert(f.blocked&&!f.buttons);
 f=frame(&in,0,0,0,0);
 f=frame(&in,0x2800,0,0,0);assert(f.chords==SPMP_MMI_CHORD_PLUS);
 f=frame(&in,0x2800,0,0,0);assert(!f.chords);
 f=frame(&in,0x2000,0,0,0);
 f=frame(&in,0x2200,0,0,0);assert(f.chords==SPMP_MMI_CHORD_MINUS);
 f=frame(&in,0,0,0,0);
 f=frame(&in,0x4000,0,0,0);assert(!f.buttons&&f.unmapped_state==0x4000); /* HOLD not guessed. */
 struct spmp_mmi_shell s;spmp_mmi_shell_init(&s);
 f=frame(&in,0,0x1000,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.view==SPMP_MMI_POWER_MENU);
 f=frame(&in,0,0,0,0);spmp_mmi_shell_update(&s,&f);
 f=frame(&in,0x20,0,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.view==SPMP_MMI_HOME);
 f=frame(&in,0,0,0,0);spmp_mmi_shell_update(&s,&f);
 f=frame(&in,0x4,0,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.selection==1);
 f=frame(&in,0x4,0,0,0);assert(!spmp_mmi_shell_update(&s,&f)&&s.selection==1); /* No held repeat. */
 f=frame(&in,0,0,0,0);spmp_mmi_shell_update(&s,&f);
 f=frame(&in,0x20,0,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.view==SPMP_MMI_CONTROLS);
 f=frame(&in,0,0,0,0);spmp_mmi_shell_update(&s,&f);
 f=frame(&in,0x80,0,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.view==SPMP_MMI_HOME);
 f=frame(&in,0,0,0,0);spmp_mmi_shell_update(&s,&f);
 f=frame(&in,0x80,0,0,0);assert(spmp_mmi_shell_update(&s,&f)&&s.view==SPMP_MMI_EXIT);
 puts("PASS: MMI profile, 14 states, namespaces, edges, START hold, POWER pulse, locks/rearm, chords and shell navigation");
 return 0;
}
