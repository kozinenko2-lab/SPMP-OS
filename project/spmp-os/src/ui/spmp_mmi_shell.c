#include "spmp_mmi_shell.h"
void spmp_mmi_shell_init(struct spmp_mmi_shell *s)
{s->view=SPMP_MMI_HOME;s->selection=0;s->return_view=SPMP_MMI_HOME;s->power_selection=0;s->blocked=0;s->changes=0;}
int spmp_mmi_shell_update(struct spmp_mmi_shell *s,const struct spmp_mmi_frame *f)
{
 if(!s||!f||s->view==SPMP_MMI_EXIT)return 0;
 int changed=s->blocked!=f->blocked;s->blocked=f->blocked;
 if(!f->blocked){
  if(f->power_short){
   if(s->view==SPMP_MMI_POWER_MENU)s->view=s->return_view;
   else {s->return_view=s->view;s->view=SPMP_MMI_POWER_MENU;s->power_selection=0;}
   changed=1;
  }else if(f->pressed&SPMP_MMI_BUTTON(SPMP_MMI_B)){
   if(s->view==SPMP_MMI_POWER_MENU)s->view=s->return_view;
   else s->view=s->view==SPMP_MMI_HOME?SPMP_MMI_EXIT:SPMP_MMI_HOME;
   changed=1;
  }else if(s->view==SPMP_MMI_HOME||s->view==SPMP_MMI_POWER_MENU){
   uint32_t *selection=s->view==SPMP_MMI_HOME?&s->selection:&s->power_selection;
   uint32_t count=s->view==SPMP_MMI_HOME?3u:2u;
   if(f->pressed&SPMP_MMI_BUTTON(SPMP_MMI_UP)){*selection=*selection?*selection-1u:count-1u;changed=1;}
   else if(f->pressed&SPMP_MMI_BUTTON(SPMP_MMI_DOWN)){*selection=*selection+1u==count?0:*selection+1u;changed=1;}
   else if(f->pressed&SPMP_MMI_BUTTON(SPMP_MMI_A)){
    if(s->view==SPMP_MMI_POWER_MENU)s->view=s->power_selection?SPMP_MMI_EXIT:s->return_view;
    else s->view=s->selection==0?SPMP_MMI_INFO:s->selection==1?SPMP_MMI_CONTROLS:SPMP_MMI_EXIT;
    changed=1;
   }
  }
 }
 if(changed)s->changes++;
 return changed;
}
