#include "spmp_fm03.h"
#include <stddef.h>
static void rect(uint16_t *p,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t c){
    unsigned yy,xx;
    if(x>=320||y>=240)return;
    if(w>320-x)w=320-x;
    if(h>240-y)h=240-y;
    for(yy=0;yy<h;yy++)for(xx=0;xx<w;xx++)p[(y+yy)*pitch+x+xx]=c;
}
static void draw(const fm01_font *f,uint16_t *p,unsigned pitch,unsigned x,unsigned y,
                 const char *s,uint16_t c,unsigned w,unsigned fontsize){
    if(f && f->draw && s)f->draw(f->ctx,p,pitch,x,y,s,c,w,fontsize);
}
static void putstr(char *dst,unsigned cap,const char *s){
    unsigned i=0;if(!cap)return;
    while(s[i] && i+1<cap){dst[i]=s[i];i++;}
    dst[i]=0;
}
static void append(char *dst,unsigned cap,const char *s){
    unsigned i=0,j=0;
    if(!cap)return;
    while(i<cap && dst[i])i++;
    while(i+1<cap && s[j])dst[i++]=s[j++];
    if(i<cap)dst[i]=0;
}
static void num(uint32_t n,char *buf){
    char rev[11];unsigned k=0,i=0;
    do{rev[k++]=(char)('0'+n%10u);n/=10u;}while(n && k<sizeof(rev));
    while(k){buf[i++]=rev[--k];}
    buf[i]=0;
}
static void drawpair(const fm01_font *f,uint16_t *p,unsigned pitch,
    unsigned y,const char *label,const char *value,const fm01_palette *colors){
    draw(f,p,pitch,28,y,label,colors->secondary,102,12);
    draw(f,p,pitch,132,y,value,colors->text,157,12);
}
static void tail_path(const char *path,char *out,unsigned cap){
    unsigned n=0,i=0;if(!cap)return;
    while(path[n])n++;
    if(n+1<=cap){putstr(out,cap,path);return;}
    if(cap<6){putstr(out,cap,"...");return;}
    out[0]='.';out[1]='.';out[2]='.';
    i=n-(cap-4);
    while(i<n && (((unsigned char)path[i]&0xC0u)==0x80u))i++;
    putstr(out+3,cap-3,path+i);
}
static const char *word(unsigned locale,unsigned index){
    static const char *ru[]={"Свойства","Имя","Путь","Тип","Размер","Изменён","Файлы","Папки","На SD","Сканирование...","Завершено","Прервано","Ошибка","Файл","Каталог","Только чтение","Да","Нет","B Закрыть","START Стоп","Скрытый"};
    static const char *en[]={"Properties","Name","Path","Type","Size","Modified","Files","Folders","SD used","Scanning...","Complete","Cancelled","Error","File","Folder","Read only","Yes","No","B Close","START Stop","Hidden"};
    return (locale?en:ru)[index];
}
void fm03_render_overlay(const fm03_model *m,uint16_t *p,unsigned pitch,
    const fm01_palette *colors,const fm01_font *font){
    unsigned loc;char str[48],scratch[48],dated[20],path[FM01_PATH_MAX];
    fm01_entry dt;
    if(!m||!m->is_open||!p||pitch<320||!colors)return;
    loc=(m->browser && m->browser->locale)?1:0;
    rect(p,pitch,12,22,296,193,colors->panel);
    rect(p,pitch,12,22,296,2,colors->active_border);
    rect(p,pitch,12,213,296,2,colors->active_border);
    rect(p,pitch,12,24,2,189,colors->line);
    rect(p,pitch,306,24,2,189,colors->line);
    rect(p,pitch,14,26,292,20,colors->footer);
    draw(font,p,pitch,22,28,word(loc,0),colors->text,279,15);
    if(!m->has_info){draw(font,p,pitch,25,91,word(loc,12),colors->text,270,12);return;}
    /* File names may be longer than visible width. The external font clips output. */
    drawpair(font,p,pitch,49,word(loc,1),m->info.name,colors);
    tail_path(m->info.path,path,39u);
    drawpair(font,p,pitch,66,word(loc,2),path,colors);
    drawpair(font,p,pitch,83,word(loc,3),word(loc,m->info.kind==FM01_ENTRY_DIR?14:13),colors);
    if(m->info.kind==FM01_ENTRY_DIR)fm01_size_text(m->scan.bytes,str,sizeof(str));
    else fm01_size_text(m->info.bytes,str,sizeof(str));
    if(m->info.kind==FM01_ENTRY_DIR && m->state==FM03_SCANNING)append(str,sizeof(str)," +");
    drawpair(font,p,pitch,100,word(loc,4),str,colors);
    dt.year=m->info.year;dt.month=m->info.month;dt.day=m->info.day;
    fm01_date_text(&dt,dated,sizeof(dated));
    num(m->info.hour,str);if(m->info.hour<10u){scratch[0]='0';putstr(scratch+1,sizeof(scratch)-1,str);putstr(str,sizeof(str),scratch);}
    append(dated,sizeof(dated)," ");append(dated,sizeof(dated),str);append(dated,sizeof(dated),":");
    num(m->info.minute,str);if(m->info.minute<10u){scratch[0]='0';putstr(scratch+1,sizeof(scratch)-1,str);putstr(str,sizeof(str),scratch);}
    append(dated,sizeof(dated),str);
    drawpair(font,p,pitch,117,word(loc,5),dated,colors);
    if(m->info.kind==FM01_ENTRY_DIR){
        num(m->scan.files,str);drawpair(font,p,pitch,134,word(loc,6),str,colors);
        num(m->scan.directories,str);drawpair(font,p,pitch,151,word(loc,7),str,colors);
    }else{
        drawpair(font,p,pitch,134,word(loc,15),word(loc,(m->info.attrs&FM03_ATTR_READ_ONLY)?16:17),colors);
        drawpair(font,p,pitch,151,word(loc,20),word(loc,(m->info.attrs&FM03_ATTR_HIDDEN)?16:17),colors);
    }
    if(m->volume.valid){
        fm01_size_text(m->volume.total_bytes>=m->volume.free_bytes ?
                       m->volume.total_bytes-m->volume.free_bytes : 0u,str,sizeof(str));
        append(str,sizeof(str)," / ");
        fm01_size_text(m->volume.total_bytes,scratch,sizeof(scratch));
        append(str,sizeof(str),scratch);
        drawpair(font,p,pitch,168,word(loc,8),str,colors);
    }
    rect(p,pitch,15,187,289,24,colors->footer);
    switch(m->state){
        case FM03_SCANNING:putstr(str,sizeof(str),word(loc,9));break;
        case FM03_FINISHED:putstr(str,sizeof(str),word(loc,10));break;
        case FM03_CANCELLED:putstr(str,sizeof(str),word(loc,11));break;
        case FM03_ERROR:putstr(str,sizeof(str),word(loc,12));break;
        default:putstr(str,sizeof(str)," ");break;
    }
    draw(font,p,pitch,20,191,str,colors->secondary,147,12);
    draw(font,p,pitch,180,191,word(loc,m->state==FM03_SCANNING?19:18),colors->text,122,12);
}
