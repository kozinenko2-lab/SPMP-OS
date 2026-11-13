/* SPDX-License-Identifier: MIT
 * RGB565 QVGA viewer. Glyphs and translations come from the firmware font API.
 */
#include "spmp_fm05.h"
#include <stdint.h>
#include <stddef.h>

static void rect(uint16_t *fb,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color){
    unsigned dx,dy;if(x>=320u||y>=240u)return;
    if(w>320u-x)w=320u-x;if(h>240u-y)h=240u-y;
    for(dy=0;dy<h;dy++)for(dx=0;dx<w;dx++)fb[(y+dy)*pitch+x+dx]=color;
}
static void write_text(const fm01_font *font,uint16_t *fb,unsigned pitch,unsigned x,unsigned y,
    const char *s,uint16_t color,unsigned width,unsigned size){
    if(font&&font->draw&&s)font->draw(font->ctx,fb,pitch,x,y,s,color,width,size);
}
static void utoa(unsigned n,char *dst,unsigned cap){
    char rev[16];unsigned i=0,j=0;if(!cap)return;
    do{rev[i++]=(char)('0'+n%10u);n/=10u;}while(n&&i<sizeof(rev));
    while(i&&j+1u<cap)dst[j++]=rev[--i];dst[j]=0;
}
static unsigned cp_length(const char *p){unsigned v=(uint8_t)p[0];return v<0x80u?1u:v<0xe0u?2u:v<0xf0u?3u:4u;}
static void line_snippet(const fm05_doc *m,unsigned line,unsigned first,char *out,unsigned cap){
    unsigned p=fm05_line_start_offset(m,line),col=0,n=0,cp=0,i;
    while(p<m->length&&m->text[p]!='\n'&&m->text[p]!='\r'&&col<first+FM05_VIEW_COLS){
        unsigned k=cp_length(m->text+p);
        if(k>m->length-p)break;
        if(col>=first){
            if(m->text[p]=='\t'){
                if(n+2u>=cap)break;out[n++]=' ';out[n++]=' ';
            }else{
                if(n+k>=cap)break;
                for(i=0;i<k;i++)out[n++]=m->text[p+i];
            }
            cp++;
        }
        p+=k;col++;
    }
    (void)cp;out[n]=0;
}
void fm05_render(const fm05_doc *m,uint16_t *fb,unsigned pitch,
    const fm01_palette *c,const fm01_font *font){
    fm01_palette p;char count[16],pos[16],line_text[16];
    unsigned i,line,caret_line,col;
    uint16_t panel,accent,secondary,text;
    if(!m||!fb||pitch<320u||m->state==FM05_CLOSED)return;
    if(!c){fm01_palette_default(&p);c=&p;}
    panel=c->panel;accent=c->active_border;secondary=c->secondary;text=c->text;
    rect(fb,pitch,0,0,320,240,c->background);
    rect(fb,pitch,0,0,320,22,c->footer);
    rect(fb,pitch,0,21,320,1,accent);
    write_text(font,fb,pitch,5,4,fm05_localize(m,FM05_TX_EDITOR),text,66,12);
    write_text(font,fb,pitch,74,5,m->path,secondary,235,10);
    caret_line=fm05_caret_line(m);col=fm05_caret_column(m);
    rect(fb,pitch,0,23,320,177,panel);
    rect(fb,pitch,0,23,34,177,c->panel_inactive);
    for(i=0;i<FM05_VIEW_ROWS;i++){
        char snippet[FM05_VIEW_COLS*4u+6u];
        unsigned y=25u+i*14u;
        line=m->top_line+i;
        if(line>=m->lines)break;
        if(line==caret_line){rect(fb,pitch,0,y,320,14,c->highlight);rect(fb,pitch,0,y,33,14,c->active_border);}
        utoa(line+1u,line_text,sizeof(line_text));
        write_text(font,fb,pitch,3,y+1u,line_text,line==caret_line?c->highlight_text:secondary,29,10);
        line_snippet(m,line,m->first_col,snippet,sizeof(snippet));
        write_text(font,fb,pitch,38,y+1u,snippet,line==caret_line?c->highlight_text:text,278,11);
        if(line==caret_line){
            if(col>=m->first_col&&col-m->first_col<FM05_VIEW_COLS){
                unsigned cx=38u+7u*(col-m->first_col);
                if(cx<316u)rect(fb,pitch,cx,y+12u,6u,1u,accent);
            }
        }
    }
    rect(fb,pitch,0,202,320,38,c->footer);
    rect(fb,pitch,0,201,320,1,accent);
    utoa(caret_line+1u,count,sizeof(count));utoa(col+1u,pos,sizeof(pos));
    write_text(font,fb,pitch,5,204,fm05_localize(m,FM05_TX_LINE),text,25,10);
    write_text(font,fb,pitch,29,204,count,secondary,35,10);
    write_text(font,fb,pitch,65,204,fm05_localize(m,FM05_TX_COLUMN),text,26,10);
    write_text(font,fb,pitch,92,204,pos,secondary,40,10);
    utoa(m->length,count,sizeof(count));
    write_text(font,fb,pitch,147,204,count,secondary,55,10);
    write_text(font,fb,pitch,203,204,fm05_localize(m,FM05_TX_BYTES),secondary,35,10);
    write_text(font,fb,pitch,244,204,m->dirty?"*":"OK",m->dirty?accent:text,27,10);
    if(m->state==FM05_DISCARD_CONFIRM){
        rect(fb,pitch,14,74,292,75,c->panel_inactive);
        rect(fb,pitch,14,74,292,2,accent);
        write_text(font,fb,pitch,22,85,fm05_localize(m,FM05_TX_DISCARD),text,272,12);
        write_text(font,fb,pitch,22,120,fm05_localize(m,FM05_TX_DISCARD_HELP),text,275,10);
    }
    write_text(font,fb,pitch,4,221,fm05_localize(m,FM05_TX_CONTROLS),text,312,9);
}
