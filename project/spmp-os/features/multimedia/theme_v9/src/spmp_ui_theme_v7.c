#include "spmp_ui_theme_v7.h"
/* v7 independent theme engine. No memory allocations, malloc, printf or libc. */
static unsigned u16(const uint8_t*p){return (unsigned)p[0]|((unsigned)p[1]<<8);}
static unsigned mn(unsigned a,unsigned b){return a<b?a:b;}
static uint16_t mix(uint16_t a,uint16_t b,unsigned alpha){
 unsigned r=((a>>11&31u)*alpha+(b>>11&31u)*(16u-alpha)+8u)/16u;
 unsigned g=((a>>5&63u)*alpha+(b>>5&63u)*(16u-alpha)+8u)/16u;
 unsigned bl=((a&31u)*alpha+(b&31u)*(16u-alpha)+8u)/16u;
 return (uint16_t)(((r>31?31:r)<<11)|((g>63?63:g)<<5)|(bl>31?31:bl));
}
int spmp_ui_parse_style(const uint8_t*data,size_t n,spmp_ui_style*s){
 if(!data||!s||n!=32||data[0]!='S'||data[1]!='P'||data[2]!='T'||data[3]!='H'||u16(data+4)!=1||u16(data+6)!=32)return -1;
 if(data[8]>1||data[9]>1)return -1;
 s->layout_default=data[8];s->enable_sfx=data[9];
 s->accent=(uint16_t)u16(data+10);s->panel=(uint16_t)u16(data+12);
 s->text=(uint16_t)u16(data+14);s->muted=(uint16_t)u16(data+16);
 s->selection=(uint16_t)u16(data+18);s->border=(uint16_t)u16(data+20);
 return 0;
}
int spmp_ui_validate_font(const uint8_t*data,size_t n){
 if(!data||n<8||data[0]!='S'||data[1]!='P'||data[2]!='F'||data[3]!='T')return -1;
 unsigned c=u16(data+4);if(!c||c>512||data[6]!=16||data[7]!=2||n!=8u+c*36u)return -1;
 unsigned previous=0;
 for(unsigned i=0;i<c;++i){const uint8_t*g=data+8u+i*36u;unsigned cp=u16(g);
  if((i&&cp<=previous)||g[2]<1||g[2]>12||g[3]<1||g[3]>12)return -1;
  previous=cp;}
 return 0;
}
int spmp_ui_validate_icons(const uint8_t*d,size_t n){
 return d&&n==8u+8u*54u&&d[0]=='S'&&d[1]=='P'&&d[2]=='I'&&d[3]=='1'&&u16(d+4)==8&&d[6]==18&&d[7]==18?0:-1;
}
int spmp_ui_validate_background(spmp_ui_read_at fn,void*ctx){
 uint8_t h[16];if(!fn||fn(ctx,0,h,16)!=16)return -1;
 return h[0]=='S'&&h[1]=='P'&&h[2]=='B'&&h[3]=='G'&&u16(h+4)==320&&u16(h+6)==240&&u16(h+8)==1?0:-1;
}
void spmp_ui_state_init(spmp_ui_state*s,uint8_t layout,uint8_t n){
 if(!s)return;
 s->layout=layout==1?1:0;s->selected=s->scroll=0;s->menu_count=(uint8_t)mn(n,32);
 s->frame=s->anim_frame=0;s->previous_selected=0;
}
spmp_ui_sfx spmp_ui_key_event(spmp_ui_state*s,spmp_ui_key key){
 if(!s)return SPMP_UI_SFX_NONE;
 if(key==SPMP_UI_ACCEPT)return SPMP_UI_SFX_ACCEPT;
 if(key==SPMP_UI_BACK)return SPMP_UI_SFX_BACK;
 if(key==SPMP_UI_TOGGLE_LAYOUT){s->layout^=1;s->scroll=0;s->anim_frame=0;return SPMP_UI_SFX_MOVE;}
 if(!s->menu_count)return SPMP_UI_SFX_NONE;
 unsigned v=s->selected;
 if(s->layout==SPMP_UI_LAYOUT_LIST){
  if(key==SPMP_UI_UP){if(v)v--;}
  else if(key==SPMP_UI_DOWN){if(v+1<s->menu_count)v++;}
  else return SPMP_UI_SFX_NONE;
 }else{
  if(key==SPMP_UI_LEFT){if(v%3)v--;}
  else if(key==SPMP_UI_RIGHT){if(v%3!=2&&v+1<s->menu_count)v++;}
  else if(key==SPMP_UI_UP){if(v>=3)v-=3;}
  else if(key==SPMP_UI_DOWN){if(v+3<s->menu_count)v+=3;}
  else return SPMP_UI_SFX_NONE;
 }
 if(v==s->selected)return SPMP_UI_SFX_NONE;
 s->previous_selected=s->selected;s->selected=(uint8_t)v;s->anim_frame=0;
 if(s->layout==SPMP_UI_LAYOUT_LIST){
  if(v<s->scroll)s->scroll=(uint8_t)v;
  if(v>=s->scroll+8)s->scroll=(uint8_t)(v-7);
 }else s->scroll=(uint8_t)((v/9)*9);
 return SPMP_UI_SFX_MOVE;
}
void spmp_ui_animate(spmp_ui_state*s){if(s){s->frame++;if(s->anim_frame<15)s->anim_frame++;}}
static void rect(uint16_t*fb,unsigned pitch,int x,int y,int w,int h,uint16_t c,unsigned alpha){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x>=320||y>=240)return;
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 if(w<=0||h<=0)return;
 for(int yy=y;yy<y+h;yy++){uint16_t*p=fb+(unsigned)yy*pitch+(unsigned)x;
  for(int xx=0;xx<w;xx++){p[xx]=alpha==16?c:mix(c,p[xx],alpha);}}
}
static unsigned utf8_next(const char **pp){const uint8_t*p=(const uint8_t*)*pp;unsigned a=*p;
 if(!a)return 0;
 if(a<128){*pp=(const char*)(p+1);return a;}
 if(a>=0xc2&&a<=0xdf&&p[1]>=0x80&&p[1]<=0xbf){*pp=(const char*)(p+2);return ((a&31u)<<6)|(p[1]&63u);}
 if(a>=0xe0&&a<=0xef&&p[1]>=0x80&&p[1]<=0xbf&&p[2]>=0x80&&p[2]<=0xbf){*pp=(const char*)(p+3);return ((a&15u)<<12)|((p[1]&63u)<<6)|(p[2]&63u);}
 *pp=(const char*)(p+1);return '?';
}
static const uint8_t *glyph_find(const uint8_t *d,unsigned code){
 unsigned lo=0,hi=u16(d+4);
 while(lo<hi){unsigned md=lo+(hi-lo)/2;const uint8_t*g=d+8+md*36u;
  unsigned v=u16(g);if(v==code)return g;if(v<code)lo=md+1;else hi=md;}
 return 0;
}
static void string(uint16_t*fb,unsigned pitch,int x,int y,const uint8_t*font,const char*t,uint16_t fg,unsigned maxw){
 if(!t)return;
 int start=x;
 for(unsigned glyphs=0;*t&&glyphs<96;glyphs++){
  unsigned cp=utf8_next(&t);const uint8_t*g=glyph_find(font,cp);if(!g)g=glyph_find(font,'?');if(!g)continue;
  unsigned width=g[3],advance=g[2];if((unsigned)(x-start)+advance>maxw)break;
  for(unsigned yy=0;yy<16;yy++){
   int iy=y+(int)yy;if(iy<0||iy>=240)continue;
   unsigned bits=u16(g+4+yy*2);
   for(unsigned xx=0;xx<width;xx++)if(bits&(1u<<xx)){
    int ix=x+(int)xx;if(ix>=0&&ix<320)fb[(unsigned)iy*pitch+(unsigned)ix]=fg;
   }
  }
  x+=(int)advance;
 }
}
static void icon(uint16_t*fb,unsigned pitch,int x,int y,const uint8_t*data,unsigned id,uint16_t fg){
 if(id>=8)return;
 const uint8_t*g=data+8+id*54u;
 for(unsigned yy=0;yy<18;yy++){
  unsigned row=(unsigned)g[yy*3]|((unsigned)g[yy*3+1]<<8)|((unsigned)g[yy*3+2]<<16);
  int ty=y+(int)yy;if(ty<0||ty>=240)continue;
  for(unsigned xx=0;xx<18;xx++)if(row&(1u<<xx)){
   int tx=x+(int)xx;if(tx>=0&&tx<320)fb[(unsigned)ty*pitch+(unsigned)tx]=fg;
  }
 }
}
static int background(uint16_t*fb,unsigned pitch,spmp_ui_read_at read,void*ctx,
                      uint8_t*scratch,size_t capacity,const spmp_ui_style*s){
 if(!read||!scratch||capacity<640||spmp_ui_validate_background(read,ctx)!=0){
  rect(fb,pitch,0,0,320,240,s->panel,16);return -1;
 }
 unsigned rows=(unsigned)mn((unsigned)(capacity/640),16);if(!rows)rows=1;
 unsigned y=0;
 while(y<240){unsigned chunk=mn(rows,240-y),n=chunk*640;
  if(read(ctx,16+y*640,scratch,n)!=(int)n){rect(fb,pitch,0,0,320,240,s->panel,16);return -1;}
  for(unsigned j=0;j<chunk;j++)for(unsigned x=0;x<320;x++){
   unsigned at=j*640+x*2;fb[(y+j)*pitch+x]=(uint16_t)u16(scratch+at);
  }
  y+=chunk;
 }
 return 0;
}
static void outline(uint16_t*fb,unsigned pitch,int x,int y,int w,int h,uint16_t c){
 rect(fb,pitch,x,y,w,1,c,16);rect(fb,pitch,x,y+h-1,w,1,c,16);
 rect(fb,pitch,x,y,1,h,c,16);rect(fb,pitch,x+w-1,y,1,h,c,16);
}
static void statusbar(uint16_t*fb,unsigned pitch,const spmp_ui_style*s,const uint8_t*font,const spmp_ui_status*status){
 char time[6]={'-','-',':','-','-',0};
 if(status->hour<24 && status->minute<60){unsigned hh=status->hour,mm=status->minute;
 time[0]=(char)('0'+hh/10);time[1]=(char)('0'+hh%10);
 time[3]=(char)('0'+mm/10);time[4]=(char)('0'+mm%10);}
 rect(fb,pitch,0,0,320,24,s->panel,15);rect(fb,pitch,0,23,320,1,s->accent,12);
 string(fb,pitch,8,4,font,time,s->text,58);
 /* Status contains ONLY real clock, volume, SD-card state. */
 /* volume icon and five-step fill: user volume only; no WiFi/battery. */
 rect(fb,pitch,224,9,3,6,s->muted,16);rect(fb,pitch,227,7,3,10,s->muted,16);
 unsigned levels=(mn(status->volume_percent,100u)+19u)/20u;
 for(unsigned i=0;i<5;i++)rect(fb,pitch,238+(int)i*7,16-(int)i*2,4,3+(int)i*2,
                                 i<levels?s->accent:s->border,16);
 string(fb,pitch,286,4,font,status->sd_present?"SD":"--",status->sd_present?s->text:s->muted,30);
}
static void draw_list(uint16_t*fb,unsigned pitch,const spmp_ui_style*s,const uint8_t*font,
                      const uint8_t*icons,const spmp_ui_item*items,unsigned count,const spmp_ui_state*state){
 rect(fb,pitch,0,26,150,189,s->panel,11);
 for(unsigned i=0;i<8;i++){
  unsigned k=(unsigned)state->scroll+i;if(k>=count)break;
  int y=30+(int)i*22;
  int selected=k==state->selected;
  if(selected){unsigned a=11u+((state->frame/3u)%4u);
   int ease=(int)mn(state->anim_frame,10u);
   int left=3+(10-ease)*2;
   rect(fb,pitch,left,y,146-left,21,s->selection,a);
   outline(fb,pitch,left,y,146-left,21,s->accent);
   rect(fb,pitch,4,y+2,2,17,s->accent,16);
  }else rect(fb,pitch,6,y+21,136,1,s->border,9);
  icon(fb,pitch,11,y+2,icons,items[k].icon,selected?s->text:s->muted);
  string(fb,pitch,37,y+2,font,items[k].label,selected?s->text:s->muted,100);
  if(selected)string(fb,pitch,132,y+2,font,">",s->accent,12);
 }
}
static void draw_grid(uint16_t*fb,unsigned pitch,const spmp_ui_style*s,const uint8_t*font,
                      const uint8_t*icons,const spmp_ui_item*items,unsigned count,const spmp_ui_state*state){
 unsigned first=state->scroll;
 for(unsigned k=first;k<count&&k<first+9;k++){
  unsigned loc=k-first,xx=loc%3,yy=loc/3;
  int x=10+(int)xx*104,y=30+(int)yy*61;
  int chosen=k==state->selected;
  rect(fb,pitch,x,y,92,55,s->panel,chosen?14:11);
  outline(fb,pitch,x,y,92,55,chosen?s->accent:s->border);
  if(chosen){rect(fb,pitch,x+1,y+1,90,3,s->accent,12);
   rect(fb,pitch,x+2,y+50,88,3,s->accent,11);}
  icon(fb,pitch,x+37,y+7,icons,items[k].icon,chosen?s->text:s->muted);
  string(fb,pitch,x+6,y+32,font,items[k].label,chosen?s->text:s->muted,81);
 }
}
int spmp_ui_render_overlay(uint16_t*fb,unsigned pitch,const spmp_ui_style*s,
                           const uint8_t*font,size_t fn,const uint8_t*icons,size_t in,
                           const spmp_ui_item*items,unsigned count,const spmp_ui_status*status,
                           const spmp_ui_state*state){
 if(!fb||pitch<320||!s||!font||!icons||!items||!status||!state||count>32||
    state->layout>1||spmp_ui_validate_font(font,fn)||spmp_ui_validate_icons(icons,in))return -1;
 statusbar(fb,pitch,s,font,status);
 if(state->layout==SPMP_UI_LAYOUT_LIST)draw_list(fb,pitch,s,font,icons,items,count,state);
 else draw_grid(fb,pitch,s,font,icons,items,count,state);
 rect(fb,pitch,0,216,320,24,s->panel,15);rect(fb,pitch,0,216,320,1,s->accent,12);
 string(fb,pitch,8,221,font,"A OK",s->text,67);
 string(fb,pitch,91,221,font,"B Назад",s->text,115);
 string(fb,pitch,228,221,font,"SEL Вид",s->text,88);
 return 0;
}
int spmp_ui_render(uint16_t*fb,unsigned pitch,spmp_ui_read_at read,void*ctx,
                   uint8_t*scratch,size_t cap,const spmp_ui_style*s,
                   const uint8_t*font,size_t fn,const uint8_t*icons,size_t in,
                   const spmp_ui_item*items,unsigned count,const spmp_ui_status*status,
                   const spmp_ui_state*state){
 if(!fb||pitch<320||!s||!font||!icons||!items||!status||!state||count>32||
    state->layout>1||spmp_ui_validate_font(font,fn)||spmp_ui_validate_icons(icons,in))return -1;
 int bg=background(fb,pitch,read,ctx,scratch,cap,s);
 if(spmp_ui_render_overlay(fb,pitch,s,font,fn,icons,in,items,count,status,state))return -1;
 return bg?1:0; /* positive: safe fallback palette rendered, e.g. SD removed */
}
size_t spmp_ui_sfx_generate(spmp_ui_sfx type,int16_t*out,size_t cap){
 if(!out||type==SPMP_UI_SFX_NONE)return 0;
 size_t n=cap<SPMP_UI_SOUND_FRAMES?cap:SPMP_UI_SOUND_FRAMES;
 unsigned p=0,rate=(type==SPMP_UI_SFX_ACCEPT?2800u:(type==SPMP_UI_SFX_BACK?1100u:1900u));
 for(size_t i=0;i<n;i++){
  p+=rate;unsigned ph=p%16000u;
  int wave=(int)(ph<8000u?ph:16000u-ph)-4000;
  int32_t amplitude=(int32_t)(n-i)*6500/(int32_t)SPMP_UI_SOUND_FRAMES;
  int32_t value=wave*amplitude/4000;
  if(value>32767)value=32767;
  if(value< -32768)value=-32768;
  out[i*2]=(int16_t)value;out[i*2+1]=(int16_t)value;
 }
 return n;
}
