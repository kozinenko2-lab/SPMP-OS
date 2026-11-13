/* SPDX-License-Identifier: MIT - action UI draws through shared system font. */
#include "spmp_fm02.h"
static void rect(uint16_t *p,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color){
    unsigned i,j;if(x>=320||y>=240)return;
    if(w>320-x)w=320-x;
    if(h>240-y)h=240-y;
    for(j=y;j<y+h;j++)for(i=x;i<x+w;i++)p[j*pitch+i]=color;
}
static void text(const fm01_font *f,uint16_t *p,unsigned pitch,unsigned x,unsigned y,
                 const char *s,uint16_t c,unsigned maxw){
    if(f && f->draw)f->draw(f->ctx,p,pitch,x,y,s,c,maxw,11);
}
static unsigned append_num(char *out,unsigned pos,unsigned cap,unsigned value){
    char tmp[12];unsigned j=0,i;
    do{tmp[j++]=(char)('0'+value%10u);value/=10u;}while(value && j<11);
    for(i=j;i>0 && pos+1<cap;i--)out[pos++]=tmp[i-1];
    out[pos]=0;return pos;
}
static unsigned progress_width(uint64_t done,uint64_t total){
    uint64_t remainder;unsigned frac=0,i;
    if(!total || done>=total)return 310;
    while(total>0x7fffffffu){done>>=1;total>>=1;}
    if(!total)return 310;
    remainder=done;
    for(i=0;i<8;i++){
        remainder<<=1;frac<<=1;
        if(remainder>=total){remainder-=total;frac|=1u;}
    }
    return (frac*310u)>>8;
}
void fm02_render_status(const fm02_model *m,uint16_t *pixels,unsigned pitch,
    const fm01_palette *p,const fm01_font *font){
    char b[48];unsigned j,i;
    static const char *ru[]={"Копировать","Переместить","Переименовать","Удалить"};
    static const char *en[]={"Copy","Move","Rename","Delete"};
    const char **labels;
    if(!m||!m->browser||!pixels||pitch<320||!p)return;
    labels=m->browser->locale?en:ru;
    /* Selection marks overlaid on FM01 rows without embedded labels. */
    for(i=0;i<2;i++){
        const fm01_panel *panel=&m->browser->panels[i];
        for(j=0;j<panel->loaded && j<FM01_PAGE_ROWS;j++){
            const fm01_entry *e=&panel->entries[j];
            if(fm02_is_marked(m,panel->path,e->name,i)){
                unsigned x=i?162:4;unsigned y=44+j*17;
                rect(pixels,pitch,x,y+5,4,5,p->active_border);
            }
        }
    }
    if(m->state==FM02_MENU || m->state==FM02_CONFIRM_DELETE || m->state==FM02_RENAME_INPUT_REQUIRED){
        rect(pixels,pitch,60,58,200,124,p->panel);rect(pixels,pitch,60,58,200,1,p->active_border);
        rect(pixels,pitch,60,181,200,1,p->active_border);
        if(m->state==FM02_MENU){
            text(font,pixels,pitch,70,65,m->browser->locale?"File actions":"Действия с файлами",p->text,180);
            for(i=0;i<4;i++){
                if(m->menu_action==i)rect(pixels,pitch,68,84+i*19,183,17,p->highlight);
                text(font,pixels,pitch,77,85+i*19,labels[i],p->text,171);
            }
        }else if(m->state==FM02_CONFIRM_DELETE){
            text(font,pixels,pitch,70,79,m->browser->locale?"Delete selected items?":"Удалить выбранное?",p->text,181);
            text(font,pixels,pitch,70,113,m->browser->locale?"A: yes   B: cancel":"A: да   B: отмена",p->secondary,177);
        }else{
            text(font,pixels,pitch,70,75,m->browser->locale?"Rename needs keyboard":"Нужна клавиатура",p->text,180);
            text(font,pixels,pitch,70,109,"FM-04",p->secondary,164);
        }
    }else if(m->state==FM02_BUSY || m->state==FM02_DONE || m->state==FM02_FAILED || m->state==FM02_CANCELLED){
        rect(pixels,pitch,0,207,320,33,p->footer);
        b[0]=0;i=0;
        if(m->state==FM02_DONE){text(font,pixels,pitch,5,210,m->browser->locale?"Completed":"Завершено",p->text,92);}
        else if(m->state==FM02_FAILED){text(font,pixels,pitch,5,210,m->browser->locale?"Error":"Ошибка",p->text,75);}
        else if(m->state==FM02_CANCELLED){text(font,pixels,pitch,5,210,m->browser->locale?"Cancelled":"Отменено",p->text,85);}
        else text(font,pixels,pitch,5,210,m->browser->locale?"Working":"Выполняется",p->text,105);
        i=append_num(b,i,sizeof(b),m->processed);
        if(i+1<sizeof(b))b[i++]='/';
        b[i]=0;
        (void)append_num(b,i,sizeof(b),m->requested);
        text(font,pixels,pitch,126,210,b,p->text,70);
        rect(pixels,pitch,5,227,310,7,p->line);
        if(m->progress.bytes_total){
            unsigned v=progress_width(m->progress.bytes_done,m->progress.bytes_total);
            if(v>310)v=310;
            rect(pixels,pitch,5,227,v,7,p->active_border);
        }else if(m->state==FM02_DONE)rect(pixels,pitch,5,227,310,7,p->active_border);
    }else if(m->marks_count){
        rect(pixels,pitch,0,223,320,17,p->footer);
        text(font,pixels,pitch,5,226,m->browser->locale?"Selected":"Выделено",p->text,85);
        b[0]=0;(void)append_num(b,0,sizeof(b),m->marks_count);
        text(font,pixels,pitch,96,226,b,p->text,40);
        text(font,pixels,pitch,163,226,m->browser->locale?"START actions":"START действия",p->secondary,152);
    }
}
