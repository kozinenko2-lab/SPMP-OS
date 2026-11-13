#include "spmp_settings.h"

#include <string.h>

static void copy_value(char *dst,size_t cap,const char*src){
 if(!cap)return;
 size_t i=0;
 while(src[i]&&i+1<cap){dst[i]=src[i];i++;}
 dst[i]=0;
}
static void number_value(char *dst,size_t cap,int value,int suffix){
 char b[8];unsigned v=(unsigned)value;
 if(v>=10u){b[0]=(char)('0'+v/10u);b[1]=(char)('0'+v%10u);b[2]=0;}
 else {b[0]=(char)('0'+v);b[1]=0;}
 if(suffix){size_t n=strlen(b);b[n++]='/';b[n++]='1';b[n++]='0';b[n]=0;}
 copy_value(dst,cap,b);
}
static uint32_t fnv(const uint8_t *p,size_t n){
 uint32_t h=2166136261u;
 for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}return h;
}
static void put32(uint8_t *p,uint32_t x){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(x>>(8*i));}
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void add(spmp_settings *s,spmp_setting_id id,const char *label,int lo,int hi,int val,unsigned deferred){
 if(s->count>=SPMP_SETTINGS_MAX)return;
 unsigned i=s->count++;
 s->defs[i]=(spmp_setting_def){id,label,lo,hi,val,deferred};s->values[i]=val;
}
int spmp_settings_init(spmp_settings *s,const char *core_id,const spmp_settings_io *io){
 if(!s||!core_id||!core_id[0])return -1;
 memset(s,0,sizeof *s);if(io)s->io=*io;
 size_t n=strlen(core_id);if(n>30)return -1;
 s->core_hash=fnv((const uint8_t*)core_id,n);
 char safe[32];
 for(size_t i=0;i<n;i++){
  char c=core_id[i];
  safe[i]=((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-')?c:'_';
 }
 safe[n]=0;
 const char *prefix="settings/";
 size_t at=0;
 for(size_t i=0;prefix[i];i++)s->path[at++]=prefix[i];
 for(size_t i=0;i<n;i++)s->path[at++]=safe[i];
 s->path[at++]='-';
 static const char hexdigits[]="0123456789abcdef";
 for(int i=7;i>=0;i--)s->path[at++]=hexdigits[(s->core_hash>>(4u*(unsigned)i))&15u];
 s->path[at++]='.';s->path[at++]='c';s->path[at++]='f';s->path[at++]='g';
 s->path[at]=0;
 add(s,SPMP_OPT_SCALE,"SCREEN SIZE",0,2,1,1); /* applied by platform compositor */
 add(s,SPMP_OPT_VOLUME,"VOLUME",0,10,8,0); /* router PCM gain */
 add(s,SPMP_OPT_SWAP_AB,"SWAP A/B",0,1,0,0); /* router input mapping */
 add(s,SPMP_OPT_FRAMESKIP,"FRAMESKIP",0,3,0,1); /* scheduler hook pending */
 if(!strcmp(core_id,"gb")){
  add(s,SPMP_OPT_GB_PALETTE,"GB PALETTE",0,3,0,1);
  add(s,SPMP_OPT_CPU_ACCURACY,"CPU ACCURACY",0,1,1,1);
 }else if(!strcmp(core_id,"sms")){
  add(s,SPMP_OPT_REGION,"TV REGION",0,2,0,1);
  add(s,SPMP_OPT_GG_BORDER,"GG BORDER",0,1,0,1);
 }else if(!strcmp(core_id,"nes")){
  add(s,SPMP_OPT_SPRITE_LIMIT,"SPRITE LIMIT",0,1,0,1);
  add(s,SPMP_OPT_REGION,"TV REGION",0,2,0,1);
 }else if(!strcmp(core_id,"atari2600")){
  add(s,SPMP_OPT_TIA_PALETTE,"TIA PALETTE",0,2,0,1);
 }else if(!strcmp(core_id,"spectrum")){
  add(s,SPMP_OPT_MACHINE_MODEL,"ZX MODEL",0,2,0,1);
 }else if(!strcmp(core_id,"gba")){
  add(s,SPMP_OPT_USE_BIOS,"EXTERNAL BIOS",0,1,0,1);
 }else if(!strcmp(core_id,"ps1")){
  add(s,SPMP_OPT_CPU_ACCURACY,"CPU ACCURACY",0,1,1,1);
 }else if(!strcmp(core_id,"c64")){
  add(s,SPMP_OPT_CPU_ACCURACY,"CPU ACCURACY",0,1,1,1);
 }else if(!strcmp(core_id,"snes")||!strcmp(core_id,"megadrive")||!strcmp(core_id,"pcengine")){
  add(s,SPMP_OPT_REGION,"TV REGION",0,2,0,1);
 }
 return 0;
}
int spmp_settings_read(spmp_settings *s){
 if(!s)return -1;
 if(!s->io.load)return 1; /* defaults, not an error */
 uint8_t b[SPMP_SETTINGS_DISK_BYTES];size_t len=0;
 if(s->io.load(s->io.ctx,s->path,b,sizeof b,&len))return 1;
 if(len!=sizeof b||memcmp(b,"SPMPCFG1",8)||get32(b+8)!=1||get32(b+12)!=s->core_hash||
    get32(b+16)>SPMP_SETTINGS_MAX||get32(b+124)!=fnv(b,124))return -1;
 unsigned count=get32(b+16);
 /* Validate entire structure before touching live values. Ignore unknown IDs. */
 for(unsigned k=0;k<count;k++){
  uint32_t id=get32(b+20+k*8), val=get32(b+24+k*8);
  for(unsigned j=0;j<s->count;j++)if(id==(unsigned)s->defs[j].id &&
     (val<(unsigned)s->defs[j].min_value||val>(unsigned)s->defs[j].max_value))return -1;
 }
 for(unsigned k=0;k<count;k++){
  uint32_t id=get32(b+20+k*8), val=get32(b+24+k*8);
  for(unsigned j=0;j<s->count;j++)if(id==(unsigned)s->defs[j].id)s->values[j]=(int)val;
 }
 for(unsigned j=0;j<s->count;j++)if(s->io.apply){
  int r=s->io.apply(s->io.ctx,s->defs[j].id,s->values[j]);
  if(r!=0)s->dirty=1;
 }
 return 0;
}
int spmp_settings_write(spmp_settings *s){
 if(!s||!s->io.save_atomic)return -1;
 uint8_t b[SPMP_SETTINGS_DISK_BYTES]={0};
 memcpy(b,"SPMPCFG1",8);put32(b+8,1);put32(b+12,s->core_hash);put32(b+16,s->count);
 for(unsigned i=0;i<s->count;i++){
  put32(b+20+i*8,(uint32_t)s->defs[i].id);
  put32(b+24+i*8,(uint32_t)s->values[i]);
 }
 put32(b+124,fnv(b,124));
 return s->io.save_atomic(s->io.ctx,s->path,b,sizeof b);
}
int spmp_settings_change(spmp_settings *s,unsigned index,int delta){
 if(!s||index>=s->count||!(delta==1||delta==-1))return -1;
 spmp_setting_def*d=&s->defs[index];int old=s->values[index];
 int val=old+delta;if(val>d->max_value)val=d->min_value;
 if(val<d->min_value)val=d->max_value;
 if(val==old)return 0;
 if(s->io.apply){int r=s->io.apply(s->io.ctx,d->id,val);if(r<0)return -1;if(r>0)s->dirty=1;}
 else if(d->deferred)s->dirty=1;
 s->values[index]=val;
 if(s->io.save_atomic && spmp_settings_write(s)){
  s->values[index]=old;
  if(s->io.apply)(void)s->io.apply(s->io.ctx,d->id,old);
  return -1;
 }
 if(!s->io.save_atomic)s->dirty=1;
 return 0;
}
int spmp_settings_get(const spmp_settings*s,spmp_setting_id id,int fallback){
 if(!s)return fallback;
 for(unsigned i=0;i<s->count;i++)if(s->defs[i].id==id)return s->values[i];
 return fallback;
}
const char *spmp_settings_value(const spmp_settings*s,unsigned i,char *out,size_t cap){
 if(!out||!cap)return "";
 *out=0;if(!s||i>=s->count)return out;
 int v=s->values[i];
 switch(s->defs[i].id){
 case SPMP_OPT_SCALE:
  copy_value(out,cap,(const char*[]){"NATIVE","FIT","FULL"}[v]);break;
 case SPMP_OPT_VOLUME:number_value(out,cap,v,1);break;
 case SPMP_OPT_SWAP_AB:case SPMP_OPT_SPRITE_LIMIT:case SPMP_OPT_GG_BORDER:
  copy_value(out,cap,v?"ON":"OFF");break;
 case SPMP_OPT_REGION:
  copy_value(out,cap,(const char*[]){"AUTO","NTSC","PAL"}[v]);break;
 case SPMP_OPT_GB_PALETTE:
  copy_value(out,cap,(const char*[]){"PAL 0","PAL 1","PAL 2","PAL 3"}[v]);break;
 case SPMP_OPT_CPU_ACCURACY:copy_value(out,cap,v?"NORMAL":"FAST");break;
 case SPMP_OPT_TIA_PALETTE:
  copy_value(out,cap,(const char*[]){"NTSC","PAL","SECAM"}[v]);break;
 case SPMP_OPT_MACHINE_MODEL:
  copy_value(out,cap,(const char*[]){"48K","128K","PLUS 2A"}[v]);break;
 case SPMP_OPT_USE_BIOS:
  copy_value(out,cap,v?"ON":"OFF");break;
 case SPMP_OPT_FRAMESKIP:
  copy_value(out,cap,v==0?"OFF":v==1?"AUTO":v==2?"1":"2");break;
 default:number_value(out,cap,v,0);break;
 }
 return out;
}
