#include "spmp_fm01.h"

static void putsafe(char *dst,size_t cap,const char *src) {
    size_t i=0;if(!cap)return;
    while(i+1<cap && src[i]) {dst[i]=src[i];i++;}dst[i]=0;
}
static unsigned u32_dec(unsigned v,char *out) {
    char buf[12];unsigned i=0,j;
    do {buf[i++]=(char)('0'+v%10u);v/=10u;}while(v && i<sizeof(buf));
    for(j=0;j<i;j++)out[j]=buf[i-j-1];
    return i;
}
static void append_str(char *dst,size_t cap,const char *s) {
    size_t n=0;while(n<cap && dst[n])n++;
    while(n+1<cap && *s)dst[n++]=*s++;
    if(n<cap)dst[n]=0;
}
static void append_num(char *dst,size_t cap,uint32_t n) {
    char tmp[12];unsigned len=u32_dec(n,tmp);tmp[len]=0;append_str(dst,cap,tmp);
}
void fm01_size_text(uint64_t bytes,char *dst,size_t cap) {
    static const char *units[]={"B","KiB","MiB","GiB","TiB","PiB"};
    uint64_t n=bytes;unsigned unit=0;unsigned frac=0;
    if(!cap)return;
    while(n>=1024 && unit<5){frac=(unsigned)(n&1023u);n>>=10;unit++;}
    dst[0]=0;
    append_num(dst,cap,(uint32_t)n);
    if(unit && n<100) {char c[4]={'.',(char)('0'+(frac*10u+512u)/1024u),0,0};
        if(c[1]>'9')c[1]='9';
        append_str(dst,cap,c);}
    append_str(dst,cap," ");append_str(dst,cap,units[unit]);
}
void fm01_date_text(const fm01_entry *e,char *dst,size_t cap) {
    char b[9];
    if(!cap)return;
    if(!e || !e->year || e->month<1 || e->month>12 || e->day<1 || e->day>31) {putsafe(dst,cap,"--.--.--");return;}
    b[0]=(char)('0'+e->day/10);b[1]=(char)('0'+e->day%10);b[2]='.';
    b[3]=(char)('0'+e->month/10);b[4]=(char)('0'+e->month%10);b[5]='.';
    b[6]=(char)('0'+(e->year%100)/10);b[7]=(char)('0'+e->year%10);b[8]=0;
    putsafe(dst,cap,b);
}

void fm01_palette_default(fm01_palette *p) {
    if(!p)return;
    p->background=0x0821;p->panel=0x10A3;p->panel_inactive=0x1082;
    p->line=0x2965;p->text=0xFFFF;p->secondary=0xBDF7;
    p->active_border=0x05FF;p->highlight=0x043F;
    p->highlight_text=0xFFFF;p->footer=0x1082;
}
static void rect(uint16_t *px,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t c) {
    unsigned yy,xx;if(x>=FM01_WIDTH || y>=FM01_HEIGHT)return;
    if(x+w>FM01_WIDTH)w=FM01_WIDTH-x;
    if(y+h>FM01_HEIGHT)h=FM01_HEIGHT-y;
    for(yy=y;yy<y+h;yy++)for(xx=x;xx<x+w;xx++)px[yy*pitch+xx]=c;
}
static void blend_rect(uint16_t *px,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t c) {
    unsigned yy,xx;
    if(x>=FM01_WIDTH || y>=FM01_HEIGHT)return;
    if(x+w>FM01_WIDTH)w=FM01_WIDTH-x;
    if(y+h>FM01_HEIGHT)h=FM01_HEIGHT-y;
    for(yy=y;yy<y+h;yy++)for(xx=x;xx<x+w;xx++) {
        uint16_t old=px[yy*pitch+xx];
        /* 50% alpha in RGB565 without crossing adjacent components */
        px[yy*pitch+xx]=(uint16_t)(((old&0xF7DEu)>>1)+((c&0xF7DEu)>>1));
    }
}
static void border(uint16_t *px,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t c) {
    if(!w||!h)return;
    rect(px,pitch,x,y,w,1,c);rect(px,pitch,x,y+h-1,w,1,c);
    rect(px,pitch,x,y,1,h,c);rect(px,pitch,x+w-1,y,1,h,c);
}
/* Fraction n/d mapped to 0..144 with only shifts/subtractions.
 * Avoids ARMv5 __aeabi_uidiv, which is not guaranteed by firmware HAL. */
