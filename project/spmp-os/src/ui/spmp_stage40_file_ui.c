#include "spmp_stage40_file_ui.h"
#include <stdint.h>

static size_t slen(const char *s, size_t limit) {
    size_t n=0; if (!s) return limit;
    while (n<limit && s[n]) n++;
    return n;
}
static void clear_dual(spmp_ro_dual_panel *dual) {
    /* Deliberately avoid a large temporary on a small ARM stack. */
    unsigned char *p=(unsigned char *)dual;
    for (size_t i=0;i<sizeof(*dual);i++) p[i]=0;
}
int spmp40_init(spmp40_file_ui *ui, spmp_ro_dual_panel *dual,
                const spmp_ro_fs *fs, spmp_ro_panel *scratch,
                const char *left, const char *right) {
    if (!ui || !dual || !fs || !scratch || !left || !right ||
        (void *)scratch==(void *)&dual->panels[0] ||
        (void *)scratch==(void *)&dual->panels[1] ||
        !spmp_ro_path_valid(left) || !spmp_ro_path_valid(right)) return -1;
    /* Stage34's readonly facade is the *only* filesystem entry point. */
    if (!fs->confirm_readonly || !fs->read_directory ||
        fs->confirm_readonly(fs->context)!=1) return -1;
    ui->dual=dual; ui->fs=fs; ui->scratch=scratch;
    clear_dual(dual);
    if (spmp_ro_open_page(fs,&dual->panels[0],scratch,left,0)!=0 ||
        spmp_ro_open_page(fs,&dual->panels[1],scratch,right,0)!=0) {
        clear_dual(dual); ui->dual=0; ui->fs=0; ui->scratch=0;
        return -1;
    }
    return 0;
}
static int path_join(char out[SPMP_FS_PATH_MAX],const char *path,const char *name){
    size_t a=slen(path,SPMP_FS_PATH_MAX),b=slen(name,SPMP_FS_NAME_MAX);
    if (!a || a>=SPMP_FS_PATH_MAX || !b || b>=SPMP_FS_NAME_MAX) return -1;
    size_t slash=a==1?0:1;
    if (a+slash+b>=SPMP_FS_PATH_MAX)return -1;
    for(size_t i=0;i<a;i++)out[i]=path[i];
    if(slash)out[a++]='/';
    for(size_t i=0;i<b;i++)out[a+i]=name[i];
    out[a+b]=0;
    return spmp_ro_path_valid(out)?0:-1;
}
static int path_parent(char out[SPMP_FS_PATH_MAX],const char *path){
    size_t len=slen(path,SPMP_FS_PATH_MAX);
    if(len==0||len>=SPMP_FS_PATH_MAX||!spmp_ro_path_valid(path))return -1;
    if(len==1)return 1;
    size_t cut=len;
    while(cut>1 && path[cut-1]!='/')cut--;
    if(cut<=1){out[0]='/';out[1]=0;return 0;}
    for(size_t i=0;i<cut-1;i++)out[i]=path[i];
    out[cut-1]=0;
    return 0;
}
int spmp40_navigate(spmp40_file_ui *ui, spmp40_nav nav){
    if(!ui || !ui->dual || !ui->fs || !ui->scratch || ui->dual->active>1)return -1;
    spmp_ro_panel *p=&ui->dual->panels[ui->dual->active];
    size_t count=p->count;
    if(nav==SPMP40_NAV_SWITCH){ui->dual->active^=1u;return 0;}
    if(nav==SPMP40_NAV_UP){
        if(p->selected>0){p->selected--;return 0;}
        if(p->page_offset==0)return 0;
        size_t prev=p->page_offset>SPMP_FS_MAX_ENTRIES?p->page_offset-SPMP_FS_MAX_ENTRIES:0;
        if(spmp_ro_open_page(ui->fs,p,ui->scratch,p->path,prev)!=0)return -1;
        if(p->count)p->selected=p->count-1;
        return 0;
    }
    if(nav==SPMP40_NAV_DOWN){
        if(count && p->selected+1<count){p->selected++;return 0;}
        if(count && p->page_offset+count<p->total_entries)
            return spmp_ro_open_page(ui->fs,p,ui->scratch,p->path,p->page_offset+count);
        return 0;
    }
    if(nav==SPMP40_NAV_PAGE_DOWN){
        if(p->page_offset+count>=p->total_entries)return 0;
        return spmp_ro_open_page(ui->fs,p,ui->scratch,p->path,p->page_offset+count);
    }
    if(nav==SPMP40_NAV_PAGE_UP){
        if(p->page_offset==0){p->selected=0;return 0;}
        size_t prev=p->page_offset>SPMP_FS_MAX_ENTRIES?p->page_offset-SPMP_FS_MAX_ENTRIES:0;
        return spmp_ro_open_page(ui->fs,p,ui->scratch,p->path,prev);
    }
    if(nav==SPMP40_NAV_ENTER){
        if(p->selected>=p->count || !p->entries[p->selected].is_dir)return -1;
        char path[SPMP_FS_PATH_MAX];
        if(path_join(path,p->path,p->entries[p->selected].name)!=0)return -1;
        return spmp_ro_open_page(ui->fs,p,ui->scratch,path,0);
    }
    if(nav==SPMP40_NAV_PARENT){
        char path[SPMP_FS_PATH_MAX];
        int r=path_parent(path,p->path);
        return r==1?0:(r!=0?-1:spmp_ro_open_page(ui->fs,p,ui->scratch,path,0));
    }
    return -1;
}

