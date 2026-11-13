/* Host-only reference harness; the hardware HAL is intentionally separate. */
#include "spmp_mm.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int mm_host_photo(const char *filename,mm_type type,uint16_t *fb);
static int file_read(void *ctx,uint64_t at,void *dst,size_t size){
 FILE *f=ctx;
 if(at>(uint64_t)LONG_MAX||fseek(f,(long)at,SEEK_SET))return -1;
 return fread(dst,1,size,f)==size?0:-1;
}
static int open_reader(const char *name,mm_reader *r){
 FILE *f=fopen(name,"rb");if(!f)return -1;
 if(fseek(f,0,SEEK_END)){fclose(f);return -1;}
 long sz=ftell(f);if(sz<0){fclose(f);return -1;}
 r->ctx=f;r->size=(uint64_t)sz;r->read_at=file_read;return 0;
}
static int output_ppm(const char *name,const uint16_t *pixels){
 FILE *f=fopen(name,"wb");if(!f)return -1;
 if(fprintf(f,"P6\n320 240\n255\n")<0){fclose(f);return -1;}
 for(unsigned i=0;i<320u*240u;i++){
  uint16_t c=pixels[i];uint8_t rgb[3]={
   (uint8_t)((c>>11)*255/31),(uint8_t)(((c>>5)&63)*255/63),(uint8_t)((c&31)*255/31)};
  if(fwrite(rgb,1,3,f)!=3){fclose(f);return -1;}
 }
 return fclose(f)==0?0:-1;
}
static int output_audio(void *ctx,const int16_t *samples,size_t frames,unsigned rate){
 (void)rate;return fwrite(samples,sizeof(*samples)*2,frames,(FILE*)ctx)==frames?0:-1;
}
static int read_skin(const char *path,mm_skin *skin){
 FILE *f=fopen(path,"rb");if(!f)return -1;
 char text[4097];size_t n=fread(text,1,sizeof(text),f);
 int fail=ferror(f)||n>4096;
 fclose(f);return fail?-1:mm_skin_parse(skin,text,n);
}
static void usage(const char *prog){
 fprintf(stderr,"Usage: %s --inspect FILE | --home --output out.ppm [--theme theme.ini] [--select N] | --photo FILE --output out.ppm | --wav FILE --pcm out.raw\n",prog);
}
int main(int argc,char **argv){
 const char *inspect=NULL,*home=NULL,*photo=NULL,*wav=NULL,*pcm=NULL,*out=NULL,*theme=NULL;
 unsigned selected=0;
 for(int i=1;i<argc;i++){
  if(!strcmp(argv[i],"--inspect")&&i+1<argc)inspect=argv[++i];
  else if(!strcmp(argv[i],"--home"))home="yes";
  else if(!strcmp(argv[i],"--photo")&&i+1<argc)photo=argv[++i];
  else if(!strcmp(argv[i],"--wav")&&i+1<argc)wav=argv[++i];
  else if(!strcmp(argv[i],"--pcm")&&i+1<argc)pcm=argv[++i];
  else if(!strcmp(argv[i],"--output")&&i+1<argc)out=argv[++i];
  else if(!strcmp(argv[i],"--theme")&&i+1<argc)theme=argv[++i];
  else if(!strcmp(argv[i],"--select")&&i+1<argc){
   char *end;unsigned long v=strtoul(argv[++i],&end,10);
   if(!*argv[i]||*end||v>=6){usage(argv[0]);return 2;}selected=(unsigned)v;
  } else {usage(argv[0]);return 2;}
 }
 unsigned modes=(inspect!=NULL)+(home!=NULL)+(photo!=NULL)+(wav!=NULL);
 if(modes!=1||(home&&!out)||(photo&&!out)||(wav&&!pcm)){
  usage(argv[0]);return 2;
 }
 if(home){
  uint16_t *fb=malloc(320*240*2);mm_skin skin;mm_skin_default(&skin);
  if(!fb)return 1;
  if(theme&&read_skin(theme,&skin)){fprintf(stderr,"Invalid theme\n");free(fb);return 1;}
  mm_draw_home(fb,320,&skin,selected,NULL);
  int rc=output_ppm(out,fb);free(fb);
  if(rc){perror("PPM output");return 1;}
  printf("Theme: %s | home preview: %s\n",skin.name,out);return 0;
 }
 const char *input=inspect?inspect:photo?photo:wav;
 mm_reader r;if(open_reader(input,&r)){perror(input);return 1;}
 mm_info info;int rc=mm_inspect(&r,&info);
 if(rc){fprintf(stderr,"Unsupported or malformed media\n");fclose(r.ctx);return 1;}
 if(inspect){
  printf("format=%s\n",mm_type_name(info.type));
  printf("width=%u\nheight=%u\n",info.width,info.height);
  printf("audio_codec=%s\nvideo_codec=%s\n",info.audio_codec,info.video_codec);
  printf("sample_rate=%u\nchannels=%u\nbits=%u\n",info.sample_rate,info.channels,info.bits_per_sample);
  printf("duration_ms=%u\n",info.duration_ms);
  fclose(r.ctx);return 0;
 }
 if(photo){
  uint16_t *fb=malloc(320*240*2);
  if(!fb){fclose(r.ctx);return 1;}
  if(info.type==MM_BMP)rc=mm_render_bmp(&r,fb,320,320,240);
  else rc=mm_host_photo(photo,info.type,fb);
  fclose(r.ctx);
  if(!rc)rc=output_ppm(out,fb);
  free(fb);if(rc){fprintf(stderr,"Cannot render photo\n");return 1;}
  printf("Photo: %s %ux%u -> %s\n",mm_type_name(info.type),info.width,info.height,out);
  return 0;
 }
 if(info.type!=MM_WAV){fprintf(stderr,"Only PCM WAV is implemented in the portable audio path\n");fclose(r.ctx);return 1;}
 FILE *stream=fopen(pcm,"wb");if(!stream){perror(pcm);fclose(r.ctx);return 1;}
 mm_audio_output audio={stream,output_audio};uint32_t frames=0;
 rc=mm_play_wav(&r,&audio,&frames);
 if(fclose(stream))rc=-1;
 fclose(r.ctx);
 if(rc){fprintf(stderr,"WAV streaming failure\n");return 1;}
 printf("WAV frames=%u sample_rate=%u stereo PCM written to %s\n",frames,info.sample_rate,pcm);
 return 0;
}
