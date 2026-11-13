#include "common.h"
/* A fixed-size screen renderer: no dynamic memory or GPU required. */
static const uint8_t letters[][7]={
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
 {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
 {31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {31,4,4,4,4,4,31},{7,2,2,2,18,18,12},
 {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
 {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
 {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},
 {17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},
 {14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14},
 {0,0,0,0,0,0,0},{0,0,0,31,0,0,0},
 {0,0,0,0,0,12,12},{0,0,0,0,12,0,0},
 {0,12,12,0,12,12,0},{1,1,2,4,8,16,16},
 {2,4,8,8,8,4,2},{8,4,2,2,2,4,8},
 {14,17,1,2,4,0,4},
};
static int glyph(char c){
 if(c>='a'&&c<='z')c-=32;
 if(c>='A'&&c<='Z')return c-'A';
 if(c>='0'&&c<='9')return 26+c-'0';
 switch(c){case ' ':return 36;case '-':return 37;case '.':return 38;
 case ',':return 39;case ':':return 40;case '/':return 41;
 case '(':return 42;case ')':return 43;default:return 44;}
}
static void rect(uint16_t *f,unsigned pitch,int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 if(w<=0||h<=0)return;
 for(int j=0;j<h;j++)for(int i=0;i<w;i++)f[(size_t)(y+j)*pitch+x+i]=c;
}
static void rounded(uint16_t *f,unsigned p,int x,int y,int w,int h,uint16_t color){
 rect(f,p,x+3,y,w-6,h,color);
 rect(f,p,x,y+3,3,h-6,color);
 rect(f,p,x+w-3,y+3,3,h-6,color);
 rect(f,p,x+1,y+1,2,h-2,color);
 rect(f,p,x+w-3,y+1,2,h-2,color);
}
static void label(uint16_t *f,unsigned p,int x,int y,const char *s,uint16_t c,int scale,unsigned limit){
 if(!s)return;
 for(unsigned i=0;i<limit&&s[i];i++){
  const uint8_t *g=letters[glyph(s[i])];
  for(unsigned yy=0;yy<7;yy++)for(unsigned xx=0;xx<5;xx++)
   if(g[yy]&(1u<<(4-xx)))rect(f,p,x+(int)i*6*scale+(int)xx*scale,
                                   y+(int)yy*scale,scale,scale,c);
 }
}
static void card_icon(uint16_t *f,unsigned p,int x,int y,unsigned type,uint16_t color){
 /* Icon silhouettes to minimize font and asset memory on ARM926EJ-S. */
 if(type==0){ /* music note */
  rect(f,p,x+13,y+3,3,15,color);rect(f,p,x+16,y+3,11,3,color);
  rounded(f,p,x+5,y+15,11,7,color);rounded(f,p,x+19,y+12,10,7,color);
  rect(f,p,x+25,y+4,3,12,color);
 }else if(type==1){
  rounded(f,p,x+4,y+3,27,23,color);rect(f,p,x+7,y+6,21,17,0);
  for(unsigned i=0;i<3;i++){rect(f,p,x+5+(int)i*10,y+3,3,3,color);rect(f,p,x+5+(int)i*10,y+23,3,3,color);}
  for(int yy=0;yy<9;yy++)rect(f,p,x+15+yy/2,y+9+yy,2,color==0?0:1,color);
 }else if(type==2){
  rounded(f,p,x+5,y+4,26,22,color);rect(f,p,x+7,y+6,22,18,0);
  rounded(f,p,x+21,y+8,5,5,color);
  for(int yy=0;yy<9;yy++)rect(f,p,x+8+yy,y+19-yy,11,2,color);
 }else if(type==3){
  rounded(f,p,x+5,y+4,26,22,color);
  rect(f,p,x+9,y+9,7,7,0);rect(f,p,x+18,y+9,7,7,0);
  rect(f,p,x+9,y+18,16,3,0);
 }else if(type==4){
  rect(f,p,x+5,y+9,26,18,color);rect(f,p,x+5,y+5,12,6,color);
  rect(f,p,x+8,y+13,20,11,0);
 }else{
  rounded(f,p,x+8,y+4,21,22,color);
  rect(f,p,x+11,y+8,15,3,0);rect(f,p,x+11,y+16,15,3,0);
 }
}
void mm_draw_home(uint16_t *f,unsigned pitch,const mm_skin *t,
                  unsigned selected,const char *status){
 if(!f||!t||pitch<SPMP_MM_W)return;
 static const char *titles[6]={"MUSIC","VIDEO","PHOTOS","THEMES","FILES","SETTINGS"};
 static const char *sub[6]={"LIBRARY","MOVIES","GALLERY","STYLES","STORAGE","OPTIONS"};
 for(unsigned y=0;y<240;y++)for(unsigned x=0;x<320;x++)f[(size_t)y*pitch+x]=t->bg;
 rect(f,pitch,0,0,320,45,t->panel);
 rect(f,pitch,0,44,320,1,t->border);
 rounded(f,pitch,12,10,27,27,t->accent);
 label(f,pitch,17,17,"S",t->bg,2,1);
 label(f,pitch,49,12,"SPMP-OS",t->text,2,7);
 label(f,pitch,50,31,"MULTIMEDIA EDITION",t->muted,1,22);
 rounded(f,pitch,275,12,33,19,t->panel_alt);
 label(f,pitch,281,18,"V1",t->accent,1,3);
 for(unsigned i=0;i<6;i++){
  int x=11+(int)(i%3)*102,y=56+(int)(i/3)*76;
  int active=(i==selected);
  rounded(f,pitch,x+1,y+2,95,68,t->border);
  rounded(f,pitch,x,y,95,68,active?t->active:t->panel);
  if(active){rect(f,pitch,x+9,y+3,76,2,t->accent);}
  rounded(f,pitch,x+9,y+10,34,31,active?t->accent:t->panel_alt);
  card_icon(f,pitch,x+8,y+11,i,active?t->bg:t->accent_alt);
  label(f,pitch,x+9,y+47,titles[i],active?t->text:t->text,1,11);
  label(f,pitch,x+9,y+58,sub[i],t->muted,1,11);
 }
 rect(f,pitch,0,211,320,29,t->panel);
 rect(f,pitch,0,211,320,1,t->border);
 rounded(f,pitch,10,217,8,8,t->accent);
 label(f,pitch,24,217,status?status:"A SELECT    B BACK    START MENU",t->muted,1,46);
}