/* Safe token compositor. Pixel interpretation is supplied by host profile. */
typedef struct {
    const spmp39_surface16 *surface;
    spmp40_pixel16 token;
    uint32_t x,y,clip_width,clip_height;
} plot_context;
static void put_token(const spmp39_surface16 *s,uint32_t x,uint32_t y,spmp40_pixel16 c){
    if(x>=s->width||y>=s->height)return;
    uint8_t *p=s->pixels+(size_t)y*s->row_pitch_bytes+(size_t)x*2u;
    p[0]=c.first;p[1]=c.second;
}
static void put_clip(void *opaque,uint32_t x,uint32_t y){
    plot_context *p=(plot_context*)opaque;
    if(x<p->clip_width && y<p->clip_height &&
       x<=UINT32_MAX-p->x && y<=UINT32_MAX-p->y)
        put_token(p->surface,p->x+x,p->y+y,p->token);
}
static void rect(const spmp39_surface16 *s,uint32_t x,uint32_t y,
                 uint32_t w,uint32_t h,spmp40_pixel16 c){
    if(x>=s->width||y>=s->height)return;
    if(w>s->width-x)w=s->width-x;
    if(h>s->height-y)h=s->height-y;
    for(uint32_t iy=0;iy<h;iy++){
        uint8_t *p=s->pixels+(size_t)(y+iy)*s->row_pitch_bytes+(size_t)x*2u;
        for(uint32_t ix=0;ix<w;ix++){p[ix*2u]=c.first;p[ix*2u+1u]=c.second;}
    }
}
static void label(const spmp39_surface16 *s,spmp40_system_font font,void *ctx,
                  const char *text,uint32_t x,uint32_t y,uint32_t w,
                  uint32_t h,spmp40_pixel16 c){
    if(!font||!text||x>=s->width||y>=s->height)return;
    if(w>s->width-x)w=s->width-x;
    if(h>s->height-y)h=s->height-y;
    plot_context plot={s,c,x,y,w,h};
    font(ctx,text,w,put_clip,&plot);
}
static void append(char *dst,size_t cap,size_t *pos,const char *s){
    while(*s && *pos+1<cap)dst[(*pos)++]=*s++;
    dst[*pos]=0;
}
static void decimal(char *dst,size_t cap,size_t *pos,uint64_t value){
    char rev[21]; unsigned n=0;
    do{rev[n++]=(char)('0'+(value%10u));value/=10u;}while(value&&n<sizeof rev);
    while(n && *pos+1<cap)dst[(*pos)++]=rev[--n];
    dst[*pos]=0;
}
static void selected_summary(const spmp_ro_panel *p,char dst[96]){
    size_t i=0;dst[0]=0;
    append(dst,96,&i,"FILES:"); decimal(dst,96,&i,p->total_files);
    if(p->count && p->selected<p->count){
        const spmp_ro_entry *e=&p->entries[p->selected];
        append(dst,96,&i," SIZE:");decimal(dst,96,&i,e->bytes);
        char date[9];if(spmp_ro_short_date(e->fat_date,date)==0){
            append(dst,96,&i," DATE:");append(dst,96,&i,date);
        }
    }
}
int spmp40_render(const spmp40_file_ui *ui,const spmp39_surface16 *surface,
                  const spmp40_theme *t,spmp40_system_font font,void *font_ctx){
    if(!ui||!ui->dual||!surface||!t||!font||!surface->pixels||
       surface->width!=320||surface->height!=240||ui->dual->active>1)return -1;
    uint32_t required=0;
    if(spmp39_surface16_required(surface->width,surface->height,
                                surface->row_pitch_bytes,surface->capacity_bytes,&required)!=0)return -1;
    for(unsigned a=0;a<2;a++){
        const spmp_ro_panel *p=&ui->dual->panels[a];
        if(p->count>SPMP_FS_MAX_ENTRIES||(p->selected>=p->count && p->count!=0) ||
           !spmp_ro_path_valid(p->path))return -1;
    }
    rect(surface,0,0,320,240,t->background);
    rect(surface,0,0,320,18,t->border);
    label(surface,font,font_ctx,"SPMP-OS  FILE MANAGER",6,3,305,12,t->text);
    for(unsigned a=0;a<2;a++){
        uint32_t x=a?161u:2u;
        uint32_t w=a?157u:157u;
        const spmp_ro_panel *p=&ui->dual->panels[a];
        rect(surface,x,22,w,186,a==ui->dual->active?t->active_panel:t->panel);
        rect(surface,x,22,w,1,t->border);
        label(surface,font,font_ctx,p->path,x+3,25,w-6,12,t->text);
        rect(surface,x,38,w,1,t->border);
        /* 13 visible rows, 12 pixels high, scroll around selection. */
        size_t first=p->selected>=13?p->selected-12:0;
        for(size_t row=0;row<13 && first+row<p->count;row++){
            size_t k=first+row;
            uint32_t y=43u+(uint32_t)row*12u;
            const spmp_ro_entry *e=&p->entries[k];
            if(k==p->selected)rect(surface,x+2,y-1,w-4,12,t->selection);
            label(surface,font,font_ctx,e->is_dir?"[+]":"   ",x+4,y,24,10,t->dim_text);
            label(surface,font,font_ctx,e->name,x+26,y,w-30,10,t->text);
        }
        rect(surface,x,199,w,1,t->border);
        char pos[48];size_t n=0;pos[0]=0;
        append(pos,sizeof pos,&n,"ITEM ");
        decimal(pos,sizeof pos,&n,p->count?p->page_offset+p->selected+1:0);
        append(pos,sizeof pos,&n,"/");decimal(pos,sizeof pos,&n,p->total_entries);
        label(surface,font,font_ctx,pos,x+3,201,w-6,9,t->dim_text);
    }
    rect(surface,0,210,320,30,t->border);
    char status[96];selected_summary(&ui->dual->panels[ui->dual->active],status);
    label(surface,font,font_ctx,status,3,213,314,10,t->text);
    label(surface,font,font_ctx,"TAB  UP/DOWN  ENTER  BACK",3,226,314,10,t->text);
    (void)required;
    return 0;
}
