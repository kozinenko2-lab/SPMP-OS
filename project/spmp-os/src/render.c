#include "defos.h"
#include "spmp_apps.h"
#include <stdint.h>
#include <string.h>

// Original tiny 5x7 bitmap typeface. Each row is the low five bits.
typedef struct {char c;uint8_t row[7];} glyph;
static const glyph font[] = {
 {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}},
 {'C',{14,17,16,16,16,17,14}}, {'D',{30,17,17,17,17,17,30}},
 {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}},
 {'G',{14,17,16,23,17,17,15}}, {'H',{17,17,17,31,17,17,17}},
 {'I',{31,4,4,4,4,4,31}}, {'J',{7,2,2,2,18,18,12}},
 {'K',{17,18,20,24,20,18,17}}, {'L',{16,16,16,16,16,16,31}},
 {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}},
 {'O',{14,17,17,17,17,17,14}}, {'P',{30,17,17,30,16,16,16}},
 {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}},
 {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}},
 {'U',{17,17,17,17,17,17,14}}, {'V',{17,17,17,17,17,10,4}},
 {'W',{17,17,17,21,21,21,10}}, {'X',{17,17,10,4,10,17,17}},
 {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
 {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}},
 {'2',{14,17,1,2,4,8,31}}, {'3',{30,1,1,14,1,1,30}},
 {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
 {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}},
 {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}},
 {' ',{0,0,0,0,0,0,0}}, {'-',{0,0,0,31,0,0,0}},
 {'.',{0,0,0,0,0,12,12}}, {':',{0,12,12,0,12,12,0}},
 {'/',{1,1,2,4,8,16,16}}, {'_',{0,0,0,0,0,0,31}},
 {'[',{14,8,8,8,8,8,14}}, {']',{14,2,2,2,2,2,14}},
 {'>',{16,8,4,2,4,8,16}}, {'?',{14,17,1,2,4,0,4}},
 {'+',{0,4,4,31,4,4,0}}, {'=',{0,0,31,0,31,0,0}},
 {'(',{2,4,8,8,8,4,2}}, {')',{8,4,2,2,2,4,8}},
};

static void fill(uint16_t *p,int x,int y,int w,int h,uint16_t c) {
  if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
  if(x+w>DEFOS_SCREEN_W)w=DEFOS_SCREEN_W-x;
  if(y+h>DEFOS_SCREEN_H)h=DEFOS_SCREEN_H-y;
  if(w<1||h<1)return;
  for(int j=y;j<y+h;++j)for(int i=x;i<x+w;++i)p[j*DEFOS_SCREEN_W+i]=c;
}
static const glyph *find_glyph(char c) {
 if(c>='a'&&c<='z')c-=32;
 for(size_t i=0;i<sizeof font/sizeof font[0];++i)if(font[i].c==c)return &font[i];
 return &font[45]; // '?' glyph
}
static void text(uint16_t *p,int x,int y,const char *str,uint16_t color,int scale,int maxchars) {
 for(int i=0;str[i] && i<maxchars;++i){
  const glyph *g=find_glyph(str[i]);
  for(int r=0;r<7;r++)for(int c=0;c<5;c++)if(g->row[r]&(1u<<(4-c)))
   fill(p,x+i*6*scale+c*scale,y+r*scale,scale,scale,color);
 }
}
static void cut(char *out,size_t cap,const char *in) {
 size_t n=strlen(in);if(n>=cap)n=cap-1;
 memcpy(out,in,n);out[n]=0;
}
void defos_render(const defos_model *m,uint16_t *p) {
 if(!p||!m)return;
 uint16_t BG=m->theme.background, PANEL=m->theme.panel,ACCENT=m->theme.accent,
  TEXT=m->theme.text,DIM=m->theme.muted,SELECT=m->theme.selection,BORDER=m->theme.border;
 fill(p,0,0,320,240,BG);
 fill(p,0,0,320,37,PANEL);
 fill(p,0,35,320,2,ACCENT);
 text(p,10,9,"SPMP-OS",ACCENT,2,14);
 text(p,213,15,"STAGE 2",DIM,1,17);
 fill(p,8,44,304,158,PANEL);
 text(p,16,51,"FILE MANAGER",TEXT,1,28);
 char path[60];path[0]='/';
 size_t plen=strlen(m->relative);if(plen>57u)plen=57u;
 memcpy(path+1,m->relative,plen);path[1+plen]=0;
 text(p,17,64,path,DIM,1,45);
 fill(p,16,75,288,1,BORDER);
 for(size_t i=0;i<12 && i+m->scroll<m->count;i++){
  size_t idx=i+m->scroll;const defos_entry *e=&m->entries[idx];
  int y=79+(int)i*10;
  if(idx==m->selected)fill(p,13,y-2,292,10,SELECT);
  text(p,17,y,e->kind==DEFOS_FILE_DIR?">":" ",ACCENT,1,1);
  char name[36];cut(name,sizeof name,e->name);
  text(p,26,y,name,idx==m->selected?TEXT:DIM,1,31);
  text(p,252,y,defos_kind_name(e->kind),idx==m->selected?TEXT:ACCENT,1,8);
 }
 fill(p,0,208,320,32,PANEL);
 text(p,9,215,"UP/DOWN SELECT   A OPEN   B BACK",TEXT,1,49);
 text(p,9,226,m->message,DIM,1,49);
}

