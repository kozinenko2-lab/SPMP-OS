#include "probe.h"
static enum probe_result finish(struct probe_state *s,enum probe_result result)
{
    if(s->active&&!s->finished){
        if(s->color_saved&&s->ops->set_color(s->user,&s->original_color)!=0)result=PROBE_ERROR;
        s->ops->finish(s->user);
    }
    s->active=0;s->finished=1;s->result=result;
    return result;
}
enum probe_result probe_start(struct probe_state *s,const struct probe_ops *ops,void *user,const struct probe_config *c)
{
    if(!s)return PROBE_ERROR;
    s->ops=ops;s->user=user;s->config=c;s->steps=0;s->last_keys=0;
    s->x=152;s->y=112;s->active=0;s->color_saved=0;s->finished=0;s->result=PROBE_ERROR;
    if(!ops||!c||!c->max_steps||c->max_steps>10000||!c->quit||!ops->begin||!ops->dimensions||
       !ops->get_color||!ops->set_color||!ops->fill||!ops->present||!ops->keys||!ops->finish){s->finished=1;return PROBE_ERROR;}
    /* begin must clean up its own partial failure; finish is valid after success. */
    if(ops->begin(user)!=0){s->finished=1;return PROBE_ERROR;}
    s->active=1;
    uint16_t width=0,height=0;
    if(ops->dimensions(user,&width,&height)!=0||width!=320||height!=240)return finish(s,PROBE_ERROR);
    if(ops->get_color(user,&s->original_color)!=0)return finish(s,PROBE_ERROR);
    s->color_saved=1;s->result=PROBE_RUNNING;
    return PROBE_RUNNING;
}
static int draw(struct probe_state *s)
{
    const struct mmi_rgb888 colors[4]={{255,0,0},{0,255,0},{0,0,255},{255,255,255}};
    for(unsigned i=0;i<4;i++){
        struct probe_rect r;
        if(i==3){r.x=s->x;r.y=s->y;r.width=16;r.height=16;}
        else{r.x=(int16_t)(i*106);r.y=0;r.width=(uint16_t)(i==2?108:106);r.height=240;}
        if(s->ops->set_color(s->user,&colors[i])!=0||s->ops->fill(s->user,&r)!=0)return 0;
    }
    return s->ops->present(s->user)==0;
}
enum probe_result probe_step(struct probe_state *s)
{
    if(!s)return PROBE_ERROR;
    if(s->finished)return s->result;
    if(!s->active)return finish(s,PROBE_ERROR);
    struct mmi_key_output32 input={0,0};
    if(s->ops->keys(s->user,&input)!=1)return finish(s,PROBE_ERROR);
    s->last_keys=input.keys;
    if(input.keys&s->config->quit)return finish(s,PROBE_RETURN);
    if(input.keys&s->config->left)s->x-=6;
    if(input.keys&s->config->right)s->x+=6;
    if(input.keys&s->config->up)s->y-=6;
    if(input.keys&s->config->down)s->y+=6;
    if(s->x<0)s->x=0;
    if(s->x>304)s->x=304;
    if(s->y<0)s->y=0;
    if(s->y>224)s->y=224;
    if(!draw(s))return finish(s,PROBE_ERROR);
    s->steps++;
    return s->steps>=s->config->max_steps?finish(s,PROBE_RETURN):PROBE_RUNNING;
}