static unsigned scroll_y(uint32_t n,uint32_t d) {
    uint64_t remainder=n;uint32_t frac=0;unsigned i;
    if(!d)return 0;
    for(i=0;i<8;i++) {
        remainder<<=1;frac<<=1;
        if(remainder>=d){remainder-=d;frac|=1u;}
    }
    return (unsigned)((frac*145u)>>8);
}
static void draw(const fm01_font *font,uint16_t *px,unsigned pitch,
    unsigned x,unsigned y,const char *text,uint16_t color,unsigned width,unsigned pixels) {
    if(font && font->draw && width && x<FM01_WIDTH && y<FM01_HEIGHT)
       font->draw(font->ctx,px,pitch,x,y,text,color,width,pixels);
}
static void symbol(uint16_t *px,unsigned pitch,unsigned x,unsigned y,unsigned dir,uint16_t c) {
    /* Language-neutral folder/file pictogram, no baked-in text. */
    if(dir){rect(px,pitch,x,y+3,3,2,c);rect(px,pitch,x+2,y+2,6,2,c);rect(px,pitch,x,y+4,10,7,c);}
    else {rect(px,pitch,x+1,y,8,11,c);rect(px,pitch,x+3,y+2,5,1,0x0861);rect(px,pitch,x+3,y+5,5,1,0x0861);}
}
static void text_path(const char *path,char *out,unsigned cap) {
    const char *p=path;unsigned i=0;
    if(!cap)return;
    out[i++]='/';
    while(*p && i+1<cap)out[i++]=*p++;
    out[i]=0;
}
static const char *label(unsigned locale,unsigned k) {
    static const char *ru[]={"Ф:","Кат:","Пусто","Ошибка чтения","Нет SD-карты","A открыть  B назад","L/R панели","Файл","Папка"};
    static const char *en[]={"Files:","Dirs:","Empty","Read error","No SD card","A open  B back","L/R panels","File","Folder"};
    return (locale?en:ru)[k];
}
void fm01_render_overlay(const fm01_model *m,uint16_t *px,unsigned pitch,const fm01_palette *p,
    const fm01_font *font,const fm01_status *s) {
    unsigned i,volume=0,sd=1;char tmp[64],date[20],size[32];
    if(!m||!px||pitch<FM01_WIDTH||!p)return;
    rect(px,pitch,0,0,320,20,p->footer);
    draw(font,px,pitch,5,2,"SPMP-OS / FM",p->text,152,13);
    if(s){volume=s->volume_0_100;sd=(unsigned)(s->sd_present!=0);}
    if(volume>100)volume=100;
    if(s && s->time_hhmm[0])draw(font,px,pitch,196,2,s->time_hhmm,p->text,45,13);
    rect(px,pitch,246,6,3,8,p->secondary);
    for(i=0;i<5;i++)rect(px,pitch,252+i*5,13-(i*2),3,2+i*2,
        i*20u<volume?p->highlight:p->line);
    border(px,pitch,284,4,30,13,sd?p->active_border:p->line);
    draw(font,px,pitch,288,4,sd?"SD":"--",sd?p->text:p->secondary,23,12);
    for(i=0;i<2;i++) {
        const fm01_panel *q=&m->panels[i];
        unsigned x=2+i*160,j;uint16_t base=(i==m->active)?p->panel:p->panel_inactive;
        blend_rect(px,pitch,x,23,156,181,base);
        border(px,pitch,x,23,156,181,(i==m->active)?p->active_border:p->line);
        rect(px,pitch,x+2,25,152,17,p->footer);
        text_path(q->path,tmp,sizeof(tmp));
        draw(font,px,pitch,x+5,26,tmp,p->text,147,11);
        if(!sd){draw(font,px,pitch,x+7,91,label(m->locale,4),p->secondary,140,11);continue;}
        if(q->last_error){draw(font,px,pitch,x+7,91,label(m->locale,3),p->secondary,140,11);continue;}
        if(!q->total){draw(font,px,pitch,x+7,91,label(m->locale,2),p->secondary,140,11);continue;}
        for(j=0;j<q->loaded && j<FM01_PAGE_ROWS;j++) {
            const fm01_entry *e=&q->entries[j];
            unsigned y=44+j*17;
            unsigned sel=(q->cursor==q->first+j);
            if(sel && i==m->active)rect(px,pitch,x+2,y,152,16,p->highlight);
            else if(j%2u==1u)blend_rect(px,pitch,x+2,y,152,16,p->background);
            symbol(px,pitch,x+6,y+2,e->type==FM01_ENTRY_DIR,p->secondary);
            draw(font,px,pitch,x+22,y+1,e->name,sel && i==m->active?p->highlight_text:p->text,130,11);
        }
        /* Narrow scrollbar; page is at most 9 rows, but total may be unbounded. */
        if(q->total>FM01_PAGE_ROWS){
            uint32_t pos=scroll_y(q->cursor,q->total);
            rect(px,pitch,x+153,44,2,153,p->line);
            rect(px,pitch,x+153,44+pos,2,8,p->active_border);
        }
    }
    rect(px,pitch,0,207,320,33,p->footer);
    border(px,pitch,0,207,320,33,p->line);
    if(sd) {
       const fm01_panel *q=&m->panels[m->active];const fm01_entry *sel=fm01_selected(m,m->active);
       unsigned n=0;
       tmp[0]=0;append_str(tmp,sizeof(tmp),label(m->locale,0));append_num(tmp,sizeof(tmp),q->file_count);
       append_str(tmp,sizeof(tmp),"  ");append_str(tmp,sizeof(tmp),label(m->locale,1));append_num(tmp,sizeof(tmp),q->dir_count);
       draw(font,px,pitch,5,208,tmp,p->text,123,11);
       if(sel && sel->type==FM01_ENTRY_FILE)fm01_size_text(sel->size_bytes,size,sizeof(size));
       else putsafe(size,sizeof(size),"--");
       if(sel)fm01_date_text(sel,date,sizeof(date));else putsafe(date,sizeof(date),"--.--.--");
       draw(font,px,pitch,138,208,size,p->text,82,11);
       draw(font,px,pitch,237,208,date,p->text,80,11);
       n=(unsigned)m->active;(void)n;
    }else draw(font,px,pitch,5,209,label(m->locale,4),p->secondary,180,11);
    draw(font,px,pitch,6,223,label(m->locale,5),p->secondary,151,10);
    draw(font,px,pitch,205,223,label(m->locale,6),p->secondary,106,10);
}

void fm01_render(const fm01_model *m,uint16_t *px,unsigned pitch,
    const fm01_palette *p,const fm01_font *font,const fm01_status *s) {
    if(!m||!px||pitch<FM01_WIDTH||!p)return;
    rect(px,pitch,0,0,FM01_WIDTH,FM01_HEIGHT,p->background);
    fm01_render_overlay(m,px,pitch,p,font,s);
}
