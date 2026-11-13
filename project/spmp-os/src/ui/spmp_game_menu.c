#include "spmp_game_menu.h"
#include <string.h>
#include <stdint.h>

#define THUMB_BYTES (SPMP_UI_THUMB_PIXELS*2u)
#define MAGIC "SPMPSAV1"
#define HEADER SPMP_UI_STATE_HEADER

static uint64_t hash_id(const char *s){
 uint64_t h=UINT64_C(14695981039346656037);
 if(!s)return 0;
 while(*s){h^=(uint8_t)*s++;h*=UINT64_C(1099511628211);}
 return h;
}
static uint32_t checksum(const uint8_t *p,size_t n){
 uint32_t h=2166136261u;
 for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}
 return h;
}
static void put32(uint8_t *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void put64(uint8_t *p,uint64_t n){for(unsigned i=0;i<8;i++)p[i]=(uint8_t)(n>>(8*i));}
static uint64_t get64(const uint8_t *p){uint64_t r=0;for(unsigned i=0;i<8;i++)r|=(uint64_t)p[i]<<(8*i);return r;}
static int settings_read(void *ctx,const char *path,uint8_t*buf,size_t cap,size_t *actual){
 spmp_game_menu*m=ctx;
 return m->storage.load?m->storage.load(m->storage.user,path,buf,cap,actual):-1;
}
static int settings_save(void *ctx,const char *path,const uint8_t*data,size_t size){
 spmp_game_menu*m=ctx;
 return m->storage.store_atomic?m->storage.store_atomic(m->storage.user,path,data,size):-1;
}
static int settings_apply(void *ctx,spmp_setting_id id,int value){
 spmp_game_menu*m=ctx;
 return m->emulator.apply_setting?m->emulator.apply_setting(m->emulator.user,id,value):1;
}
static void copy_label(char *out,size_t cap,const char *src){
 if(!cap)return;
 size_t i=0;if(src)while(i+1<cap&&src[i]){out[i]=src[i];i++;}out[i]=0;
}
static int same(const uint8_t *p,const char *s,size_t n){return memcmp(p,s,n)==0;}
void spmp_menu_init(spmp_game_menu *m,const char *core_id,const char *rom_id,
 const spmp_ui_storage *storage,const spmp_ui_emulator *emu,
 uint8_t *scratch,size_t scratch_size){
 if(!m)return;
 memset(m,0,sizeof(*m));
 if(storage)m->storage=*storage;
 if(emu)m->emulator=*emu;
 m->scratch=scratch;m->scratch_size=scratch_size;
 m->core_hash=hash_id(core_id);m->rom_hash=hash_id(rom_id);
 copy_label(m->core_id,sizeof m->core_id,core_id);
 copy_label(m->rom_id,sizeof m->rom_id,rom_id);
 spmp_settings_io io={m,settings_read,settings_save,settings_apply};
 if(!spmp_settings_init(&m->settings,core_id,&io))
  (void)spmp_settings_read(&m->settings);

}
int spmp_menu_capture_clean(spmp_game_menu *m,const uint16_t *f,unsigned w,unsigned h,size_t pitch){
 if(!m||!f||!w||!h||w>4096||h>4096||pitch<(size_t)w*2||pitch&1u||m->opened)return -1;
 /* Keep aspect ratio; pad black in the thumbnail.  No UI layers involved. */
 unsigned out_w=SPMP_UI_THUMB_W,out_h=SPMP_UI_THUMB_H;
 if((uint64_t)w*out_h>(uint64_t)h*out_w)out_h=(unsigned)((uint64_t)h*out_w/w);
 else out_w=(unsigned)((uint64_t)w*out_h/h);
 if(!out_w)out_w=1;
 if(!out_h)out_h=1;
 unsigned x0=(SPMP_UI_THUMB_W-out_w)/2,y0=(SPMP_UI_THUMB_H-out_h)/2;
 memset(m->clean_frame,0,sizeof m->clean_frame);
 for(unsigned y=0;y<out_h;y++){
  unsigned sy=(unsigned)((uint64_t)y*h/out_h);
  const uint16_t *line=(const uint16_t*)((const uint8_t*)f+(size_t)sy*pitch);
  for(unsigned x=0;x<out_w;x++){
   unsigned sx=(unsigned)((uint64_t)x*w/out_w);
   m->clean_frame[(y0+y)*SPMP_UI_THUMB_W+x0+x]=line[sx];
  }
 }
 m->clean_valid=1;return 0;
}
static int append_char(char **p,size_t *room,char c){
 if(*room<=1)return -1;
 *(*p)++=c;
 (*room)--;
 **p=0;
 return 0;
}
int spmp_menu_save_path(const spmp_game_menu *m,char *dst,size_t cap){
 if(!m||!dst||cap<30||!m->core_hash||!m->rom_hash||m->slot>=SPMP_UI_SLOTS)return -1;
 char *p=dst;size_t remain=cap;*p=0;
 const char *prefix="saves/";
 while(*prefix)if(append_char(&p,&remain,*prefix++))return -1;
 for(size_t i=0;m->core_id[i];i++){
  char c=m->core_id[i];
  if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'||c=='_'))c='_';
  if(append_char(&p,&remain,c))return -1;
 }
 if(append_char(&p,&remain,'/'))return -1;
 for(int i=15;i>=0;i--){
  static const char hex[]="0123456789abcdef";
  if(append_char(&p,&remain,hex[(m->rom_hash>>(4*i))&15u]))return -1;
 }
 if(append_char(&p,&remain,'.')||append_char(&p,&remain,'s')||append_char(&p,&remain,'0'+(char)m->slot))return -1;
 for(const char *q=".sps";*q;q++)if(append_char(&p,&remain,*q))return -1;
 return 0;
}
static int load_valid(spmp_game_menu *m,size_t *state_n){
 if(!m||!m->scratch||!m->storage.load||m->scratch_size<HEADER+THUMB_BYTES)return -1;
 char path[SPMP_UI_PATH_MAX];if(spmp_menu_save_path(m,path,sizeof(path)))return -1;
 size_t got=0;
 if(m->storage.load(m->storage.user,path,m->scratch,m->scratch_size,&got))return -1;
 const uint8_t *p=m->scratch;
 if(got<HEADER+THUMB_BYTES||!same(p,MAGIC,8)||get32(p+8)!=2||get32(p+12)!=m->slot||
    p[16]!=SPMP_UI_THUMB_W||p[17]!=0||p[18]!=SPMP_UI_THUMB_H||p[19]!=0||
    get64(p+24)!=m->core_hash||get64(p+32)!=m->rom_hash)return -1;
 uint32_t len=get32(p+20);
 if(len>SPMP_UI_STATE_MAX||(size_t)len!=got-HEADER-THUMB_BYTES)return -1;
 if(get32(p+44)!=checksum(p+HEADER,THUMB_BYTES)||
    get32(p+40)!=checksum(p+HEADER,THUMB_BYTES+len))return -1;
 if(state_n)*state_n=len;
 return 0;
}
int spmp_menu_save(spmp_game_menu *m){
 if(!m||!m->clean_valid||!m->scratch||!m->storage.store_atomic||
    !m->emulator.state_size||!m->emulator.serialize){if(m)m->last_result=-1;return -1;}
 size_t n=m->emulator.state_size(m->emulator.user);
 if(!n||n>SPMP_UI_STATE_MAX||m->scratch_size<HEADER+THUMB_BYTES||
    n>m->scratch_size-HEADER-THUMB_BYTES){m->last_result=-1;return -1;}
 uint8_t *p=m->scratch;
 memset(p,0,HEADER);
 memcpy(p,MAGIC,8);put32(p+8,2);put32(p+12,m->slot);
 p[16]=SPMP_UI_THUMB_W;p[18]=SPMP_UI_THUMB_H;
 put32(p+20,(uint32_t)n);put64(p+24,m->core_hash);put64(p+32,m->rom_hash);
 for(size_t i=0;i<SPMP_UI_THUMB_PIXELS;i++){
  uint16_t px=m->clean_frame[i];p[HEADER+i*2]=(uint8_t)px;p[HEADER+i*2+1]=(uint8_t)(px>>8);
 }
 if(m->emulator.serialize(m->emulator.user,p+HEADER+THUMB_BYTES,n)){
  m->last_result=-1;return -1;
 }
 put32(p+44,checksum(p+HEADER,THUMB_BYTES));
 put32(p+40,checksum(p+HEADER,THUMB_BYTES+n));
 char path[SPMP_UI_PATH_MAX];if(spmp_menu_save_path(m,path,sizeof path)||
    m->storage.store_atomic(m->storage.user,path,p,HEADER+THUMB_BYTES+n)){
  m->last_result=-1;return -1;
 }
 memcpy(m->preview,m->clean_frame,sizeof m->preview);
 m->preview_valid=1;m->last_result=0;return 0;
}
int spmp_menu_refresh_preview(spmp_game_menu *m){
 if(!m)return -1;
 m->preview_valid=0;
 if(m->storage.load_prefix){
  if(!m->scratch||m->scratch_size<HEADER+THUMB_BYTES)return -1;
  char path[SPMP_UI_PATH_MAX];
  if(spmp_menu_save_path(m,path,sizeof path)||
     m->storage.load_prefix(m->storage.user,path,m->scratch,HEADER+THUMB_BYTES))return -1;
  const uint8_t *h=m->scratch;
  if(!same(h,MAGIC,8)||get32(h+8)!=2||get32(h+12)!=m->slot||
     h[16]!=SPMP_UI_THUMB_W||h[17]!=0||h[18]!=SPMP_UI_THUMB_H||h[19]!=0||
     get64(h+24)!=m->core_hash||get64(h+32)!=m->rom_hash||
     get32(h+20)>SPMP_UI_STATE_MAX||
     get32(h+44)!=checksum(h+HEADER,THUMB_BYTES))return -1;
 }else if(load_valid(m,NULL))return -1;
 const uint8_t *p=m->scratch+HEADER;
 for(size_t i=0;i<SPMP_UI_THUMB_PIXELS;i++)m->preview[i]=(uint16_t)p[i*2]|((uint16_t)p[i*2+1]<<8);
 m->preview_valid=1;return 0;
}
int spmp_menu_load(spmp_game_menu *m){
 size_t state_n=0;
 if(!m||!m->emulator.unserialize||load_valid(m,&state_n)){
  if(m)m->last_result=-1;
  return -1;
 }
 /* Core owns validation and apply; stay paused if it rejects. */
 if(m->emulator.unserialize(m->emulator.user,
    m->scratch+HEADER+THUMB_BYTES,state_n)){
  m->last_result=-1;return -1;
 }
 const uint8_t *p=m->scratch+HEADER;
 for(size_t i=0;i<SPMP_UI_THUMB_PIXELS;i++)m->preview[i]=(uint16_t)p[i*2]|((uint16_t)p[i*2+1]<<8);
 m->preview_valid=1;m->last_result=0;return 0;
}
static void resume(spmp_game_menu *m){m->settings_open=0;m->opened=0;if(m->emulator.resume)m->emulator.resume(m->emulator.user);}
int spmp_menu_input(spmp_game_menu *m,uint32_t keys,uint32_t ms){
 if(!m)return -1;
 uint32_t down=keys&~m->previous_buttons;
 int tap=0;
 if(keys&SPMP_UI_POWER){
  if(!m->power_was_down){m->power_was_down=1;m->power_down_at=ms;m->held_long=0;}
  if((uint32_t)(ms-m->power_down_at)>650u)m->held_long=1;
 }else if(m->power_was_down){
  uint32_t elapsed=(uint32_t)(ms-m->power_down_at);
  tap=!m->held_long&&elapsed>=30u&&elapsed<=650u;
  m->power_was_down=0;
 }
 m->previous_buttons=keys;
 if(tap){
  if(m->opened)resume(m);
  else{m->opened=1;m->selection=SPMP_UI_CONTINUE;
       if(m->emulator.pause)m->emulator.pause(m->emulator.user);
       (void)spmp_menu_refresh_preview(m);}
  return 1;
 }
 if(!m->opened)return 0;
 if(m->settings_open){
  if(down&SPMP_UI_BACK){m->settings_open=0;return 1;}
  if(down&SPMP_UI_UP)m->settings_selection=(m->settings_selection+m->settings.count-1u)%m->settings.count;
  if(down&SPMP_UI_DOWN)m->settings_selection=(m->settings_selection+1u)%m->settings.count;
  int delta=(down&SPMP_UI_RIGHT)?1:((down&SPMP_UI_LEFT)?-1:((down&SPMP_UI_CONFIRM)?1:0));
  if(delta && spmp_settings_change(&m->settings,m->settings_selection,delta)){
   m->last_result=-1;return -1;
  }
  return 0;
 }
 if(down&SPMP_UI_BACK){resume(m);return 1;}
 if(down&SPMP_UI_UP)m->selection=(m->selection+SPMP_UI_ITEMS-1u)%SPMP_UI_ITEMS;
 if(down&SPMP_UI_DOWN)m->selection=(m->selection+1u)%SPMP_UI_ITEMS;
 if((down&(SPMP_UI_LEFT|SPMP_UI_RIGHT))){
  if(down&SPMP_UI_RIGHT)m->slot=(m->slot+1u)%SPMP_UI_SLOTS;
  else m->slot=(m->slot+SPMP_UI_SLOTS-1u)%SPMP_UI_SLOTS;
  (void)spmp_menu_refresh_preview(m);
 }
 if(!(down&SPMP_UI_CONFIRM))return 0;
 switch(m->selection){
 case SPMP_UI_CONTINUE:resume(m);return 1;
 case SPMP_UI_SAVE:return spmp_menu_save(m)?-1:2;
 case SPMP_UI_LOAD:return spmp_menu_load(m)?-1:3;
 case SPMP_UI_SLOT:m->slot=(m->slot+1u)%SPMP_UI_SLOTS;(void)spmp_menu_refresh_preview(m);return 0;
 case SPMP_UI_SETTINGS:m->settings_open=1;m->settings_selection=0;return 1;
 case SPMP_UI_EXIT:
  if(m->emulator.exit_to_library&&m->emulator.exit_to_library(m->emulator.user)){
   m->last_result=-1;return -1;
  }
  m->quit=1;m->opened=0;return 4;
 default:return 0;
 }
}
