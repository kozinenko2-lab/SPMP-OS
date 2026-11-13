/* SPDX-License-Identifier: MIT */
#include "spmp_fm04.h"
#include <stddef.h>
#include <stdint.h>
static void rect(uint16_t *fb,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color) {
    unsigned iy,ix;
    if(x>=320u||y>=240u)return;
    if(w>320u-x)w=320u-x;
    if(h>240u-y)h=240u-y;
    for(iy=0;iy<h;iy++)for(ix=0;ix<w;ix++)fb[(y+iy)*pitch+x+ix]=color;
}
static void border(uint16_t *fb,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color) {
    if(!w||!h)return;
    rect(fb,pitch,x,y,w,1u,color);rect(fb,pitch,x,y+h-1u,w,1u,color);
    rect(fb,pitch,x,y,1u,h,color);rect(fb,pitch,x+w-1u,y,1u,h,color);
}
static void blend(uint16_t *fb,unsigned pitch,unsigned x,unsigned y,unsigned w,unsigned h,uint16_t color) {
    unsigned iy,ix;
    if(x>=320u||y>=240u)return;
    if(w>320u-x)w=320u-x;
    if(h>240u-y)h=240u-y;
    for(iy=0;iy<h;iy++)for(ix=0;ix<w;ix++){
        unsigned old=fb[(y+iy)*pitch+x+ix];
        fb[(y+iy)*pitch+x+ix]=(uint16_t)(((old&0xf7deu)>>1)+((color&0xf7deu)>>1));
    }
}
static void draw(const fm01_font *font,uint16_t *fb,unsigned pitch,unsigned x,unsigned y,
    const char *s,uint16_t color,unsigned width,unsigned height) {
    if(font&&font->draw&&x<320u&&y<240u&&width)
        font->draw(font->ctx,fb,pitch,x,y,s,color,width,height);
}
static unsigned display_cursor(const char *source,unsigned offset,unsigned end,char *dest,unsigned capacity) {
    unsigned o=0,pos=0,i=offset;
    if(!capacity)return 0;
    while(i<end && source[i] && o+5u<capacity){
        unsigned c=(uint8_t)source[i],n=c<128u?1u:(c<224u?2u:(c<240u?3u:4u));
        if(pos==24u)break;
        if(o+n+1u>=capacity)break;
        while(n--&&i<end)dest[o++]=source[i++];
        pos++;
    }
    dest[o]=0;
    return o;
}
void fm04_render_overlay(const fm04_model *k,uint16_t *fb,unsigned pitch,
    const fm01_palette *pal,const fm01_font *font) {
    unsigned row,col,id,i,start,cursor_chars=0,chars=0;
    char visible[FM01_NAME_MAX+8u];
    const char *name,*hint;
    if(!k||k->state!=FM04_EDITING||!fb||pitch<320u||!pal)return;
    blend(fb,pitch,2,19,316,218,pal->background);
    rect(fb,pitch,4,21,312,216,pal->panel);
    border(fb,pitch,4,21,312,216,pal->active_border);
    rect(fb,pitch,6,23,308,20,pal->footer);
    name=fm04_localize(k,k->mode==FM04_FILENAME?FM04_TEXT_RENAME:FM04_TEXT_INPUT);
    draw(font,fb,pitch,12,25,name,pal->text,240,13);
    draw(font,fb,pitch,261,25,k->layout==FM04_RU?"RU":(k->layout==FM04_EN?"EN":"#+"),pal->secondary,40,12);
    rect(fb,pitch,9,45,302,35,pal->background);
    border(fb,pitch,9,45,302,35,pal->line);
    /* Scroll input so cursor remains visible. Text is UTF-8; never slice mid-codepoint. */
    for(i=0;i<k->cursor;i++)if(((uint8_t)k->value[i]&0xc0u)!=0x80u)cursor_chars++;
    start=0;
    if(cursor_chars>23u){unsigned skip=cursor_chars-23u,seen=0;
        while(start<k->bytes && seen<skip){
            if(((uint8_t)k->value[start]&0xc0u)!=0x80u)seen++;
            start++;
        }
        while(start<k->bytes && ((uint8_t)k->value[start]&0xc0u)==0x80u)start++;
    }
    {
        unsigned taken=display_cursor(k->value,start,k->bytes,visible,sizeof(visible));
        (void)taken;
        draw(font,fb,pitch,15,54,visible,pal->text,286,14);
    }
    for(i=start;i<k->cursor;i++)if(((uint8_t)k->value[i]&0xc0u)!=0x80u)chars++;
    /* Cursor estimate is intentionally independent of font metrics (FM01 exposes draw only).
       Firmware may later supply measure callback for pixel-perfect caret. */
    if(chars>33u)chars=33u;
    rect(fb,pitch,15u+chars*8u,69u,2u,8u,pal->active_border);
    if(k->error)draw(font,fb,pitch,12,78,fm04_localize(k,FM04_TEXT_ERROR),
        pal->active_border,270,10);
    for(row=0;row<FM04_ROWS;row++)for(col=0;col<FM04_COLS;col++){
        unsigned x=6u+28u*col,y=91u+29u*row;
        uint16_t c=k->selection==row*FM04_COLS+col?pal->highlight:pal->panel_inactive;
        id=row*FM04_COLS+col;
        rect(fb,pitch,x,y,27,26,c);
        border(fb,pitch,x,y,27,26,k->selection==id?pal->active_border:pal->line);
        draw(font,fb,pitch,x+2u,y+5u,fm04_key_label(k,id),k->selection==id?pal->highlight_text:pal->text,23,11);
    }
    hint=fm04_localize(k,FM04_TEXT_HINT);
    rect(fb,pitch,6,210,308,22,pal->footer);
    draw(font,fb,pitch,11,215,hint,pal->secondary,298,11);
}
