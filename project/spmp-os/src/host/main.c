#include "defos.h"
#include "spmp_apps.h"
#include "spmp_buttons.h"
#include "spmp_media.h"
#include "spmp_cores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int spmp_host_list_cores(void);
extern int spmp_host_resolve_file(const char *file);
extern int spmp_host_run_core(const char *id,unsigned frames,const char *out,const char *content);
extern void *host_fs_init(const char *root);
extern int host_fs_list(void *ctx,const char*,defos_entry*,size_t);
static int save_ppm(const char *path,const uint16_t *px) {
 FILE *f=fopen(path,"wb");if(!f)return -1;
 fprintf(f,"P6\n320 240\n255\n");
 for(int i=0;i<320*240;i++) {
   int p=px[i];unsigned char c[3]={
    (unsigned char)((((p>>11)&31)*255+15)/31),
    (unsigned char)((((p>>5)&63)*255+31)/63),
    (unsigned char)(((p&31)*255+15)/31)};
   if(fwrite(c,1,3,f)!=3){fclose(f);return -1;}
 }
 return fclose(f);
}
static void show(const defos_model *m) {
 printf("\nSPMP-OS STAGE2 | /%s | %s\n",m->relative,m->message);
 for(size_t i=0;i<m->count;i++) {
   printf("%s %-5s %s\n",i==m->selected?">":" ",
       defos_kind_name(m->entries[i].kind),m->entries[i].name);
 }
 puts("Commands: j/down, k/up, o/open, b/back, r/refresh, q/quit");
}
int main(int argc,char **argv) {
 const char *root=NULL,*shot=NULL,*theme_path=NULL,*button_path=NULL,*inspect=NULL;
 int home=0,cores=0;const char *resolve=NULL,*run_core=NULL,*content=NULL;unsigned frames=3;
 for(int i=1;i<argc;i++){
  if(!strcmp(argv[i],"--cores"))cores=1;
  else if(!strcmp(argv[i],"--resolve")&&i+1<argc)resolve=argv[++i];
  else if(!strcmp(argv[i],"--run-core")&&i+1<argc)run_core=argv[++i];
  else if(!strcmp(argv[i],"--content")&&i+1<argc)content=argv[++i];
  else if(!strcmp(argv[i],"--frames")&&i+1<argc){char *end=0;unsigned long n=strtoul(argv[++i],&end,10);if(!end||*end||n<1||n>1000){fprintf(stderr,"frames must be 1..1000\n");return 2;}frames=(unsigned)n;}
  else if(!strcmp(argv[i],"--root")&&i+1<argc)root=argv[++i];
  else if(!strcmp(argv[i],"--screenshot")&&i+1<argc)shot=argv[++i];
  else if(!strcmp(argv[i],"--theme")&&i+1<argc)theme_path=argv[++i];
  else if(!strcmp(argv[i],"--buttons")&&i+1<argc)button_path=argv[++i];
  else if(!strcmp(argv[i],"--inspect")&&i+1<argc)inspect=argv[++i];
  else if(!strcmp(argv[i],"--home"))home=1;
  else {
   fprintf(stderr,"Usage: %s [--cores|--resolve ROM|--run-core ID --frames N --screenshot PPM] [--home] [--root DIRECTORY] [--theme INI] [--buttons INI] [--inspect FILE] [--screenshot PPM]\n",argv[0]);
   return 2;
  }
 }
 spmp_theme theme;spmp_theme_default(&theme);
 if(theme_path && spmp_theme_load(&theme,theme_path)){
  fprintf(stderr,"Cannot load theme: %s\n",theme_path);return 2;
 }
 spmp_button_map buttons;spmp_button_map_default(&buttons);
 if(button_path && spmp_button_map_load(&buttons,button_path)){
  fprintf(stderr,"Cannot load button map: %s\n",button_path);return 2;
 }
 if(cores)return spmp_host_list_cores();
 if(resolve)return spmp_host_resolve_file(resolve);
 if(run_core)return spmp_host_run_core(run_core,frames,shot,content);
 if(inspect){
  unsigned char header[32];FILE *f=fopen(inspect,"rb");
  if(!f){perror("inspect");return 1;}
  size_t got=fread(header,1,sizeof header,f);fclose(f);
  printf("Content signature: %s\n",spmp_format_name(spmp_probe_header(header,got)));
  return 0;
 }
 if(home){
  size_t n;const spmp_app *apps=spmp_get_apps(&n);
  printf("SPMP-OS HOST STAGE2 | Theme: %s | modules: %zu\n",theme.title,n);
  for(size_t i=0;i<n;i++)printf("  %-9s %-10s %s\n",apps[i].id,
    apps[i].status==SPMP_HOST_PROTOTYPE?"prototype":"planned",apps[i].description);
  printf("Button map: ");
  for(size_t i=0;i<SPMP_ACTIONS;i++)printf("%s=%s%s",spmp_button_action(i),buttons.mapping[i],i+1==SPMP_ACTIONS?"\n":"; ");
  if(shot){
   uint16_t *p=malloc(320*240*sizeof(*p));if(!p)return 1;
   spmp_render_home(&theme,p);int ret=save_ppm(shot,p);free(p);
   if(ret)perror("Save screenshot");else printf("Screenshot: %s\n",shot);
   return ret?1:0;
  }
  return 0;
 }
 if(!root){fprintf(stderr,"Missing --root or --home\n");return 2;}
 void *fs=host_fs_init(root);
 if(!fs){perror("Cannot open root");return 1;}
 defos_model *m=calloc(1,sizeof(*m));
 uint16_t *fb=malloc(320*240*sizeof(*fb));
 if(!m||!fb){perror("Memory");free(m);free(fb);return 1;}
 defos_init(m,host_fs_list,fs);m->theme=theme;
 if(shot){defos_render(m,fb);int ret=save_ppm(shot,fb);
   if(ret)perror("Save screenshot");else printf("Screenshot: %s\n",shot);
   free(m);free(fb);return ret?1:0;
 }
 show(m);char line[64];
 while(fgets(line,sizeof line,stdin)){
  switch(line[0]){
   case 'j':case 's':defos_move(m,1);break;
   case 'k':case 'w':defos_move(m,-1);break;
   case 'o':case '\n':defos_enter(m);break;
   case 'b':defos_back(m);break;
   case 'r':defos_refresh(m);break;
   case 'q':free(m);free(fb);return 0;
   default:puts("Unknown command");
  }
  show(m);
 }
 free(m);free(fb);return 0;
}