/* 320x240 testable home menu, with no target-platform side effects. */
void spmp_render_home(const spmp_theme *t,uint16_t *p) {
 if(!t||!p)return;
 fill(p,0,0,320,240,t->background);
 fill(p,0,0,320,39,t->panel);
 text(p,12,10,"SPMP-OS",t->accent,2,12);
 text(p,196,16,"HOME       ",t->muted,1,23);
 fill(p,0,37,320,2,t->accent);
 size_t n;const spmp_app *apps=spmp_get_apps(&n);
 for(size_t i=0;i<n && i<9;i++){
  int x=10+(int)(i%3)*102,y=48+(int)(i/3)*49;
  fill(p,x,y,96,44,t->panel);
  fill(p,x,y,96,2,i==0?t->accent:t->border);
  text(p,x+9,y+9,apps[i].label,t->text,1,14);
  text(p,x+9,y+23,apps[i].status==SPMP_HOST_PROTOTYPE?"PROTOTYPE":"PLANNED",t->muted,1,14);
 }
 fill(p,0,203,320,37,t->panel);
 text(p,11,209,"THEMES  BUTTONS  MEDIA  GAMES",t->accent,1,50);
 text(p,11,224,"HOST PREVIEW - NOT DEVICE FIRMWARE",t->muted,1,51);
}

/* Shared emulator menu, drawn AFTER clean-frame capture. Never overwrite
 * spmp_game_menu::clean_frame while the overlay is visible. */
#include "spmp_game_menu.h"
void spmp_menu_draw(const spmp_game_menu *m,const spmp_theme *t,
                    uint16_t *p,unsigned w,unsigned h){
 if(!m||!t||!p||!m->opened||w!=320||h!=240)return;
 static const char *items[]={"CONTINUE","SAVE STATE","LOAD STATE","SELECT SLOT","SETTINGS","EXIT GAME"};
 fill(p,0,0,320,240,t->background);
 fill(p,0,0,320,35,t->panel);
 fill(p,0,33,320,2,t->accent);
 text(p,12,10,"SPMP-OS",t->accent,2,15);
 text(p,178,15,"GAME MENU",t->text,1,16);
 fill(p,9,43,169,174,t->panel);
 if(m->settings_open){
  fill(p,9,43,303,174,t->panel);
  text(p,17,48,"CORE SETTINGS",t->accent,1,32);
  text(p,181,48,m->core_id,t->muted,1,21);
  for(unsigned i=0;i<m->settings.count;i++){
   unsigned y=67+i*18;
   if(i==m->settings_selection)fill(p,13,(int)y-3,293,16,t->selection);
   text(p,19,(int)y,m->settings.defs[i].label,i==m->settings_selection?t->accent:t->text,1,21);
   char value[20];spmp_settings_value(&m->settings,i,value,sizeof value);
   text(p,238,(int)y,value,t->text,1,11);
  }
  text(p,15,207,"LEFT/RIGHT CHANGE   B BACK",t->muted,1,50);
  fill(p,0,222,320,18,t->panel);
  text(p,12,228,"SOME OPTIONS NEED CORE ADAPTER",t->muted,1,50);
  return;
 }
 int can_state=m->emulator.state_size&&m->emulator.state_size(m->emulator.user)>0;
 for(unsigned i=0;i<SPMP_UI_ITEMS;i++){
  int y=52+(int)i*26;
  if(i==m->selection)fill(p,12,y-4,163,23,t->selection);
  uint16_t c=(i==SPMP_UI_SAVE||i==SPMP_UI_LOAD)&&!can_state?t->muted:
             i==m->selection?t->accent:t->text;
  text(p,20,y,items[i],c,1,24);
 }
 text(p,19,210,"A SELECT  B BACK",t->muted,1,29);
 fill(p,187,43,126,160,t->panel);
 text(p,196,52,"SLOT",t->text,1,10);
 char slot[2]={(char)('1'+m->slot),0};text(p,231,52,slot,t->accent,1,1);
 fill(p,191,68,120,92,t->border);
 if(m->preview_valid){
  for(unsigned y=0;y<SPMP_UI_THUMB_H;y++){
   for(unsigned x=0;x<SPMP_UI_THUMB_W;x++){
    if(x+192u<320u && y+69u<240u)p[(y+69u)*320u+x+192u]=m->preview[y*SPMP_UI_THUMB_W+x];
   }
  }
 }else text(p,202,111,"EMPTY SLOT",t->muted,1,16);
 if(m->last_result<0)text(p,192,177,"SAVE/LOAD ERR",t->accent,1,18);
 else if(!can_state)text(p,194,177,"STATE N/A",t->muted,1,18);
 else text(p,193,177,"< > CHANGE",t->muted,1,18);
 text(p,194,191,"1-5 SLOTS",t->muted,1,18);
 fill(p,0,222,320,18,t->panel);
 text(p,10,228,"SHORT POWER: MENU  /  LONG POWER: SYSTEM",t->muted,1,49);
}
