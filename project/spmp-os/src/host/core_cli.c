#include "spmp_cores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {const char *output;unsigned frames;} cli_context;
static int write_ppm(const char *filename,const uint16_t *pix,unsigned w,unsigned h){
 FILE *f=fopen(filename,"wb");if(!f)return -1;
 if(fprintf(f,"P6\n%u %u\n255\n",w,h)<0){fclose(f);return -1;}
 for(size_t i=0;i<(size_t)w*h;i++){
  unsigned pixel=pix[i];
  unsigned char rgb[3]={
   (unsigned char)((((pixel>>11)&31u)*255+15)/31),
   (unsigned char)((((pixel>>5)&63u)*255+31)/63),
   (unsigned char)(((pixel&31u)*255+15)/31)};
  if(fwrite(rgb,1,3,f)!=3){fclose(f);return -1;}
 }
 return fclose(f)==0?0:-1;
}
static void host_present(void *user,const uint16_t *pixels,unsigned w,unsigned h){
 cli_context *ctx=(cli_context *)user;
 ctx->frames++;
 if(ctx->output&&write_ppm(ctx->output,pixels,w,h)){
  fprintf(stderr,"Failed to write frame: %s\n",ctx->output);
  ctx->frames=0;
 }
}
static uint32_t host_buttons(void *p){(void)p;return 0;}
static int host_read_entire(void *user,const char *path,uint8_t *dst,
                            size_t cap,size_t *length){
 (void)user;
 if(!path||!dst||!length)return -1;
 FILE *f=fopen(path,"rb");if(!f)return -1;
 size_t n=fread(dst,1,cap,f);
 int extra=fgetc(f),bad=ferror(f);
 if(fclose(f)!=0)bad=1;
 if(extra!=EOF||bad)return -1;
 *length=n;
 return 0;
}
static int host_file_size(void *user,const char *path,size_t *bytes){
 (void)user;
 if(!path||!bytes)return -1;
 struct stat st;
 if(stat(path,&st)||!S_ISREG(st.st_mode)||st.st_size<=0||
    (uintmax_t)st.st_size>(uintmax_t)SIZE_MAX)return -1;
 *bytes=(size_t)st.st_size;
 return 0;
}
static const char *state_label(spmp_core_state state){
 switch(state){
 case SPMP_CORE_HOST_TESTED:return "HOST_TESTED";
 case SPMP_CORE_SOURCE_AVAILABLE:return "SOURCE_ONLY";
 default:return "RESEARCH";
 }
}
int spmp_host_list_cores(void){
 puts("SPMP-OS native core registry (NOT console-ready):");
 for(size_t i=0;i<spmp_core_count();i++){
  const spmp_core_desc *c=spmp_core_at(i);
  printf("%-13s %-11s %-30s sources: %s\n",c->id,state_label(c->state),c->title,c->source_archive);
 }
 return 0;
}
int spmp_host_resolve_file(const char *file){
 const spmp_core_desc *found[20];size_t total=0;
 spmp_match_result result;
 FILE *fp=fopen(file,"rb");
 if(fp){
  unsigned char header[65536];size_t read=fread(header,1,sizeof header,fp);
  fclose(fp);
  result=spmp_core_resolve_data(file,header,read,found,20,&total);
 }else result=spmp_core_resolve(file,found,20,&total);
 printf("ROM: %s\nMatch: %s (%zu core(s))\n",file,
  result==SPMP_MATCH_AMBIGUOUS?"AMBIGUOUS":result==SPMP_MATCH_UNIQUE?"UNIQUE":"UNKNOWN",total);
 for(size_t i=0;i<total&&i<20;i++)printf("  %s [%s]\n",found[i]->id,state_label(found[i]->state));
 puts("No ROM execution is attempted by the host dispatcher.");
 return 0;
}
int spmp_host_run_core(const char *id,unsigned ticks,const char *out,const char *content){
 cli_context ctx={out,0};
 spmp_core_host host={.user=&ctx,.present=host_present,.buttons=host_buttons,.read_entire=host_read_entire,.file_size=host_file_size};
 if(!ticks||ticks>1000){fprintf(stderr,"Invalid frames (1..1000)\n");return 2;}
 if(spmp_core_start(id,&host,content)){
  fprintf(stderr,"Core '%s' is not compiled/available for this host.\n",id);return 2;
 }
 int result=0;
 for(unsigned i=0;i<ticks;i++){
  int rc=spmp_core_tick();
  if(rc!=0){result=rc<0?1:0;break;}
 }
 spmp_core_shutdown();
 printf("Core: %s | Frame callbacks: %u\n",id,ctx.frames);
 if(out)printf("Output: %s\n",out);
 return ctx.frames==0?1:result;
}
